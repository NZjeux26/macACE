#include "ai.h"
#include "game.h"
#include "openingBook.h"
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include <ace/managers/sprite.h>

//#define AI_LOGGING

#define MAX_DEPTH 1
#define AI_INF 30000

//using this global buffer instead of local ones, at depth 2+ the amount of MoveList buffers is quite large and a overflow occurs somewhere
AIMove moveBuffer[MAX_DEPTH + 1][400];

const WORD kingPosWeights[BOARD_SIZE] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  // dead zone row
    0,  7500,   3000, 1250,  1250,  1250, 1250, 1250, 1250, 1250,  3000, 7500,  0,
    0,  3000,   750,  750,   750,   750,  750,  750,  750,   750,  750,  3000,  0,
    0,  1250,   750,  350,   350,   350,  350,  350,  350,   350,  750,  1250,  0,
    0,  1250,   750,  350,   225,   225,  225,  225,  225,   350,  750,  1250,  0,
    0,  1250,   750,  350,   225,   200,  200,  200,  225,   350,  750,  1250,  0,
    0,  1250,   750,  350,   225,   200, -120,  200,  225,   350,  750,  1250,  0,
    0,  1250,   750,  350,   225,   200,  200,  200,  225,   350,  750,  1250,  0,
    0,  1250,   750,  350,   225,   225,  225,  225,  225,   350,  750,  1250,  0,
    0,  1250,   750,  350,   350,   350,  350,  350,  350,   350,  750,  1250,  0,
    0,  3000,   750,  750,   750,   750,  750,  750,  750,   750,  750,  3000,  0,
    0,  7500,   3000, 1250,  1250,  1250, 1250, 1250, 1250,  1250, 3000, 7500,  0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0   // dead zone row
};

/* ------Helper Functions------ */

