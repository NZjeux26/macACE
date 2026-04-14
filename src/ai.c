#include "ai.h"
#include "game.h"
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include <ace/managers/sprite.h>

//#define AI_LOGGING

#define MAX_DEPTH 1;

const UWORD kingPosWeights[BOARD_SIZE] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  // dead zone row
    0, 10000, 10000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 10000, 10000,  0,
    0, 10000,   500,  500,  500,  500,  500,  500,  500,   500, 10000,    0,  0,
    0,  1000,   500,  200,  200,  200,  200,  200,  200,   500,  1000,    0,  0,
    0,  1000,   500,  200,   50,   50,   50,   50,   50,   500,  1000,    0,  0,
    0,  1000,   500,  200,   50,   10,   10,   10,   50,   200,  1000,    0,  0,
    0,  1000,   500,  200,   50,   10,    0,   10,   50,   200,  1000,    0,  0,
    0,  1000,   500,  200,   50,   10,   10,   10,   50,   200,  1000,    0,  0,
    0,  1000,   500,  200,   50,   50,   50,   50,   50,   500,  1000,    0,  0,
    0,  1000,   500,  200,  200,  200,  200,  200,  200,   500,  1000,    0,  0,
    0, 10000,   500,  500,  500,  500,  500,  500,  500,   500, 10000,    0,  0,
    0, 10000, 10000, 1000, 1000, 1000, 1000, 1000, 1000,  1000, 10000, 10000, 0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0   // dead zone row
};

/*
    returns a score based on how much control the defenders have of the corners

    **Again like ther attcker version, this is based on the immediate adjcent pieces, which are not
    really that effective to have. This also could be combined with Corner Danger to give a free float score
*/
UBYTE defenderCornerControl(GameState *s){
    UBYTE control = 0;

    UBYTE approaches[4][2] = {
        {15,27}, //approaches for 14 Top Left
        {23,37}, //Top Right
        {145,131}, //Bottom Left
        {153,141} //Bottom Right
    };

    for(UBYTE i = 0; i < 4; i++){
        UBYTE a = s->boardState[approaches[i][0]];
        UBYTE b = s->boardState[approaches[i][1]];
        if(a == TEAM_DEFENDER && b == TEAM_DEFENDER) control += 2;
        else if(a == TEAM_ATTACKER || b == TEAM_ATTACKER) control +=1;
    }
    return control;
}

/*
    How many side the king has attackers surrounding it
    
    ** Same as below, doesn't say what direction, or pieces further out
    than one square
*/
BYTE kingSidesThreatened(GameState *s){
    UBYTE kingPos = s->defenders[0].pos;
    BYTE threat = 0;
    
    UBYTE adj[4] = {
        kingPos +1,
        kingPos -1,
        kingPos +13,
        kingPos -13
    };

    for(UBYTE i = 0; i < 4; i++){
        UBYTE sq = s->boardState[adj[i]];
        if(sq == TEAM_ATTACKER || sq == 4) threat++; //if pieces around the king are attackers, the threat of capture increases
        if(sq == TEAM_DEFENDER) threat--; //But if it's defenders, the threat decreases
    }

    return threat;
}
/*
    returns how many defenders are adjenct to the king

    ** Doesn't return where or what direction they are, also
    having defenders right next to the king is not really an advantage

*/
UBYTE kingShieldCount(GameState *s){
    UBYTE kingPos = s->defenders[0].pos;
    UBYTE shield = 0;
    UBYTE adj[4] = {
        kingPos +1,
        kingPos -1,
        kingPos +13,
        kingPos -13
    };

    for(UBYTE i = 0; i < 4; i++){
        if(s->boardState[adj[i]] == TEAM_DEFENDER) shield++;
    }

    return shield;
}

