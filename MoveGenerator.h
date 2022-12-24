//
// Created by hasbid on 28/11/22.
//

#ifndef CPLCHESS_MOVEGENERATOR_H
#define CPLCHESS_MOVEGENERATOR_H


#include "Board.hpp"

class MoveGenerator {

public:

    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;
    using U64 = uint64_t;

    static U64 kingMoves(const Board &board, const Square &square, const U64 &empty, const U64 &enemy, bool allowCastling);
    static U64 queenMoves(const Square &square, const U64 &empty, const U64 &enemy);
    static U64 rookMoves(const Square &square, const U64 &empty, const U64 &enemy);
    static U64 bishopMoves(const Square &square, const U64 &empty, const U64 &enemy);
    static U64 knightMoves(const Square &square, const U64 &empty, const U64 &enemy);
    static U64 pawnMoves(const Square &square, const U64 &empty, const U64 &enemy, const PieceColor &turn, const std::optional<Square> &epsq);

    static U64 pawnAttacks(const Square &square, PieceColor turn);
};


#endif //CPLCHESS_MOVEGENERATOR_H