void AIgameReset(void){
    gameTurnCounter = 0;
    validGeneration = 0;
    memset(validMoves, 0, sizeof(validMoves));
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

/* ------Tactical Functions------ */

/* Provides a control score based on attacker or defender control of the perimeter. 
    Positive values favor the Defender, negative values favor the Attacker.
    Uses a synergy bonus to force the AI to complete pairs.
*/

WORD evaluatePerimeterControl(GameState *s){
    WORD control = 0;

    // Main perimeter that needs guarded early to stop an encirclement
    UBYTE prime[4][2] = {
        {125,113}, // Top Left
        {121,107}, // Top Right
        {43,55},   // Bottom Left
        {47,61}    // Bottom Right
    };

    // Secondary squares to guard perimeter
    UBYTE secondary[4][2] = {
        {124,100}, // Top Left
        {122,94},  // Top Right
        {44,31},   // Bottom Left
        {46,74}    // Bottom Right
    };

    // Evaluate prime perimeter
    for(UBYTE i = 0; i < 4; i++){
        UBYTE a = s->boardState[prime[i][0]];
        UBYTE b = s->boardState[prime[i][1]];

        //defender scoring
        if(a == TEAM_DEFENDER && b == TEAM_DEFENDER) control += 52;
        else if(a == TEAM_DEFENDER || b == TEAM_DEFENDER) control += 23;

        //attacker scoring
        if(a == TEAM_ATTACKER && b == TEAM_ATTACKER) control -= 52;
        else if(a == TEAM_ATTACKER || b == TEAM_ATTACKER) control -= 23;
    }

    // Evaluate secondary perimeter
    for(UBYTE i = 0; i < 4; i++){
        UBYTE a = s->boardState[secondary[i][0]];
        UBYTE b = s->boardState[secondary[i][1]];

        //defender scoring
        if(a == TEAM_DEFENDER && b == TEAM_DEFENDER) control += 42;
        else if(a == TEAM_DEFENDER || b == TEAM_DEFENDER) control += 17;

        //attacker scoring
        if(a == TEAM_ATTACKER && b == TEAM_ATTACKER) control -= 42;
        else if(a == TEAM_ATTACKER || b == TEAM_ATTACKER) control -= 17;
    }

    return control;
}

/*
    This calculates the saftey of the king, how many attackers are around it, vs how many blank or friendly sqaures
*/
WORD kingSafety(GameState *s){
    UBYTE kingPos = s->defenders[0].pos;
    
    WORD safety = 0;
    UBYTE emptyCount = 0;

    UBYTE adj[4] = {
        kingPos +1,
        kingPos -1,
        kingPos +13,
        kingPos -13
    };

    for(UBYTE i = 0; i < 4; i++){
        UBYTE sq = s->boardState[adj[i]];
        if(sq == 0){
            emptyCount++;
            safety += 50; //empty squares increase safety
        }
        else if(sq == TEAM_ATTACKER) safety -= 304; //if pieces around the king are attackers, the threat of capture increases
        else if(sq == TEAM_DEFENDER || sq == 4) safety += 28; //But if it's defenders or empty squares, the threat decreases
        else if(sq == 99) safety += 101; //if the king is against the board edge, it cannot be captured and can relax
    }
    //check the second squares out from the king, the more emp
    for(UBYTE i = 0; i < 4; i++){
        UBYTE sq = s->boardState[adj[i] + adj[0]];
        if(sq == 0){
            emptyCount++;
            safety += 26; //empty squares increase safety
        }
        else if(sq == TEAM_ATTACKER) safety -= 122; //if pieces around the king are attackers, the threat of capture increases
        else if(sq == TEAM_DEFENDER || sq == 4) safety += 12; //But if it's defenders or empty squares, the threat decreases
    }
    return safety;
}

/* Evaluates the overall strength of each team's forces on the board */

WORD evaluateForces(GameState *s){
    WORD forceScore = 0;

    // Attacker scoring
    for(UBYTE i = 0; i < MAX_ATTACKERS; i++){
        if(!s->attackers[i].captured){
            forceScore -= 22; // each attacker on the board reduces the score for the defender
        }
    }

    // Defender scoring (starts at 1 since the King while a defender isn't a combatant)
    for(UBYTE j = 1; j < MAX_DEFENDERS; j++){
        if(!s->defenders[j].captured){
            forceScore += 44; // each defender on the board increases the score for the defender
        }
    }

    return forceScore;
}
/* Evaluates the potential for capturing pieces (non shield wall)*/
WORD evaluateCapturePotential(GameState *s){
    WORD captureScore = 0;
    //We check if there is a empty square next to a piece that a piece is next to, meaning is vulnerable to capture on the next turn if the opponent moves into that square. 
    //This is a very basic way to evaluate capture potential but it should give the AI some sense of which pieces are in danger and which pieces have good opportunities to capture on the next turn.
    //Remembering in this game capturing pieces is NOT the objective, but a self defensive measure.
    for (UBYTE i = 0; i < MAX_ATTACKERS; i++){
        if(!s->attackers[i].captured){
            UBYTE pos = s->attackers[i].pos;
            //check if there's a defender on the other side of the attacker, if so, it's a potential capture
            if((s->boardState[pos+1] == TEAM_DEFENDER || s->boardState[pos+1] == 4 || s->boardState[pos+1] == 3) && s->boardState[pos-1] == 0) captureScore += 26;
            if((s->boardState[pos-1] == TEAM_DEFENDER || s->boardState[pos-1] == 4 || s->boardState[pos-1] == 3) && s->boardState[pos+1] == 0) captureScore += 26;
            if((s->boardState[pos+13] == TEAM_DEFENDER || s->boardState[pos+13] == 4 || s->boardState[pos+13] == 3) && s->boardState[pos-13] == 0) captureScore += 26;
            if((s->boardState[pos-13] == TEAM_DEFENDER || s->boardState[pos-13] == 4 || s->boardState[pos-13] == 3) && s->boardState[pos+13] == 0) captureScore += 26;
        }
    }
    for (UBYTE j = 1; j < MAX_DEFENDERS; j++){
        if(!s->defenders[j].captured){
            UBYTE pos = s->defenders[j].pos;
            //check if there's an attacker on the other side of the defender, if so, it's a potential capture
            if((s->boardState[pos+1] == TEAM_ATTACKER || s->boardState[pos+1] == 4) && s->boardState[pos-1] == 0) captureScore -= 26;
            if((s->boardState[pos-1] == TEAM_ATTACKER || s->boardState[pos-1] == 4) && s->boardState[pos+1] == 0) captureScore -= 26;
            if((s->boardState[pos+13] == TEAM_ATTACKER || s->boardState[pos+13] == 4) && s->boardState[pos-13] == 0) captureScore -= 26;
            if((s->boardState[pos-13] == TEAM_ATTACKER || s->boardState[pos-13] == 4) && s->boardState[pos+13] == 0) captureScore -= 26;
        }
    }
    return captureScore;
}

WORD evaluateLanes(GameState *s){
    WORD laneScore = 0;
    UBYTE kingPos = s->defenders[0].pos;
    UBYTE adj[4] = {
        kingPos +1,
        kingPos -1,
        kingPos +13,
        kingPos -13
    };

    for(UBYTE i = 0; i < 4; i++){
        UBYTE dir = adj[i];
        UBYTE p = kingPos + dir;
        
        UBYTE enemyBlockers = 0;
        UBYTE friendBlockers = 0;

        for(UBYTE j = 0; j < 10; j++){
          UBYTE piece = s->boardState[p];
          if(piece == 99) break; //board edge, lane is blocked but not by an enemy piece
          if(piece == TEAM_ATTACKER) enemyBlockers++;
          else if(piece == TEAM_DEFENDER) friendBlockers++;

          p += dir;
        }
        //if there are no enemy blockers in the lane, it's a potential escape route for the king, increasing the score for the defender
        if(enemyBlockers == 0) laneScore += 504;
        //if there are enemy blockers but also friend blockers, it's a contested lane, slightly increasing the score for the defender since they have some presence there
        else if(friendBlockers > 0 && friendBlockers < 3) laneScore += 286; //This should be less than two.
        //if there are enemy blockers and no friend blockers, it's a dangerous lane for the king, decreasing the score for the defender
        else laneScore -= 31; //IDK about this, I want to encourage the attack to block, but the AI not to avoid but to attack those pieces.
    }   

        

    return laneScore;
}

WORD evaluateBoard(GameState *s){
    if(s->kingState == KING_CAPTURED) return -30000;
    if(s->kingState == KING_ESCAPED) return 30000;

    WORD score = 0;
    //King is always the first position in the defender array
    UBYTE kingPos = s->defenders[0].pos;

    // Get the raw values from the tactical functions, these will be combined and weighted to give the final score

    /* Board Control*/
    WORD perimeter = evaluatePerimeterControl(s);
    WORD lanes = evaluateLanes(s);

    //King Safety / Mobility
    WORD corners = kingPosWeights[kingPos]; //this needs to have a flip enabled so the attackers get a penalty for allowing the king to manouver
    WORD kingSafetyScore = kingSafety(s);

    //Force Evaluation
    WORD forceScore = evaluateForces(s);

    //Capture Potential
    WORD captureScore = evaluateCapturePotential(s);

    // different phases of the game require different weightings to different parts of the board
    
    //Early game, focuses on establishing control of the perimeter and getting the king mobile (opposite for the attackers)
    if(gameTurnCounter <= 6){
        score += perimeter;
        score += (corners >> 1); //reduce the impact of the corners in the early game.
        score += (kingSafetyScore >> 1);
        score += (forceScore >> 2); //reduce the impact of force in the early game
        score += (captureScore >> 1); //reduce the impact of capture potential in the early game
        
    }
    //Mid game, we start to focus on creating lanes for the king to escape, but maintain our perimeter control so we don't get encircled.
    else if(gameTurnCounter >6 && gameTurnCounter <= 16){
        score += perimeter; //reduce the impact of perimeter control in the mid game
        //lanes
        score += (lanes >> 1); //reduce the impact of the corners in the mid game, but not as much as early game since they start to become more important as the king starts to move towards them
        score += corners; //increase the impact of the corners in the mid game
        //King 
        score += kingSafetyScore;
        //Force
        score += forceScore; 
        score += captureScore; 
        
    }
    //Late game, we focus on getting the king out, increase the values of lanes, corners and taking enemy pieces that block our path.
    else{
        score += (perimeter >> 1); //reduce the impact of perimeter control in the late game
        //lanes
        score += lanes; //lanes become more important in the late game as the king starts to try to escape and needs open lanes to do so
        score += corners; //full impact of the corners in the late game
        score += kingSafetyScore << 1; //increase the impact of king safety in the late game
        score += forceScore; 
        score += captureScore; 
    }

    return score;
}

/* -----AI Move Generation Functions----- */

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
    undo->pieceIndex = 0xFF; //sets a default value that is above any checks below, so if it's not set properly it will failover easier and be more obvious in testing.

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
        undo->pieceTeam = TEAM_ATTACKER;
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
        undo->capturedTeams[undo->captureCount] = result.capturedPieceTeam[i];
        undo->capturedArrayIndexes[undo->captureCount] = result.capturedArrayIndexes[i];
        undo->captureCount++;
    }

    //these if statements might not be working as intended.
    if(s->currentPlayer == TEAM_DEFENDER) checkExitFort(s);
    if(s->currentPlayer == TEAM_ATTACKER) checkSurrounded(s, move.toIndex);

}

