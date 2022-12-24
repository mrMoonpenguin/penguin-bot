#include "PrincipalVariation.hpp"

#include <vector>

PrincipalVariation::PrincipalVariation(std::vector<Move> moves, PieceColor turn, long score, bool mate) :
    moves_(std::move(moves)), turn_(turn), score_(score), mate_(mate) {}

bool PrincipalVariation::isMate() const {
    return mate_;
}

long PrincipalVariation::score() const {
    return score_;
}

std::size_t PrincipalVariation::length() const {
    return moves_.size();
}

PrincipalVariation::MoveIter PrincipalVariation::begin() const {
    return moves_.begin();
}

PrincipalVariation::MoveIter PrincipalVariation::end() const {
    return moves_.end();
}

PieceColor PrincipalVariation::turn() const {
    return turn_;
}

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv) {
    (void)pv;
    return os;
}