/* 
    Returns the corner nearest the king
*/
UBYTE nearestCornerDist(GameState *s){
    UBYTE corners[4] = {14,24,144,154};
    UBYTE kingPos = s->defenders[0].pos;
    UBYTE nearest = 255;

    for(UBYTE i = 0; i < 4; i++){
        UBYTE d = manhattanToCorner(kingPos, corners[i]);
        if(d < nearest) nearest = d;
    }

    return nearest;
}

// Manhattan distance from king to nearest corner
UBYTE manhattanToCorner(UBYTE pos, UBYTE corner){
    BYTE rowDiff = (pos / 13) - (corner / 13);
    BYTE colDiff = (pos % 13) - (corner % 13);
    if(rowDiff < 0) rowDiff = -rowDiff;
    if(colDiff < 0) colDiff = -colDiff;
    return (UBYTE)(rowDiff + colDiff);
}

/* 
    Returns a danger score based on the approaches to corners being occupied 
    
    **This is a very simplified idea, in the actual game you wouldn't block the
    approaches in these locations since they can be taken. The better places would be
    in the area one square further out either as a single or in pairs.

    Keep for now, but this will definatly need improved.
*/
UBYTE cornerDanger(GameState *s){
    UBYTE danger = 0;

    UBYTE approaches[4][2] = {
        {15,27}, //approaches for 14 Top Left
        {23,37}, //Top Right
        {145,131}, //Bottom Left
        {153,141} //Bottom Right
    };

    for(UBYTE i = 0; i < 4; i++){
        UBYTE a = s->boardState[approaches[i][0]];
        UBYTE b = s->boardState[approaches[i][1]];
        if(a == TEAM_ATTACKER && b == TEAM_ATTACKER) danger += 2;
        else if(a == TEAM_ATTACKER || b == TEAM_ATTACKER) danger +=1;
    }
    return danger;
}

WORD evaluateBoard(GameState *s){
    WORD score = 0;
    //King is always the first position in the defender array
    UBYTE kingPos = s->defenders[0].pos;

    if(s->kingState == KING_CAPTURED) {
        #ifdef AI_LOGGING
        logWrite("EVAL: King captured, returning -30000\n");
        #endif
        return -30000;
    }
    if(s->kingState == KING_ESCAPED) {
        #ifdef AI_LOGGING
        logWrite("EVAL: King escaped, returning 30000\n");
        #endif
        return 30000;
    }

    //Lookup the king pos table for nearest corner and corner paths
    score += (kingPosWeights[kingPos] >> 2); //shift right to divide the score by 4 since the other factors are more important than just the position of the king, but this still gives a good incentive to move towards the corners.

    //Corner danger
    WORD danger = cornerDanger(s);
    UBYTE dist = nearestCornerDist(s);
    if(dist <= 3) score -= (danger * 80);
    else score -= (danger * 40);

    //King Safety
    score += (kingShieldCount(s) * 15);
    score -=(kingSidesThreatened(s) * 1000);

    //defender map control
    score += (defenderCornerControl(s) * 15);

    //Piece counting / scores
    UBYTE attackersAlive = 0;
    UBYTE defendersAlive = 0;
    
    for(UBYTE k = 0; k < MAX_ATTACKERS; k ++){
        if(!s->attackers[k].captured) attackersAlive++;
    }
    //start at 1 since the king is always the first position in the defenders array and we want to count the other defenders here.
    for(UBYTE j = 1; j < MAX_DEFENDERS; j ++){
        if(!s->defenders[j].captured) defendersAlive++;
    }

    score -= (attackersAlive * 12);
    score += (defendersAlive * 24);

    //Attacker logic

    #ifdef AI_LOGGING
    logWrite("--- EVAL ---\n");
    logWrite("  KingPos:       %d (idx %d)\n", kingPosWeights[kingPos], kingPos);
    logWrite("  CornerDanger:  %d (dist %d, raw danger %d)\n", (dist <= 3) ? danger * 80 : danger * 40, dist, danger);
    logWrite("  ShieldCount:   %d\n", kingShieldCount(s) * 15);
    logWrite("  SidesThreat:   %d\n", kingSidesThreatened(s) * 1000);
    logWrite("  CornerCtrl:    %d\n", defenderCornerControl(s) * 15);
    logWrite("  Pieces ATK:%d DEF:%d score: %d\n", attackersAlive, defendersAlive, score);
    logWrite("  TOTAL:         %d\n", score);
    logWrite("------------\n");
    #endif

    return score;
}

