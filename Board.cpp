#include "Board.hpp"
#include "MoveGenerator.h"

#include <ostream>
#include <iostream>
#include <algorithm>
#include <bitset>
#include <memory>
#include <random>

#define FILE_A 0x0101010101010101L
#define FILE_B 0x0202020202020202L
#define FILE_C 0x0404040404040404L
#define FILE_D 0x0808080808080808L
#define FILE_E 0x1010101010101010L
#define FILE_F 0x2020202020202020L
#define FILE_G 0x4040404040404040L
#define FILE_H 0x8080808080808080L
#define RANK_1 0x00000000000000FFL
#define RANK_2 0x000000000000FF00L
#define RANK_3 0x0000000000FF0000L
#define RANK_4 0x00000000FF000000L
#define RANK_5 0x000000FF00000000L
#define RANK_6 0x0000FF0000000000L
#define RANK_7 0x00FF000000000000L
#define RANK_8 0xFF00000000000000L

#define bit 1UL
#define NBB 12
#define NSQ 64

using U64 = uint64_t;

Board::Board() {

    // initialize bitboards
    bitboards = std::vector<U64>(NBB);
    for (int i = 0; i < NBB; i++) {
        bitboards[i] = 0UL;
    }

    turn_ = PieceColor::White;
    cr_ = CastlingRights::All;
}

bool Board::isNewGame() const {
    if (turn_ == PieceColor::White &&
        bitboards[0] == 16UL &&
        bitboards[1] == 8UL &&
        bitboards[2] == 129UL &&
        bitboards[3] == 36UL &&
        bitboards[4] == 66UL &&
        bitboards[5] == 65280UL)
        return true;
    if (turn_ == PieceColor::Black &&
        bitboards[6] == 1152921504606846976UL &&
        bitboards[7] == 576460752303423488UL &&
        bitboards[8] == 9295429630892703744UL &&
        bitboards[9] == 2594073385365405696UL &&
        bitboards[10] == 4755801206503243776UL &&
        bitboards[11] == 71776119061217280UL)
        return true;
    return false;
}

/*
 * Places the given piece on the given square.
 */
void Board::setPiece(const Square &square, const Piece::Optional &piece) {
    if (piece) {
        // if square is occupied, free square first
        removePiece(square);

        // set piece
        auto idx = bitboardMap.at(*piece);
        bitboards[idx] |= bit << square.index();
    }
}

/*
 * Removes the piece on the given square.
 */
void Board::removePiece(const Square &square) {
    for (auto &bitboard: bitboards) {
        bitboard &= ~(bit << square.index());
    }
}

/*
 * Returns the piece on the given square.
 *
 * If the square is empty, returns std::nullopt.
 * If the given argument is not a valid square, returns std::nullopt.
 */
Piece::Optional Board::piece(const std::optional<Square> &square) const {
    if (square.has_value())
        return piece(square.value());
    else
        return std::nullopt;
}

/*
 * Returns the piece on the given square.
 *
 * If the square is empty, returns std::nullopt.
 */
Piece::Optional Board::piece(const Square &square) const {
    auto idx = square.index();
    return piece(idx);
}

/*
 * Returns the piece on the given index.
 *
 * If the square is empty, returns std::nullopt.
 */
Piece::Optional Board::piece(unsigned int idx) const {
    U64 mask = bit << idx;

    for (int i = 0; i < NBB; i++) {
        auto bitboard = this->bitboards[i];
        if (bitboard & mask) {
            return bitboardTypes[i];
        }
    }

    return std::nullopt;
}

void Board::setTurn(PieceColor turn) {
    turn_ = turn;
}

PieceColor Board::turn() const {
    return turn_;
}

void Board::setCastlingRights(CastlingRights cr) {
    cr_ = cr;
}

CastlingRights Board::castlingRights() const {
    return cr_;
}

void Board::setEnPassantSquare(const Square::Optional &square) {
    enPassantSquare_ = *square;
}

