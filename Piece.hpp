#ifndef CHESS_ENGINE_PIECE_HPP
#define CHESS_ENGINE_PIECE_HPP

#include <optional>
#include <iosfwd>

enum class PieceColor {
    White,
    Black
};

enum class PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

class Piece {
public:

    using Optional = std::optional<Piece>;

    Piece(PieceColor color, PieceType type);

    static Optional fromSymbol(char symbol);

    [[nodiscard]] char toSymbol() const;

    [[nodiscard]] PieceColor color() const;

    [[nodiscard]] PieceType type() const;

    [[nodiscard]] int value() const;

    static const Piece WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing;
    static const Piece BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing;

private:
    PieceColor color_;
    PieceType type_;
};

bool operator==(const Piece &lhs, const Piece &rhs);

bool operator<(const Piece &lhs, const Piece &rhs);

std::ostream &operator<<(std::ostream &os, const Piece &piece);

// Invert a color (White becomes Black and vice versa)
PieceColor operator!(PieceColor color);

std::ostream &operator<<(std::ostream &os, PieceType type);

#endif