/*
    gets all the moves for each piece
*/

void getAllMoves(GameState *s, UBYTE team, AIMove *moveList, UBYTE *moveCount){
    *moveCount = 0;

    if(team == TEAM_DEFENDER){
        for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
            if(s->defenders[j].captured) continue;

            UBYTE piecePos = s->defenders[j].pos;
            getValidMoves(s, piecePos);

            for(UBYTE i = 0; i < BOARD_SIZE; i++){
                if(validMoves[i] == validGeneration){
                    moveList[*moveCount].fromIndex = piecePos;
                    moveList[*moveCount].toIndex = i;
                    (*moveCount)++;
                }
            }
        }
        #ifdef AI_LOGGING
        logWrite("getAllMoves: team %d generated %d moves\n", team, *moveCount);
        #endif
    }
    else{
        for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
            if(s->attackers[k].captured) continue;

            UBYTE piecePos = s->attackers[k].pos;
            getValidMoves(s, piecePos);

            for(UBYTE i = 0; i < BOARD_SIZE; i++){
                if(validMoves[i] == validGeneration){
                    moveList[*moveCount].fromIndex = piecePos;
                    moveList[*moveCount].toIndex = i;
                    (*moveCount)++;
                }
            }
        }
        #ifdef AI_LOGGING
        logWrite("getAllMoves: team %d generated %d moves\n", team, *moveCount);
        #endif
    }
}

/*
    here the AI gets to move pieces around in it's own local copy of the game state
*/
void applyMove(GameState *s, AIMove move){
    
    MoveResult result = {0}; //Not actually used anywhere since this is just internal to the AI.

    if(s->currentPlayer == TEAM_DEFENDER){
        for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
            if(s->defenders[j].pos == move.fromIndex && !s->defenders[j].captured){
                s->defenders[j].pos = move.toIndex;
                s->boardState[move.toIndex] = (s->defenders[j].type == KING) ? 3 : 1; //update the boardState array with the new position of the piece, 1 for defender
                s->boardState[move.fromIndex] = 0; //set the old position to 0 for empty *This will override the throne if the king moved off it.
                if (s->boardState[84] == 0) s->boardState[84] = 4; //if the king moves off the throne, set the throne to 4 which is a special square. 
                s->currentPlayer = TEAM_ATTACKER; //swap current player here
                break;
            }
        }
    } else {
        for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
            if(s->attackers[k].pos == move.fromIndex && !s->attackers[k].captured){
                s->attackers[k].pos = move.toIndex;
                s->boardState[move.toIndex] = 2; //update the boardState array with the new position of the piece, 2 for attacker
                s->boardState[move.fromIndex] = 0; //set the old position to 0 for empty
                s->currentPlayer = TEAM_DEFENDER;
                break;
            }
        }   
    }

    // #ifdef AI_LOGGING
    // logWrite("applyMove: from %d to %d player %d\n", 
    //     move.fromIndex, move.toIndex, s->currentPlayer);
    // #endif

    //After moving the piece, we need to check if any pieces were captured by this move, and update the board state accordingly.
    checkForCaptures(s, move.toIndex, &result); //I think passing Null is going to break something
    checkShieldWallCaptures(s, move.toIndex, &result);
    checkExitFort(s);
    checkSurrounded(s, move.toIndex);
}

