#include "Move.hpp"


Move::Move(const Square &from, const Square &to,
           const std::optional<PieceType> &promotion) : from_(from), to_(to), promotion_(promotion) {}

Move::Optional Move::fromUci(const std::string &uci) {
    if (uci.length() < 4 || uci.length() > 5) {
        return std::nullopt;
    }

    auto from = Square::fromName(uci.substr(0, 2));
    auto to = Square::fromName(uci.substr(2, 2));
    if (!from || !to) {
        return std::nullopt;
    }

    std::optional<PieceType> promotion = std::nullopt;
    if (uci.length() == 5) {
        auto optionalPiece = Piece::fromSymbol(uci[4]);
        if (optionalPiece) {
            promotion = optionalPiece->type();
        } else {
            return std::nullopt;
        }
    }

    return Move(from.value(), to.value(), promotion);
}

Square Move::from() const {
    return from_;
}

Square Move::to() const {
    return to_;
}

std::optional<PieceType> Move::promotion() const {
    return promotion_;
}

void Move::setPromotion(PieceType promotion) {
    promotion_ = promotion;
}

// default constructor needed for some containers
Move::Move() : from_(Square::A1), to_(Square::A1), promotion_(std::nullopt) {}

std::ostream &operator<<(std::ostream &os, const Move &move) {
    os << move.from() << move.to();
    if (move.promotion()) {
        os << move.promotion().value();
    }
    return os;
}


bool operator<(const Move &lhs, const Move &rhs) {
    return  lhs.from() < rhs.from() ||
           (lhs.from() == rhs.from() && lhs.to() < rhs.to()) ||
           (lhs.from() == rhs.from() && lhs.to() == rhs.to() && lhs.promotion() < rhs.promotion());
}

bool operator==(const Move &lhs, const Move &rhs) {
    return lhs.from() == rhs.from() && lhs.to() == rhs.to() && lhs.promotion() == rhs.promotion();
}