Square::Optional Board::enPassantSquare() const {
    return enPassantSquare_;
}

/*
 * Updates the internal board structure to reflect the given move.
 *
 * Does not check if the move is legal since this function is only called from the MoveGenerator, where we know only
 * legal moves are propagated.
 */
void Board::makeMove(const Move &move) {
    auto from = move.from();
    auto to = move.to();
    // we know that the given move is valid, no need to check for null
    auto p = piece(from).value();

    // CASTLING //
    // unset castling rights when piece moved
    if (p.type() == PieceType::King) {
        if (p.color() == PieceColor::White) {
            cr_ &= ~CastlingRights::White;
        } else {
            cr_ &= ~CastlingRights::Black;
        }
    } else if (p.type() == PieceType::Rook) {
        if (p.color() == PieceColor::White) {
            if (from == Square::A1) {
                cr_ &= ~CastlingRights::WhiteQueenside;
            } else if (from == Square::H1) {
                cr_ &= ~CastlingRights::WhiteKingside;
            }
        } else {
            if (from == Square::A8) {
                cr_ &= ~CastlingRights::BlackQueenside;
            } else if (from == Square::H8) {
                cr_ &= ~CastlingRights::BlackKingside;
            }
        }
    }

    // unset castling rights when rook captured
    if (to == Square::A1 && piece(Square::A1) == Piece::WhiteRook) {
        cr_ &= ~CastlingRights::WhiteQueenside;
    } else if (to == Square::H1 && piece(Square::H1) == Piece::WhiteRook) {
        cr_ &= ~CastlingRights::WhiteKingside;
    } else if (to == Square::A8 && piece(Square::A8) == Piece::BlackRook) {
        cr_ &= ~CastlingRights::BlackQueenside;
    } else if (to == Square::H8 && piece(Square::H8) == Piece::BlackRook) {
        cr_ &= ~CastlingRights::BlackKingside;
    }

    // move rook when castling
    if (p.type() == PieceType::King) {
        if (from == Square::E1 && to == Square::G1) {
            removePiece(Square::H1);
            setPiece(Square::F1, Piece::WhiteRook);
        } else if (from == Square::E1 && to == Square::C1) {
            removePiece(Square::A1);
            setPiece(Square::D1, Piece::WhiteRook);
        } else if (from == Square::E8 && to == Square::G8) {
            removePiece(Square::H8);
            setPiece(Square::F8, Piece::BlackRook);
        } else if (from == Square::E8 && to == Square::C8) {
            removePiece(Square::A8);
            setPiece(Square::D8, Piece::BlackRook);
        }
    }

    // EN PASSANT //
    // Remove piece after en passant capture
    if (p.type() == PieceType::Pawn && to == enPassantSquare_) {
        if (p.color() == PieceColor::White) {
            removePiece(Square::fromCoordinates(to.file(), to.rank() - 1).value());
        } else {
            removePiece(Square::fromCoordinates(to.file(), to.rank() + 1).value());
        }
    }

    // Set en passant to null at the end of the turn
    if (enPassantSquare_)
        enPassantSquare_ = std::nullopt;

    // Set en passant flag
    if (p.type() == PieceType::Pawn) {
        auto left = piece(Square::fromCoordinates(to.file() - 1, to.rank()));
        auto right = piece(Square::fromCoordinates(to.file() + 1, to.rank()));
        if (p.color() == PieceColor::White) {
            if (from.rank() == 1 && to.rank() == 3 &&
                ((left.has_value() && left == Piece::BlackPawn) ||
                 (right.has_value() && right == Piece::BlackPawn))) {
                enPassantSquare_ = Square::fromCoordinates(to.file(), 2).value();
            }
        } else {
            if (from.rank() == 6 && to.rank() == 4 &&
                ((left.has_value() && left == Piece::WhitePawn) ||
                 (right.has_value() && right == Piece::WhitePawn))) {
                enPassantSquare_ = Square::fromCoordinates(to.file(), 5).value();
            }
        }
    }

    // MOVE PIECE //
    // remove piece from `from` square
    removePiece(from);

    // set piece on `to` square, if that square is occupied, it is removed first
    setPiece(to, p);

    // PROMOTION //
    if (move.promotion()) {
        removePiece(to);
        setPiece(to, Piece(p.color(), move.promotion().value()));
    }

    // SWITCH TURN //
    turn_ = !turn_;
}

