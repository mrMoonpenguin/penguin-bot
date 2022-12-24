#include <cstring>
#include <iostream>
#include <queue>
#include <algorithm>
#include "Engine.hpp"
#include "Fen.hpp"
#include "Evaluate.h"

#define DEPTH 11 // maximum depth of search

std::optional<HashInfo> Engine::hashInfo() const {
    return std::nullopt;
}

void Engine::setHashSize(std::size_t) {}

/*
 * Returns the principal variation of the current board state.
 */
PrincipalVariation ChessEngine::pv(const Board &board, const TimeInfo::Optional &timeInfo) {
    if (board.isCheckMate(board.turn()))
        return {{}, board.turn(), 0, true};

    if (board.isStaleMate(board.turn()))
        return {{}, board.turn(), 0, false};

    // very basic opening, mostly to avoid timeout in lichess bot games
    if (isInitialBoard && board.isNewGame()) {
        // set the initial board to false
        isInitialBoard = false;

        auto newBoard = board;
        auto who2move = newBoard.turn() == PieceColor::White ? 1 : -1;
        if (board.turn() == PieceColor::White) {
            auto move = Move::fromUci("e2e4").value();
            newBoard.makeMove(move);
            return {{move}, board.turn(), Evaluate::evaluate(newBoard, who2move), false};
        } else {
            auto move = Move::fromUci("e7e5").value();
            newBoard.makeMove(move);
            return {{move}, board.turn(), Evaluate::evaluate(newBoard, who2move), false};
        }
    }

    // perform iterative deepening
    auto PV = iterativeDeepening(board, timeInfo);

//    // add board state to map of board states
//    auto newBoard = board.copy();
//    newBoard.makeMove(PV.moves_[0]);
//    if (boardStates.contains(newBoard.hash()))
//        boardStates[newBoard.hash()]++;
//    else
//        boardStates[newBoard.hash()] = 1;

    return PV;
}

/*
 * Returns a principal variation for the given board, this is done by using iterative deepening.
 *
 * ID starts at depth 1 and increases the depth until the maximum depth (DEPTH) is reached. This function returns early
 * when a checkmate is found, this to prevent unnecessary searching.
 *
 * TODO: implement time management
 */
PrincipalVariation ChessEngine::iterativeDeepening(const Board &board, const TimeInfo::Optional &timeInfo) {
    LINE out;
    int maxDepth = DEPTH;
    bool mate = false;
    int color = board.turn() == PieceColor::White ? 1 : -1;
    long score = INT32_MIN;

    std::chrono::milliseconds time = moveTime(board.turn(), timeInfo.value());
    bool timeOut = false;

    // set depth to 7 if no time info is given
    if (!timeInfo.has_value())
        maxDepth = 7;

    for (int i = 1; i <= maxDepth; i++) {
        if (timeOut)
            break;

        LINE line;
        long newScore;
        if (timeInfo) {
            auto start = std::chrono::high_resolution_clock::now();
            newScore = negamax(board, i, 0, -INT64_MAX, INT64_MAX, color, &line);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            // if the elapsed time for previous iteration is greater than 50% of the time left, return the previous PV
            if (elapsed > 0.5 * time)
                timeOut = true;
        } else {
            newScore = negamax(board, i, 0, -INT64_MAX, INT64_MAX, color, &line);
        }

        // found checkmate, stop looking further
        if (newScore == INT32_MAX) {
            score = i;
            memcpy(&out, &line, sizeof(LINE));
            mate = true;
            break;
        }

        score = newScore;
        memcpy(&out, &line, sizeof(LINE));
    }

    std::vector<Move> moves;
    for (int i = 0; i < out.cMove; i++)
        moves.push_back(out.argmove[i]);

    return {moves, board.turn(), score, mate};
}

/*
 * Negamax implementation with alpha-beta pruning.
 *
 * This function is the core of the chess engine. It uses negamax to search the game tree and alpha-beta pruning to
 * prune branches that are not promising.
 *
 * The function returns the score of the current board state. The score is calculated by the evaluation function.
 *
 * The function also fills the given line with the moves that lead to the current board state. This is used to
 * construct the principal variation.
 *
 * Source for negamax: https://www.chessprogramming.org/Negamax
 * Source for extracting the PV:
 *  https://web.archive.org/web/20071031100114/http://www.brucemo.com:80/compchess/programming/pv.htm
 */
long ChessEngine::negamax(const Board &board, int depth, int ply, long alpha, long beta, int color, LINE *pline) {
    LINE line;

    // reached maximum depth, return the score of the current board state
    if (depth == 0) {
        pline->cMove = 0;
        return Evaluate::evaluate(board, color);
    }

    // generate moves
    std::vector<Move> moves;
    board.pseudoLegalMoves(moves);

    // sort moves
    std::sort(moves.begin(), moves.end(), [&board](const Move &a, const Move &b) {
        return board.compareMoves(a, b);
    });

    for (auto m : moves) {
        auto turn = board.turn();
        Board b = board;

        // check for threefold repetition
        if (ply == 0 && boardStates[b.hash()] == 2)
            continue;

        b.makeMove(m);

        // ignore move if it results in a check
        if (b.isCheck(turn))
            continue;

        // return maximum score if checkmate is found
        if (b.isCheckMate(!turn)) {
            pline->cMove = 1;
            pline->argmove[0] = m;
            return INT32_MAX;
        }

        long score = -negamax(b, depth - 1, ply + 1, -beta, -alpha, -color, &line);

        if (score >= beta)
            return beta;

        if (score > alpha) {
            alpha = score;
            pline->argmove[0] = m;
            memcpy(pline->argmove + 1, line.argmove, sizeof(Move) * line.cMove);
            pline->cMove = line.cMove + 1;
        }
    }

    return alpha;
}

std::string ChessEngine::name() const {
    return name_;
}

std::string ChessEngine::version() const {
    return version_;
}

std::string ChessEngine::author() const {
    return author_;
}

void ChessEngine::newGame() {
    board_ = Fen::createBoard(Board::INITIAL_BOARD_FEN).value();
}

std::chrono::milliseconds ChessEngine::moveTime(PieceColor turn, TimeInfo ti) const {
    std::chrono::milliseconds timeLeft;
    if (turn == PieceColor::White)
        timeLeft = ti.white.timeLeft;
    else
        timeLeft = ti.black.timeLeft;

    // return time left divided by moves to go
    // built in buffer of two extra moves to account for time lost
    if (ti.movesToGo.has_value())
        return timeLeft / (ti.movesToGo.value() + 2);

    // calculate time per move
    auto timePerMove = timeLeft / 20;
    return timePerMove;
}
