#ifndef _AI_H_
#define _AI_H_

#include "game.h"

typedef struct{
    UBYTE fromIndex;
    UBYTE toIndex;
} AIMove;

/* *****AI Functions***** */
AIMove getBestMove(GameState *s);
void getAllMoves(GameState *s, UBYTE team, AIMove *moveList, UBYTE *moveCount);
WORD evaluateBoard(GameState *s);
void stratagy(GameState *s);
WORD minimax(GameState *s, UBYTE depth, WORD alpha, WORD beta, UBYTE maximising);
void applyMove(GameState *s, AIMove move);

/* *****Helper Functions****** */
UBYTE manhattanToCorner(UBYTE pos, UBYTE corner);
UBYTE cornerDanger(GameState *s);
UBYTE nearestCornerDist(GameState *s);
UBYTE defenderCornerControl(GameState *s);
BYTE kingSidesThreatened(GameState *s);
UBYTE kingShieldCount(GameState *s);
UBYTE kingMobility(GameState *s);

#endif