#include "ai.h"
#include "game.h"
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include <ace/managers/sprite.h>

//#define AI_LOGGING

#define MAX_DEPTH 1
#define AI_INF 30000

static UBYTE gameTurnCounter = 0; //used to track the number of turns that have passed in the game, this is used to trigger certain strategies at certain points in the game, and also to track when to reset the AI's internal state if needed.
//UWORD aiValidMoves[BOARD_SIZE]; //Chnaging this to UWORD fixed the problem of max scores and the CPU just getting the same move as top
//UWORD validGeneration = 0;
//using this global buffer instead of local ones, at depth 2+ the amount of MoveList buffers is quite large and a overflow occurs somewhere
AIMove moveBuffer[MAX_DEPTH + 1][400];

const WORD kingPosWeights[BOARD_SIZE] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  // dead zone row
    0,  5000,   3000, 1000,  1000,  1000, 1000, 1000, 1000, 1000,  3000, 5000,  0,
    0,  3000,   500,  500,   500,   500,  500,  500,  500,   500,  500,  3000,  0,
    0,  1000,   500,  250,   250,   250,  250,  250,  250,   250,  500,  1000,  0,
    0,  1000,   500,  250,   175,   175,  175,  175,  175,   250,  500,  1000,  0,
    0,  1000,   500,  250,   175,   150,  150,  150,  175,   250,  500,  1000,  0,
    0,  1000,   500,  250,   175,   150, -100,  150,  175,   250,  500,  1000,  0,
    0,  1000,   500,  250,   175,   150,  150,  150,  175,   250,  500,  1000,  0,
    0,  1000,   500,  250,   175,   175,  175,  175,  175,   250,  500,  1000,  0,
    0,  1000,   500,  250,   250,   250,  250,  250,  250,   250,  500,  1000,  0,
    0,  3000,   500,  500,   500,   500,  500,  500,  500,   500,  500,  3000,  0,
    0,  5000,   3000, 1000,  1000,  1000, 1000, 1000, 1000,  1000, 3000, 5000,  0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0   // dead zone row
};

/* ------Helper Functions------ */

void AIgameReset(void){
    gameTurnCounter = 0;
    validGeneration = 0;
    memset(validMoves, 0, sizeof(validMoves));
}
// // /* This function will calculate the valid moves for the currently highlighted piece and populate the validMoves array, which is indexed the same as the board array, with a value over 0 for valid moves and 0 for invalid moves.*/
// void GetValidMoves(GameState *state, UBYTE selectedIndex){
  
//   //First check if the square is empty, a special square or out of bounds, there are no valid moves to calculate, so return
//   if(state->boardState[selectedIndex] == 0 || state->boardState[selectedIndex] == 4 || state->boardState[selectedIndex] == 99){ 
//     return;
//   }
//   //increment valid generation to mark the current valid moves, this way we can avoid having to clear the validMoves array every time.
//   aiValidGeneration++;

//   if(aiValidGeneration == 0){
//     memset(aiValidMoves, 0, sizeof(aiValidMoves));
//     aiValidGeneration = 1;
//   }
  
//   //need to check whether the piece is the king, if so it's allowed on squares in boardState that = 4.
//   UBYTE isKing = (state->boardState[selectedIndex] == 3);

//   /* This is no risk of over or under flow. Only 13-154 are valid game squares and only 14-153 are valid for piece movement */

//   /*lets check the rows which are +1 and -1 This needs the minus (or plus) so the index doesn't start on the piece selected and auto fails.*/
//   for(UBYTE r = (selectedIndex +1); r < 169; r++){ //rows in the + direction
//     //if the square is occupied, break
//     if(state->boardState[r] > 0 && state->boardState[r] <=3){
//       break; //if greater than 0 it means th square is occupied, a special square or out of bounds and invalid
//     }
//     else if(state->boardState[r] == 4 && !isKing){ //if it's a special square and the piece isn't the king, break
//       break;
//     }
//     else if(state->boardState[r] == 99){ //if it's out of bounds, break
//       break;
//     }
//     aiValidMoves[r] = aiValidGeneration; //add the current position to the valid moves array. 
//   }
  
//   for(UBYTE u = (selectedIndex - 1); u < 169; u--){ //rows in the - direction
//     if(state->boardState[u] > 0 && state->boardState[u] <=3){
//       break; 
//     }
//     else if(state->boardState[u] == 4 && !isKing){
//       break;
//     }
//     else if(state->boardState[u] == 99){
//       break;
//     }
//     aiValidMoves[u] = aiValidGeneration; 
//   }