/*
 * Generates all possible pseudo-legal moves for the current board state and puts them in the given vector.
 */
void Board::pseudoLegalMoves(MoveVec &moves) const {
    for (int i = 0; i < NSQ; i++) {
        auto const from = Square::fromIndex(i).value();

        pseudoLegalMovesFrom(from, moves);
    }
}

/*
 * Generates all pseudo-legal moves from the given square on the current board state and puts them in the given vector.
 */
void Board::pseudoLegalMovesFrom(const Square &from,
                                 Board::MoveVec &moves) const {
    auto empty = getEmptySquares();
    auto enemy = getEnemySquares(turn_);

    auto p = piece(from);
    if (p && p->color() == turn_) {
        U64 moveBitboard;

        switch (p->type()) {
            case PieceType::Pawn: {
                moveBitboard = MoveGenerator::pawnMoves(from, empty, enemy, turn_, enPassantSquare_);
                break;
            }
            case PieceType::Knight: {
                moveBitboard = MoveGenerator::knightMoves(from, empty, enemy);
                break;
            }
            case PieceType::Bishop: {
                moveBitboard = MoveGenerator::bishopMoves(from, empty, enemy);
                break;
            }
            case PieceType::Rook: {
                moveBitboard = MoveGenerator::rookMoves(from, empty, enemy);
                break;
            }
            case PieceType::Queen: {
                moveBitboard = MoveGenerator::queenMoves(from, empty, enemy);
                break;
            }
            case PieceType::King: {
                moveBitboard = MoveGenerator::kingMoves(*this, from, empty, enemy, true);
                break;
            }
        }

        auto newMoves = generateMovesFromBitboard(from, moveBitboard);

        // this checks whether the pawn can move to the last rank and if so, it adds the promotion moves
        if (p->type() == PieceType::Pawn &&
            ((p->color() == PieceColor::White && from.rank() == 6) ||
             (p->color() == PieceColor::Black && from.rank() == 1))) {

            for (Move m : newMoves) {
                moves.push_back(Move(m.from(), m.to(), PieceType::Queen));
                moves.push_back(Move(m.from(), m.to(), PieceType::Rook));
                moves.push_back(Move(m.from(), m.to(), PieceType::Bishop));
                moves.push_back(Move(m.from(), m.to(), PieceType::Knight));
            }
        } else {
            moves.insert(moves.end(), newMoves.begin(), newMoves.end());
        }
    }

}

/*
 * Returns a bitboard representing all squares that are empty.
 */
U64 Board::getEmptySquares() const {
    U64 occupiedSquares = 0UL;
    for (auto bitboard: bitboards)
        occupiedSquares |= bitboard;

    return ~occupiedSquares;
}

/*
 * Returns a bitboard representing all squares that are occupied by an enemy piece.
 */
U64 Board::getEnemySquares(PieceColor color) const {
    U64 enemySquares = 0UL;
    int offset = color == PieceColor::White ? 6 : 0;
    for (int i = 0; i < 6; i++)
        enemySquares |= bitboards[i + offset];
    return enemySquares;
}


/*
 * Returns a bitboard representing all the squares that are controlled by an enemy piece.
 * Controlled in this context means that the enemy either already occupies that square or can move to that square in one move.
 */
