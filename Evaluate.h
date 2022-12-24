//
// Created by hasbid on 27/11/22.
//

#ifndef CPLCHESS_EVALUATE_H
#define CPLCHESS_EVALUATE_H

#include "Board.hpp"

class Evaluate {

public:
    static int evaluate(const Board &board, int who2move);

    // seriously cba to implement a mirror function, so just doing the mirror manually
private:
    static const int pawnTableWhite[64];
    static const int pawnTableBlack[64];
    static const int knightTableWhite[64];
    static const int knightTableBlack[64];
    static const int bishopTableWhite[64];
    static const int bishopTableBlack[64];
    static const int rookTableWhite[64];
    static const int rookTableBlack[64];
    static const int queenTableWhite[64];
    static const int queenTableBlack[64];
    static const int kingTableWhite[64];
    static const int kingTableBlack[64];

    static const int *pieceSquareTables[12];

};

#endif //CPLCHESS_EVALUATE_H