//   /* **Check Coloums** */

//   for(UBYTE c = (selectedIndex +13); c < 169; c=c+13){ 
//     if(state->boardState[c] > 0 && state->boardState[c] <=3){
//       break;
//     }
//     else if(state->boardState[c] == 4 && !isKing){
//       break;
//     }
//     else if(state->boardState[c] == 99){
//       break;
//     }
//     aiValidMoves[c] = aiValidGeneration; 
//   }
  
//   for(UBYTE y = (selectedIndex -13); y < 169; y=y-13){ 
//     if(state->boardState[y] > 0 && state->boardState[y] <=3){
//       break; 
//     }
//     else if(state->boardState[y] == 4 && !isKing){
//       break;
//     }
//     else if(state->boardState[y] == 99){
//       break;
//     }
//     aiValidMoves[y] = aiValidGeneration; 
//   }
  
//   /* Warning, this will not print the full array if you select a square and then quit, you must select another square (empty or not)*/
//   #ifdef OUTPUT_LOGGING
//   logWrite("aiValidGeneration = %d\n", aiValidGeneration);
//   for(UBYTE i = 0; i < 169; i++){
//     if(aiValidMoves[i] == aiValidGeneration){
//       logWrite("Valid Moves at Square %d\n",i);
//     }
//   }
//   #endif
// }

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
        if(a == TEAM_DEFENDER && b == TEAM_DEFENDER) control += 50;
        else if(a == TEAM_DEFENDER || b == TEAM_DEFENDER) control += 25;

        //attacker scoring
        if(a == TEAM_ATTACKER && b == TEAM_ATTACKER) control -= 50;
        else if(a == TEAM_ATTACKER || b == TEAM_ATTACKER) control -= 25;
    }

    // Evaluate secondary perimeter
    for(UBYTE i = 0; i < 4; i++){
        UBYTE a = s->boardState[secondary[i][0]];
        UBYTE b = s->boardState[secondary[i][1]];

        //defender scoring
        if(a == TEAM_DEFENDER && b == TEAM_DEFENDER) control += 45;
        else if(a == TEAM_DEFENDER || b == TEAM_DEFENDER) control += 15;

        //attacker scoring
        if(a == TEAM_ATTACKER && b == TEAM_ATTACKER) control -= 45;
        else if(a == TEAM_ATTACKER || b == TEAM_ATTACKER) control -= 15;
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
        else if(sq == TEAM_ATTACKER) safety -= 300; //if pieces around the king are attackers, the threat of capture increases
        else if(sq == TEAM_DEFENDER || sq == 4) safety += 25; //But if it's defenders or empty squares, the threat decreases
    }
    //check the second squares out from the king, the more emp
    for(UBYTE i = 0; i < 4; i++){
        UBYTE sq = s->boardState[adj[i] + adj[0]];
        if(sq == 0){
            emptyCount++;
            safety += 25; //empty squares increase safety
        }
        else if(sq == TEAM_ATTACKER) safety -= 150; //if pieces around the king are attackers, the threat of capture increases
        else if(sq == TEAM_DEFENDER || sq == 4) safety += 10; //But if it's defenders or empty squares, the threat decreases
    }
    return safety;
}

/* Evaluates the overall strength of each team's forces on the board */

