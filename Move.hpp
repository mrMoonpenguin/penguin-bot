#ifndef CHESS_ENGINE_MOVE_HPP
#define CHESS_ENGINE_MOVE_HPP

#include "Square.hpp"
#include "Piece.hpp"

#include <iosfwd>
#include <optional>
#include <string>

class Board;

class Move {
public:

    using Optional = std::optional<Move>;

    Move();
    Move(const Square& from, const Square& to,
         const std::optional<PieceType>& promotion = std::nullopt);

    static Optional fromUci(const std::string& uci);

    [[nodiscard]] Square from() const;
    [[nodiscard]] Square to() const;
    [[nodiscard]] std::optional<PieceType> promotion() const;

    void setPromotion(PieceType promotion);

private:
    Square from_;
    Square to_;
    std::optional<PieceType> promotion_;
};

std::ostream& operator<<(std::ostream& os, const Move& move);

// Needed for std::map, std::set
bool operator<(const Move& lhs, const Move& rhs);
bool operator==(const Move& lhs, const Move& rhs);

#endif
