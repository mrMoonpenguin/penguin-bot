#ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"
#include "Piece.hpp"

#include <optional>
#include <iosfwd>
#include <vector>
#include <map>
#include <bitset>
#include <memory>

class Board {
public:

    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;
    using U64 = uint64_t;

    Board();

    void setPiece(const Square &square, const Piece::Optional &piece);

    void removePiece(const Square &square);

    [[nodiscard]] Piece::Optional piece(const Square &square) const;

    [[nodiscard]] Piece::Optional piece(unsigned int idx) const;

    [[nodiscard]] Piece::Optional piece(const std::optional<Square> &square) const;

    void setTurn(PieceColor turn);

    [[nodiscard]] PieceColor turn() const;

    void setCastlingRights(CastlingRights cr);

    [[nodiscard]] CastlingRights castlingRights() const;

    void setEnPassantSquare(const Square::Optional &square);

    [[nodiscard]] Square::Optional enPassantSquare() const;

    void makeMove(const Move &move);

    void pseudoLegalMoves(MoveVec &moves) const;

    void pseudoLegalMovesFrom(const Square &from, MoveVec &moves) const;

    [[nodiscard]] std::string toString() const;

//    [[nodiscard]] Board copy() const;

    [[nodiscard]] bool isCheck(PieceColor c) const;

    [[nodiscard]] bool isCheckMate(PieceColor c) const;

    [[nodiscard]] bool isStaleMate(PieceColor c) const;

    // move generating functions
    [[nodiscard]] U64 getEmptySquares() const;

    [[nodiscard]] U64 getEnemySquares(PieceColor pieceColor) const;

    [[nodiscard]] U64 getEnemyControlledSquares(PieceColor pieceColor) const;

    static MoveVec generateMovesFromBitboard(const Square &from, const U64 &bitboard);

    static void printBitBoard(U64 bitBoard);

    static int popCount(U64 bb);

    static std::string INITIAL_BOARD_FEN;

    static const Piece bitboardTypes[12];

    [[nodiscard]] std::vector<U64> getBitboards() const;

    [[nodiscard]] int moveScore(const Move &move) const;

    [[nodiscard]] bool compareMoves(const Move &lhs, const Move &rhs) const;

    void initHashKeys();

    [[nodiscard]] U64 hash() const;

    static U64 randLong();

    [[nodiscard]] bool isNewGame() const;

private:
    PieceColor turn_;
    CastlingRights cr_;
    std::optional<Square> enPassantSquare_ = std::nullopt;
    bool promotion = false;

    static const std::map<Piece, int> bitboardMap;

    std::vector<U64> bitboards;

    U64 hashTable[64][12] = {};

    U64 blackHash = 0;
};

std::ostream &operator<<(std::ostream &os, const Board &board);

#endif