void applyMoveWithUndo(GameState *s, AIMove move, UndoInfo *undo){
    //This function will apply the move like applyMove, but it will also store the necessary information in the undo struct to be able to undo the move later. This is useful for the minimax algorithm to be able to explore different move options without permanently changing the game state.

    //The implementation of this function would be similar to applyMove, but with additional code to store the previous state of any pieces that are moved or captured in the undo struct before making the changes to the game state. Then, the undoMove function can use that stored information to revert the game state back to what it was before the move was applied.
    undo->fromIndex = move.fromIndex;
    undo->toIndex = move.toIndex;
    undo->oldFrom = s->boardState[move.fromIndex];
    undo->oldTo = s->boardState[move.toIndex];
    undo->pieceTeam = (s->currentPlayer == TEAM_DEFENDER) ? TEAM_DEFENDER : TEAM_ATTACKER;
    undo->oldKingState = s->kingState;
    undo->oldCurrentPlayer = s->currentPlayer;
    undo->captureCount = 0;

    if(s->currentPlayer == TEAM_DEFENDER){
        undo->pieceTeam = TEAM_DEFENDER;
         for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
            if(s->defenders[j].pos == move.fromIndex && !s->defenders[j].captured){
                undo->pieceIndex = j;
                s->defenders[j].pos = move.toIndex;
                s->boardState[move.toIndex] = (s->defenders[j].type == KING) ? 3 : 1; //update the boardState array with the new position of the piece, 1 for defender
                s->boardState[move.fromIndex] = (move.fromIndex == 84) ? 4 : 0; //set the old position to 0 for empty *This will override the throne if the king moved off it.
                break;
            }
        }
        s->currentPlayer = TEAM_ATTACKER; //swap current player here
    } else {
        for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
            if(s->attackers[k].pos == move.fromIndex && !s->attackers[k].captured){
                undo->pieceIndex = k;
                s->attackers[k].pos = move.toIndex;
                s->boardState[move.toIndex] = 2; //update the boardState array with the new position of the piece, 2 for attacker
                s->boardState[move.fromIndex] = 0; //set the old position to 0 for empty
                break;
            }
        }   
        s->currentPlayer = TEAM_DEFENDER;
    }

    MoveResult result = {0}; //Not actually used anywhere since this is just internal to the AI.
    checkForCaptures(s, move.toIndex, &result);
    checkShieldWallCaptures(s, move.toIndex, &result);

    for(UBYTE i = 0; i < result.capturedCount[0] && undo->captureCount < MAX_CAPTURES_PM; i++){
        undo->capturedPieceIndexes[undo->captureCount] = result.capturedPieceIndexes[0][i];
        undo->capturedTeams[undo->captureCount] = TEAM_DEFENDER;
        undo->captureCount++;
    }
    for(UBYTE i = 0; i < result.capturedCount[1] && undo->captureCount < MAX_CAPTURES_PM; i++){
        undo->capturedPieceIndexes[undo->captureCount] = result.capturedPieceIndexes[1][i];
        undo->capturedTeams[undo->captureCount] = TEAM_ATTACKER;
        undo->captureCount++;
    }

    checkExitFort(s);
    checkSurrounded(s, move.toIndex);

}

void undoMove(GameState *s, UndoInfo *undo){

    //restore flags
    s->kingState = undo->oldKingState;
    s->currentPlayer = undo->oldCurrentPlayer;

    //Restore the moving piece pos directly using the stored index
    if(undo->pieceTeam == TEAM_DEFENDER){
        s->defenders[undo->pieceIndex].pos = undo->fromIndex;
    } else {
        s->attackers[undo->pieceIndex].pos = undo->fromIndex;
    }

    //Restore board squares,  oldFrom handles the throne case.
    //We havea. snapshot before everything moved.
    s->boardState[undo->fromIndex] = undo->oldFrom;
    s->boardState[undo->toIndex] = undo->oldTo;

    for(UBYTE i = 0; i< undo->captureCount; i++){
        UBYTE boardPos = undo->capturedPieceIndexes[i];

        if(undo->capturedTeams[i] == TEAM_DEFENDER){
            for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
                if(s->defenders[j].pos == boardPos && s->defenders[j].captured){
                    s->defenders[j].captured = 0;
                    s->boardState[boardPos] = (s->defenders[j].type == KING) ? 3 : 1;
                    break; 
                }
            }
        }
        else{
            for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
                if(s->attackers[k].pos == boardPos && s->attackers[k].captured){
                    s->attackers[k].captured = 0;
                    s->boardState[boardPos] = 2;
                    break;
                }
            }
        }
    }
}

