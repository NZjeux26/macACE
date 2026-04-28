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
AIMove findBookMove(AIMove history[], UBYTE currentDepth); 
WORD minimax(GameState *s, UBYTE depth, WORD alpha, WORD beta, UBYTE maximising);
WORD evaluateBoard(GameState *s);
void getAllMoves(GameState *s, UBYTE team, AIMove *moveList, UBYTE *moveCount);
void stratagy(GameState *s);
void applyMove(GameState *s, AIMove move);
void applyMoveWithUndo(GameState *s, AIMove move, UndoInfo *undo);
void undoMove(GameState *s, UndoInfo *undo);

/* *****Helper Functions****** */
UBYTE manhattanToCorner(UBYTE pos, UBYTE corner);
UBYTE nearestCornerDist(GameState *s);
void AIgameReset(void);

/* *****Tactical Functions***** */
WORD evaluatePerimeterControl(GameState *s);
WORD kingSafety(GameState *s);
WORD evaluateLanes(GameState *s);
WORD evaluateForces(GameState *s);
WORD evaluateCapturePotential(GameState *s);

extern AIMove moveHistory[255]; //Record the move history so we can track for repetitions

#endif