void undoMove(GameState *s, UndoInfo *undo){

    //restore flags
    s->kingState = undo->oldKingState;
    s->currentPlayer = undo->oldCurrentPlayer;

    //Restore the moving piece pos directly using the stored index
    if(undo->pieceTeam == TEAM_DEFENDER){
        if(undo->pieceIndex < MAX_DEFENDERS) //sanity check, we set the pieceIndex to 265 at the start and if it doesn't get used properly this will fail.
            s->defenders[undo->pieceIndex].pos = undo->fromIndex;
        
    } else {
        if(undo->pieceIndex < MAX_ATTACKERS)
            s->attackers[undo->pieceIndex].pos = undo->fromIndex;
    }

    /* Restores the board state, finds the captured pieces from the undo information and restores them */
    s->boardState[undo->fromIndex] = undo->oldFrom; 
    s->boardState[undo->toIndex] = undo->oldTo;

    for(UBYTE i = 0; i< undo->captureCount; i++){
        UBYTE boardPos = undo->capturedPieceIndexes[i];
        UBYTE teamArrayIdx = undo->capturedArrayIndexes[i];

        if(undo->capturedTeams[i] == TEAM_DEFENDER){
            s->defenders[teamArrayIdx].captured = 0;
            s->defenders[teamArrayIdx].pos = boardPos;
            s->boardState[boardPos] = (s->defenders[teamArrayIdx].type == KING) ? 3 : 1;
        }
        else{
            s->attackers[teamArrayIdx].captured = 0;
            s->attackers[teamArrayIdx].pos = boardPos;
            s->boardState[boardPos] = 2;
        }
    }
}