U64 Board::getEnemyControlledSquares(PieceColor pieceColor) const {
    auto offset = pieceColor == PieceColor::White ? 6 : 0;

    U64 controlled = 0UL;
    auto enemy = getEnemySquares(!pieceColor);
    auto empty = getEmptySquares();

    // loop over all enemy bitboards and generate moves for each piece type
    for (auto i = 0; i < 6; i++) {
        auto p = bitboardTypes[i + offset];
        auto bb = bitboards[i + offset];

        controlled |= bb;

        for (auto j = 0; j < NSQ; j++) {
            if (bb & (bit << j)) {
                auto square = Square::fromIndex(j).value();
                switch (p.type()) {
                    case PieceType::Pawn:
                        controlled |= MoveGenerator::pawnAttacks(square, !pieceColor);
                        break;
                    case PieceType::Knight:
                        controlled |= MoveGenerator::knightMoves(square, empty, enemy);
                        break;
                    case PieceType::Bishop:
                        controlled |= MoveGenerator::bishopMoves(square, empty, enemy);
                        break;
                    case PieceType::Rook:
                        controlled |= MoveGenerator::rookMoves(square, empty, enemy);
                        break;
                    case PieceType::Queen:
                        controlled |= MoveGenerator::queenMoves(square, empty, enemy);
                        break;
                    case PieceType::King:
                        // since castling does not contribute to the controlled squares,
                        // we can set the `allowCastling` parameter to `false` to save some moves
                        controlled |= MoveGenerator::kingMoves(*this, square, empty, enemy, false);
                        break;
                }
            }
        }
    }

    return controlled;
}

/*
 * Returns the population count of the given bitboard.
 * The population count is the number of bits that are set to 1.
 *
 * Brian Kernighan's way.
 * source: https://www.chessprogramming.org/Population_Count
 */
int Board::popCount(U64 bb) {
    int count = 0;
    while (bb) {
        count++;
        bb &= bb - 1;
    }
    return count;
}

/*
 * Returns a string representation of the board.
 */
std::string Board::toString() const {
    std::string board[8] = {
            "........",
            "........",
            "........",
            "........",
            "........",
            "........",
            "........",
            "........"
    };


    // iterate over bitboards, check which squares are set
    for (int i = 0; i < NBB; i++) {
        auto bb = bitboards[i];
        for (int j = 0; j < NSQ; j++) {
            if (bb & (bit << j)) { // check if it is occupied
                auto row = j / 8;
                auto col = j % 8;
                board[row][col] = bitboardTypes[i].toSymbol();
            }
        }
    }

    // reverse the board to print correctly
    std::reverse(board, board + 8);

    // join the board into a string
    std::string result;
    for (auto &s: board)
        result += s + '\n';

    return result;
}

/*
 * Prints the given bitboard to the console.
 */
void Board::printBitBoard(U64 bitBoard) {
    for (int i = 0; i < NSQ; i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        std::cout << ((bitBoard & (1UL << i)) ? 1 : 0);
    }
    std::cout << std::endl;
}

/*
 * Generates a Move from the given `from` and bitboard.
 * The bitboard has a `1` on exactly one square, which is the destination square of the move.
 */
Board::MoveVec Board::generateMovesFromBitboard(const Square &from, const U64 &bitboard) {
    MoveVec moves;
    for (int i = 0; i < NSQ; i++) {
        if (bitboard & (bit << i)) {
            auto to = Square::fromIndex(i).value();
            auto move = Move(from, to);
            moves.push_back(move);
        }
    }
    return moves;
}

std::ostream &operator<<(std::ostream &os, const Board &board) {
    return os << board.toString();
}

/*
 * The type of the bitboard that is stored in the `bitboards` array.
 */