WORD evaluateForces(GameState *s){
    WORD forceScore = 0;

    // Attacker scoring
    for(UBYTE i = 0; i < MAX_ATTACKERS; i++){
        if(!s->attackers[i].captured){
            forceScore -= 20; // each attacker on the board reduces the score for the defender
        }
    }

    // Defender scoring (starts at 1 since the King while a defender isn't a combatant)
    for(UBYTE j = 1; j < MAX_DEFENDERS; j++){
        if(!s->defenders[j].captured){
            forceScore += 40; // each defender on the board increases the score for the defender
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
            if((s->boardState[pos+1] == TEAM_DEFENDER || s->boardState[pos+1] == 4 || s->boardState[pos+1] == 3) && s->boardState[pos-1] == 0) captureScore += 25;
            if((s->boardState[pos-1] == TEAM_DEFENDER || s->boardState[pos-1] == 4 || s->boardState[pos-1] == 3) && s->boardState[pos+1] == 0) captureScore += 25;
            if((s->boardState[pos+13] == TEAM_DEFENDER || s->boardState[pos+13] == 4 || s->boardState[pos+13] == 3) && s->boardState[pos-13] == 0) captureScore += 25;
            if((s->boardState[pos-13] == TEAM_DEFENDER || s->boardState[pos-13] == 4 || s->boardState[pos-13] == 3) && s->boardState[pos+13] == 0) captureScore += 25;
        }
    }
    for (UBYTE j = 1; j < MAX_DEFENDERS; j++){
        if(!s->defenders[j].captured){
            UBYTE pos = s->defenders[j].pos;
            //check if there's an attacker on the other side of the defender, if so, it's a potential capture
            if((s->boardState[pos+1] == TEAM_ATTACKER || s->boardState[pos+1] == 4) && s->boardState[pos-1] == 0) captureScore -= 25;
            if((s->boardState[pos-1] == TEAM_ATTACKER || s->boardState[pos-1] == 4) && s->boardState[pos+1] == 0) captureScore -= 25;
            if((s->boardState[pos+13] == TEAM_ATTACKER || s->boardState[pos+13] == 4) && s->boardState[pos-13] == 0) captureScore -= 25;
            if((s->boardState[pos-13] == TEAM_ATTACKER || s->boardState[pos-13] == 4) && s->boardState[pos+13] == 0) captureScore -= 25;
        }
    }
    return captureScore;
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
    else if(gameTurnCounter >6 && gameTurnCounter <= 18){
        score += perimeter; //reduce the impact of perimeter control in the mid game
        //lanes
        score += corners; //increase the impact of the corners in the mid game
        //King 
        score += kingSafetyScore;
        //Force
        score += forceScore; 
        score += captureScore; 
        
    }
    else{
        score += (perimeter >> 1); //reduce the impact of perimeter control in the late game
        //lanes
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
    undo->pieceIndex = 0xFF; //why this hex?

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

    //I need some way of not checking these two when the turn count is under a certain value but still used in the AI search?
    checkExitFort(s);
    checkSurrounded(s, move.toIndex); //crashing here, atm AI is only playing defender but when it plays attackers it will be needed, need to have filter checks for the current team.

}

void undoMove(GameState *s, UndoInfo *undo){

    //restore flags
    s->kingState = undo->oldKingState;
    s->currentPlayer = undo->oldCurrentPlayer;

    //Restore the moving piece pos directly using the stored index
    if(undo->pieceTeam == TEAM_DEFENDER){
        if(undo->pieceIndex < MAX_DEFENDERS) //sanity check, we set the pieceIndex to 255 at the start and if it doesn't get used properly this will fail.
            s->defenders[undo->pieceIndex].pos = undo->fromIndex;
        
    } else {
        if(undo->pieceIndex < MAX_ATTACKERS)
            s->attackers[undo->pieceIndex].pos = undo->fromIndex;
    }

    //this code is the cause of the zombie pieces
    //In a case where during a search, a piece moves to a index where a previously captured piece "lives" and the AI runs this undo
    //It will restore the wrong piece back to life and restore the wrong index back to life on the boardState

    //Restore board squares,  oldFrom handles the throne case.
    //We havea. snapshot before everything moved.
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

/* ------AI Move Evaluation Functions------*/

WORD minimax(GameState *s, UBYTE depth, WORD alpha, WORD beta, UBYTE maximizingPlayer){
    if(depth == 0 || s->kingState != KING_ALIVE){
        return evaluateBoard(s);
    }

    //ordering in move list?
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
    
    gameTurnCounter++;
    
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
    //mnaybe seperate the valid moves of AI and player, so we don't have to clear them here, but for now this works.
    memset(validMoves, 0, sizeof(validMoves)); //clear valid moves after the search is done, since the getBestMove function is called during the CPU turn and we don't want any leftover valid move markings to interfere with the player's turn.
    validGeneration = 0; //reset valid generation as well since we use that to mark valid moves in the GetValidMoves function, and we don't want any leftover generation markings to interfere with the player's turn.
    
    #ifdef AI_LOGGING
    logWrite("AI getBestMove: from %d to %d, score %d\n", 
        bestMove.fromIndex, bestMove.toIndex, bestEval);
    #endif
    logWrite("AI getBestMove: from %d to %d, score %d\n", 
        bestMove.fromIndex, bestMove.toIndex, bestEval);
    
    logWrite("moveCount: %d\n", moveCount);
    logWrite("Game Turn: %d\n", gameTurnCounter);
    
    // for(UBYTE i = 0; i < 10; i++){
    //     logWrite("Move from %d to %d has a score of %d",moveList[i].fromIndex, moveList[i].toIndex,moveList[i].score);
    // }
    return bestMove;
}