AIMove findBookMove(AIMove history[], UBYTE currentDepth){
    if(currentDepth > MAX_BOOK_DEPTH) return (AIMove){0}; //if we somehow get a depth that is larger than our history, just return no book move, this is a sanity check to prevent out of bounds access to the history array.
    
    for(WORD i = 0; i < OPENING_BOOK_SIZE; i++){
        const BookEntry *entry = &openingBook[i];

        if(entry->numMoves > currentDepth) break; //if the number of moves in the book entry doesn't match our current depth, skip this entry

        if(entry->numMoves != currentDepth) continue; //if the number of moves in the book entry doesn't match our current depth, skip this entry

        BYTE match = 1;
        for(UBYTE j = 0; j < currentDepth; j++){
            if(entry->sequence[j].fromIndex != history[j].fromIndex || 
               entry->sequence[j].toIndex != history[j].toIndex){
                match = 0;
                break;
            }
        }
        if(match){
            ((BookEntry*)entry)->best_next_move.score = AI_INF; 
            return entry->best_next_move; //if we find a match, return the book move for that entry
        }
    }
    
    return (AIMove){0}; //if we don't find a match, return a blank move.
}

/* ------AI Move Evaluation Functions------*/

WORD minimax(GameState *s, UBYTE depth, WORD alpha, WORD beta, UBYTE maximizingPlayer){
    if(depth == 0 || s->kingState != KING_ALIVE){
        return evaluateBoard(s);
    }

    AIMove *moveList = moveBuffer[depth];
    UBYTE moveCount;

    if(maximizingPlayer){
        getAllMoves(s, TEAM_DEFENDER, moveList, &moveCount);
        WORD maxEval = -AI_INF;

        for(UBYTE i = 0; i < moveCount; i++){
            UndoInfo undo;
            applyMoveWithUndo(s, moveList[i], &undo);
            WORD eval = minimax(s, depth - 1, alpha, beta, 0);
            undoMove(s, &undo);
            
            if(eval > maxEval) maxEval = eval;
            if(maxEval > alpha) alpha = maxEval;

            if(beta <= alpha) break; //beta cut-off
        }
        return maxEval;
    } else {
        getAllMoves(s, TEAM_ATTACKER, moveList, &moveCount);
        WORD minEval = AI_INF
        ;
        for(UBYTE i = 0; i < moveCount; i++){
            UndoInfo undo;
            applyMoveWithUndo(s, moveList[i], &undo);
            WORD eval = minimax(s, depth - 1, alpha, beta, 1);
            undoMove(s, &undo);
            
            if(eval < minEval) minEval = eval;
            if(minEval < beta) beta = minEval;

            if(beta <= alpha) break; //alpha cut-off
        }
        return minEval;
    }
}