const Piece Board::bitboardTypes[12] = {
        Piece(PieceColor::White, PieceType::King),
        Piece(PieceColor::White, PieceType::Queen),
        Piece(PieceColor::White, PieceType::Rook),
        Piece(PieceColor::White, PieceType::Bishop),
        Piece(PieceColor::White, PieceType::Knight),
        Piece(PieceColor::White, PieceType::Pawn),
        Piece(PieceColor::Black, PieceType::King),
        Piece(PieceColor::Black, PieceType::Queen),
        Piece(PieceColor::Black, PieceType::Rook),
        Piece(PieceColor::Black, PieceType::Bishop),
        Piece(PieceColor::Black, PieceType::Knight),
        Piece(PieceColor::Black, PieceType::Pawn)
};

/*
 * Maps a piece type to the corresponding bitboard index.
 */
const std::map<Piece, int> Board::bitboardMap = {
        {Piece(PieceColor::White, PieceType::King),   0},
        {Piece(PieceColor::White, PieceType::Queen),  1},
        {Piece(PieceColor::White, PieceType::Rook),   2},
        {Piece(PieceColor::White, PieceType::Bishop), 3},
        {Piece(PieceColor::White, PieceType::Knight), 4},
        {Piece(PieceColor::White, PieceType::Pawn),   5},
        {Piece(PieceColor::Black, PieceType::King),   6},
        {Piece(PieceColor::Black, PieceType::Queen),  7},
        {Piece(PieceColor::Black, PieceType::Rook),   8},
        {Piece(PieceColor::Black, PieceType::Bishop), 9},
        {Piece(PieceColor::Black, PieceType::Knight), 10},
        {Piece(PieceColor::Black, PieceType::Pawn),   11}
};

std::string Board::INITIAL_BOARD_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/*
 * Checks whether the given color is in check for the current board state.
 */
bool Board::isCheck(PieceColor c) const {
    U64 enemyControlled = getEnemyControlledSquares(c);
    auto offset = c == PieceColor::White ? 0 : 6;
    auto king = bitboards[0 + offset];
    return enemyControlled & king;
}

/*
 * Checks whether the given color is in checkmate for the current board state.
 */
bool Board::isCheckMate(PieceColor c) const {

    // color can't be in checkmate if it is not in check
    if (!isCheck(c))
        return false;

    std::vector<Move> moves;
    pseudoLegalMoves(moves);
    for (auto m : moves) {
        auto newBoard = *this;
        newBoard.makeMove(m);
        // if the color is not in check after the move, it is not checkmate
        if (!newBoard.isCheck(c)){
            return false;
        }
    }

    // if there are no moves that remove the check, it is checkmate
    return true;
}

bool Board::isStaleMate(PieceColor c) const {

    // color can't be stalemate if it is in check
    if (isCheck(c))
        return false;

    std::vector<Move> moves;
    pseudoLegalMoves(moves);
    for (auto m : moves) {
        auto newBoard = *this;
        newBoard.makeMove(m);
        // if the color is not in check after the move, it is not checkmate
        if (!newBoard.isCheck(c)){
            return false;
        }
    }

    // if there are no moves that remove the check, it is checkmate
    return true;
}

std::vector<U64> Board::getBitboards() const {
    return bitboards;
}

int Board::moveScore(const Move &move) const {
    auto p = piece(move.to());
    if (p)
        return p->value();
    else
        return 0;
}

bool Board::compareMoves(const Move &lhs, const Move &rhs) const {
    return moveScore(lhs) > moveScore(rhs);
}

void Board::initHashKeys() {
    for (auto & row : hashTable) {
        for (unsigned long & el : row) {
            el = randLong();
        }
    }

    blackHash = randLong();
}

U64 Board::hash() const {
    U64 h = 0;
    if (turn_ == PieceColor::Black)
        h ^= blackHash;

    for (int i = 0; i < NSQ; i++) {
        auto p = piece(i);
        if (p)
            h ^= hashTable[i][bitboardMap.at(p.value())];
    }

    return h;
}

/*
 * Returns a random 64-bit integer.
 */
U64 Board::randLong() {
    std::default_random_engine generator;
    std::uniform_int_distribution<U64> distribution(0, std::numeric_limits<U64>::max());
    return distribution(generator);
}
