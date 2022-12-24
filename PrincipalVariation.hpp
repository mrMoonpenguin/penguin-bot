#ifndef CHESS_ENGINE_PRINCIPALVARIATION_HPP
#define CHESS_ENGINE_PRINCIPALVARIATION_HPP

#include "Move.hpp"
#include "Piece.hpp"

#include <iosfwd>
#include <vector>
#include <cstddef>

class PrincipalVariation {
public:

    using MoveIter = std::vector<Move>::const_iterator;

    PrincipalVariation(std::vector<Move> moves, PieceColor turn, long score, bool mate);

    [[nodiscard]] bool isMate() const;
    [[nodiscard]] long score() const;

    [[nodiscard]] std::size_t length() const;
    [[nodiscard]] MoveIter begin() const;
    [[nodiscard]] MoveIter end() const;

    [[nodiscard]] PieceColor turn() const;

    std::vector<Move> moves_;

private:
    PieceColor turn_;
    long score_;
    bool mate_;
};

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv);

#endif