WORD minimax(GameState *s, UBYTE depth, WORD alpha, WORD beta, UBYTE maximizingPlayer){
    if(depth == 0 || s->kingState != KING_ALIVE){
        return evaluateBoard(s);
    }

    //ordering in move list?
    AIMove moveList[BOARD_SIZE]; // I don't actually know why the move list is the size of the board, it's just a list of moves.
    UBYTE moveCount;

    if(maximizingPlayer){
        getAllMoves(s, TEAM_DEFENDER, moveList, &moveCount);
        WORD maxEval = -32768;
        for(UBYTE i = 0; i < moveCount; i++){
            UndoInfo undo;
            applyMoveWithUndo(s, moveList[i], &undo);
            WORD eval = minimax(s, depth - 1, alpha, beta, 0);
            undoMove(s, &undo);
            if(eval > maxEval) maxEval = eval;
            if(eval > alpha) alpha = eval;
            if(beta <= alpha) break; //beta cut-off
        }
        return maxEval;
    } else {
        getAllMoves(s, TEAM_ATTACKER, moveList, &moveCount);
        WORD minEval = 32767;
        for(UBYTE i = 0; i < moveCount; i++){
            UndoInfo undo;
            applyMoveWithUndo(s, moveList[i], &undo);
            WORD eval = minimax(s, depth - 1, alpha, beta, 1);
            undoMove(s, &undo);
            if(eval < minEval) minEval = eval;
            if(eval < beta) beta = eval;
            if(beta <= alpha) break; //alpha cut-off
        }
        return minEval;
    }
}

AIMove getBestMove(GameState *s){
    
    AIMove moveList[BOARD_SIZE]; //does it have to be this big? ordering?
    UBYTE moveCount;
    UBYTE maximisingPlayer = (s->currentPlayer == TEAM_DEFENDER);
    
    getAllMoves(s, s->currentPlayer, moveList, &moveCount);
    
    UBYTE seearchDepth = MAX_DEPTH; //this can be adjusted based on performance needs.
    AIMove bestMove = moveList[0];
    WORD bestEval = maximisingPlayer ? -32768 : 32767;
    WORD alpha = -32768;
    WORD beta = 32767;

    for(UBYTE i = 0; i < moveCount; i++){
        UndoInfo undo;
        applyMoveWithUndo(s, moveList[i], &undo);
        WORD eval = minimax(s, seearchDepth, alpha, beta, !maximisingPlayer); //depth of 2 for now, this can be adjusted based on performance needs
        undoMove(s, &undo);

        if(maximisingPlayer && eval > bestEval){
            bestEval = eval;
            bestMove = moveList[i];
            if(eval > alpha) alpha = eval;
        }
        else if(!maximisingPlayer && eval < bestEval){
            bestEval = eval;
            bestMove = moveList[i];
            if(eval < beta) beta = eval;
        }
     
    }
    //mnaybe seperate the valid moves of AI and player, so we don't have to clear them here, but for now this works.
    memset(validMoves, 0, sizeof(validMoves)); //clear valid moves after the search is done, since the getBestMove function is called during the CPU turn and we don't want any leftover valid move markings to interfere with the player's turn.
    validGeneration = 0; //reset valid generation as well since we use that to mark valid moves in the getValidMoves function, and we don't want any leftover generation markings to interfere with the player's turn.
    
    #ifdef AI_LOGGING
    logWrite("AI getBestMove: from %d to %d, score %d\n", 
        bestMove.fromIndex, bestMove.toIndex, bestEval);
    #endif

    return bestMove;
}