#ifndef _AI_H_
#define _AI_H_

#include "game.h"

typedef struct{
    UBYTE fromIndex;
    UBYTE toIndex;
    WORD score; 
} AIMove;

typedef struct{
   UBYTE fromIndex;
   UBYTE toIndex;
   UBYTE oldFrom;
   UBYTE oldTo;
   UBYTE pieceIndex;
   UBYTE pieceTeam;
   UBYTE oldKingState;
   UBYTE oldCurrentPlayer;
   UBYTE captureCount;
   UBYTE capturedPieceIndexes[MAX_CAPTURES_PM];
   UBYTE capturedArrayIndexes[MAX_CAPTURES_PM];
   UBYTE capturedTeams[MAX_CAPTURES_PM];
} UndoInfo;

/* *****AI Functions***** */
AIMove getBestMove(GameState *s);
void getAllMoves(GameState *s, UBYTE team, AIMove *moveList, UBYTE *moveCount);
WORD evaluateBoard(GameState *s);
void stratagy(GameState *s);
WORD minimax(GameState *s, UBYTE depth, WORD alpha, WORD beta, UBYTE maximising);
void applyMove(GameState *s, AIMove move);
void applyMoveWithUndo(GameState *s, AIMove move, UndoInfo *undo);
void undoMove(GameState *s, UndoInfo *undo);

/* *****Helper Functions****** */
UBYTE manhattanToCorner(UBYTE pos, UBYTE corner);
UBYTE nearestCornerDist(GameState *s);
void aiGetValidMoves(GameState *state, UBYTE selectedIndex);
void AIgameReset(void);

/* *****Tactical Functions***** */
WORD evaluatePerimeterControl(GameState *s);
UBYTE defenderCornerControl(GameState *s);
WORD kingSafety(GameState *s);
UBYTE cornerDanger(GameState *s);
UBYTE kingMobility(GameState *s);
WORD evaluateForces(GameState *s);
WORD evaluateCapturePotential(GameState *s);

#endif