AIMove getBestMove(GameState *s){
    
    AIMove *moveList = moveBuffer[0];
    UBYTE moveCount;
    UBYTE maximisingPlayer = (s->currentPlayer == TEAM_DEFENDER);
    
    UBYTE seearchDepth = MAX_DEPTH; //this can be adjusted based on performance needs.

    WORD bestEval = maximisingPlayer ? -AI_INF : AI_INF;
    WORD alpha = -AI_INF;
    WORD beta = AI_INF;

    getAllMoves(s, s->currentPlayer, moveList, &moveCount);
    
    if(moveCount == 0){
        // No valid moves, return a dummy move (could also consider returning an error code or similar)
        AIMove noMove = {0};
        return noMove;
    }

    AIMove bestMove = moveList[0];

    for(UBYTE i = 0; i < moveCount; i++){
        UndoInfo undo;
        applyMoveWithUndo(s, moveList[i], &undo);//apply the move
        WORD eval = minimax(s, seearchDepth, alpha, beta, !maximisingPlayer); //test it in minmax
        undoMove(s, &undo);//undo
        
        moveList[i].score = eval;

        if(maximisingPlayer){
            if(eval > bestEval){
                bestEval = eval;
                bestMove = moveList[i];
                if(eval > alpha) alpha = eval;
            }
            //tie breaker jitter (needs random number generator)
        } else {
            if(eval < bestEval){
                bestEval = eval;
                bestMove = moveList[i];
                if(eval < beta) beta = eval;
            }
            //attacker tie breaker jitter (needs random number generator)
        }
    }
    //count moves in moveList witht he same score as bestEval.
    UBYTE sameMoveCount = 0;
    for(UBYTE i = 0; i < moveCount; i++){
        if(moveList[i].score == bestEval) sameMoveCount++;
    }
    //if there are more than 3 moves with the same evaluation, we will randomly select one of them to add some variability to the AI's playstyle and make it less predictable.
    //this is only looking at positive values, the attacker needs neg values
    //Also this might not be ordering the list correctly, some moves scores it picks don't always line up with the previous playthroughs
    // if(sameMoveCount > 3){ 
    //     UBYTE r = randUwMax(s_pRandManager, sameMoveCount); //get a random number between 0 and sameMoveCount-1
    //     UBYTE idx = 0;
    //     for(UBYTE i = 0; i < moveCount; i++){
    //         if(moveList[i].score == bestEval){
    //             if(idx == r){
    //                 bestMove = moveList[i];
    //                 break;
    //             }
    //             idx++;
    //         }
    //     }
    // }
     
    for(UBYTE a = 0; a < moveCount - 1; a++){
        for(UBYTE b = a + 1; b < moveCount; b++){
            if(moveList[b].score > moveList[a].score){
                AIMove tmp = moveList[a];
                moveList[a] = moveList[b];
                moveList[b] = tmp;
            }
        }
    }

    //mnaybe seperate the valid moves of AI and player, so we don't have to clear them here, but for now this works.
    memset(validMoves, 0, sizeof(validMoves)); //clear valid moves after the search is done, since the getBestMove function is called during the CPU turn and we don't want any leftover valid move markings to interfere with the player's turn.
    validGeneration = 0; //reset valid generation as well since we use that to mark valid moves in the GetValidMoves function, and we don't want any leftover generation markings to interfere with the player's turn.
    
    logWrite("--- Top 10 moves (of %d total, %d tied best) ---\n", moveCount, sameMoveCount);
    logWrite("AI getBestMove: from %d to %d, score %d\n", 
        bestMove.fromIndex, bestMove.toIndex, bestEval);
    
    logWrite("moveCount: %d\n", moveCount);
    logWrite("Game Turn: %d\n", gameTurnCounter);
    
    for(UBYTE i = 0; i < 10; i++){
        logWrite("Move from %d to %d has a score of %d",moveList[i].fromIndex, moveList[i].toIndex,moveList[i].score);
    }
    return bestMove;
}