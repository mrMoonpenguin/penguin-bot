#include "Piece.hpp"

#include <ostream>

const Piece Piece::WhitePawn = Piece(PieceColor::White, PieceType::Pawn);
const Piece Piece::WhiteKnight = Piece(PieceColor::White, PieceType::Knight);
const Piece Piece::WhiteBishop = Piece(PieceColor::White, PieceType::Bishop);
const Piece Piece::WhiteRook = Piece(PieceColor::White, PieceType::Rook);
const Piece Piece::WhiteQueen = Piece(PieceColor::White, PieceType::Queen);
const Piece Piece::WhiteKing = Piece(PieceColor::White, PieceType::King);
const Piece Piece::BlackPawn = Piece(PieceColor::Black, PieceType::Pawn);
const Piece Piece::BlackKnight = Piece(PieceColor::Black, PieceType::Knight);
const Piece Piece::BlackBishop = Piece(PieceColor::Black, PieceType::Bishop);
const Piece Piece::BlackRook = Piece(PieceColor::Black, PieceType::Rook);
const Piece Piece::BlackQueen = Piece(PieceColor::Black, PieceType::Queen);
const Piece Piece::BlackKing = Piece(PieceColor::Black, PieceType::King);

Piece::Piece(PieceColor color, PieceType type) : color_(color), type_(type) {}

/*
 * Constructs a Piece from a symbol.
 */
Piece::Optional Piece::fromSymbol(char symbol) {
    switch (symbol) {
        case 'P':
            return Piece(PieceColor::White, PieceType::Pawn);
        case 'N':
            return Piece(PieceColor::White, PieceType::Knight);
        case 'B':
            return Piece(PieceColor::White, PieceType::Bishop);
        case 'R':
            return Piece(PieceColor::White, PieceType::Rook);
        case 'Q':
            return Piece(PieceColor::White, PieceType::Queen);
        case 'K':
            return Piece(PieceColor::White, PieceType::King);
        case 'p':
            return Piece(PieceColor::Black, PieceType::Pawn);
        case 'n':
            return Piece(PieceColor::Black, PieceType::Knight);
        case 'b':
            return Piece(PieceColor::Black, PieceType::Bishop);
        case 'r':
            return Piece(PieceColor::Black, PieceType::Rook);
        case 'q':
            return Piece(PieceColor::Black, PieceType::Queen);
        case 'k':
            return Piece(PieceColor::Black, PieceType::King);
        default:
            return std::nullopt;
    }
}

/*
 * Converts a Piece to a symbol.
 */
char Piece::toSymbol() const {
    char symbol;

    switch (type_) {
        case PieceType::Pawn:
            symbol = 'p';
            break;
        case PieceType::Knight:
            symbol = 'n';
            break;
        case PieceType::Bishop:
            symbol = 'b';
            break;
        case PieceType::Rook:
            symbol = 'r';
            break;
        case PieceType::Queen:
            symbol = 'q';
            break;
        case PieceType::King:
            symbol = 'k';
            break;
        default:
            symbol = '?';
    }

    if (color_ == PieceColor::White) {
        symbol = toupper(symbol, std::locale());
    }

    return symbol;
}

PieceColor Piece::color() const {
    return color_;
}

PieceType Piece::type() const {
    return type_;
}

/*
 * Returns the value of the piece in centipawns.
 * Based on https://www.chessprogramming.org/Point_Value.
 */
int Piece::value() const {
    switch (type_) {
        case PieceType::Pawn:
            return 100;
        case PieceType::Knight:
            return 320;
        case PieceType::Bishop:
            return 330;
        case PieceType::Rook:
            return 500;
        case PieceType::Queen:
            return 900;
        case PieceType::King:
            return 20000;
        default:
            return 0;
    }
}

bool operator==(const Piece &lhs, const Piece &rhs) {
    return lhs.color() == rhs.color() && lhs.type() == rhs.type();
}

bool operator<(const Piece &lhs, const Piece &rhs) {
    return lhs.color() < rhs.color() || (lhs.color() == rhs.color() && lhs.type() < rhs.type());
}

std::ostream &operator<<(std::ostream &os, const Piece &piece) {
    return os << piece.toSymbol();
}

/*
 * Returns the opposite color of the given color.
 */
PieceColor operator!(PieceColor color) {
    if (color == PieceColor::White) {
        return PieceColor::Black;
    } else {
        return PieceColor::White;
    }
}

std::ostream &operator<<(std::ostream &os, const PieceType type) {
    std::string s = "Unknown";

    switch (type) {
        case PieceType::Pawn:
            s = "p";
            break;
        case PieceType::Knight:
            s = "n";
            break;
        case PieceType::Bishop:
            s = "b";
            break;
        case PieceType::Rook:
            s = "r";
            break;
        case PieceType::Queen:
            s = "q";
            break;
        case PieceType::King:
            s = "k";
            break;
    }

    return os << s;
}
