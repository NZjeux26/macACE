#include "game.h"
#include "ai.h"
#include "input.h"
#include "states.h"
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/blit.h> // Blitting fns
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include <ace/managers/sprite.h>
#include <ace/managers/mouse.h>
#include <ace/utils/file.h>
#include <ace/utils/font.h>
#include <ace/utils/string.h>
#include <ace/utils/palette.h>
#include <mini_std/stdio.h>

/*------DEFINES Setup------*/
//Amiga Pal LOWRES is 320x256
#define SCREEN_HEIGHT 256 //32 for the top viewport height
#define SCREEN_WIDTH 320
#define BOARD_WIDTH 256 
#define BOARD_HEIGHT 256
#define BOARD_ORIGIN_X 28 //offset from left edge of screen to start of board
#define BOARD_ORIGIN_Y 3 //offset from top edge of screen to start of board
#define SQUARE_Y 23 //size of each square on the board in pixels
#define SQUARE_X 23
#define PIECE_SPRITE_WIDTH 32 //size of each piece sprite in pixels
#define PIECE_SPRITE_HEIGHT 21
#define CURSOR_SPRITE_WIDTH 16
#define CURSOR_SPRITE_HEIGHT 18
#define CURSOR_SPRITE_CHANNEL 5

//#define OUTPUT_LOGGING //uncomment to enable more logging on arrays and positions in the debug.txt file.

/*------Setting Up Viewports-------*/
static tView *s_pView; // View containing all the viewports
static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer; //only a main screen in this, no score ribbon
//static tRandManager *s_pRandManager;

/*-----Game State Setup-----*/
GameState g_state;
AIMove cpuMove;
/*-----GFX Setup-----*/
static tBitMap *pBmBoard;
static tBitMap *pBmAttackers[MAX_ATTACKERS];
static tBitMap *pBmAttackers_Mask[MAX_ATTACKERS];
static tBitMap *pBmDefenders[MAX_DEFENDERS];
static tBitMap *pBmDefenders_Mask[MAX_DEFENDERS];
static tBitMap *pBmKing; //since the king is a different graphic
static tBitMap *pBmKing_Mask;
static tBitMap *pBmClashFX; //FX to flash when one piece takes another
static tBitMap *pBmClashFX_Mask;
static tBitMap *pBmSquareHighlight; //for highlighting valid moves and selected pieces
static tBitMap *pBmSquareHighlight_Mask;
static tBitMap *pBmSquareHighlight_BG[2]; //for drawing the highlight with the background when it moves, to prevent leaving trails
static tBitMap *pBmMouseCursorSrc;
static tBitMap *pBmMouseCursorData;

static tSprite *pSMouseCursor;

tTextBitMap *gametextbitmapattack; //bitmap for the font
tTextBitMap *gametextbitmapdefend; //bitmap for the font
tTextBitMap *version;

tFont *gFontSmall; //global font for screen


/*-----Global Vars-----*/
ULONG startTime;
UBYTE validMoves[BOARD_SIZE]; //used to hold the valid moves for a selected piece.
UBYTE validGeneration = 0; //used for tracking valid moves in the valid moves array.
UBYTE s_ubBufferIndex = 0; //for double buffering, keeps track of which buffer we're currently drawing to
UBYTE hightlightActive = 0; //whether the highlight for valid moves is currently active, so we know whether to draw it or not in the drawPieces function, and whether to update it when a piece is selected.
UBYTE lastHighlightIndex[2] = {0, 0}; //the index of the last highlighted square, so we can restore the background when the highlight moves to a new square. This is needed because the highlight is drawn directly to the back buffer and not as a sprite, so we have to manually restore the background when it moves.
UBYTE highlightIndex = 0; //the index of the currently highlighted square, so we can update the highlight position when a new piece is selected or a move is made.
UBYTE HLhasBGToRestore[2] = {0, 0};//[2] = {0,0};
UBYTE pieceHasBGToRestore[2] = {0, 0}; //used to track whether the piece we're about to draw has a background that needs to be restored when it moves, so we know whether to blit the background before drawing the piece in its new position. This is needed because the pieces are drawn directly to the back buffer and not as sprites, so we have to manually restore the background when they move.
UBYTE capturedPieceIndex[2][MAX_CAPTURES_PM]; //the index of the piece that was captured in the last move, and then restore the background after.
UBYTE capturedPieceCount[2] = {0, 0}; //the number of pieces that were captured in the last move, so we know how many backgrounds we need to restore
UBYTE gameWinner = 0; //0 no one, 1 attackers, 2 defenders
ScreenPos draw_pos[BOARD_SIZE];

UBYTE moveHistory[10]; //Record the move history so we can track for repetitions
UBYTE cpuPlayerTeam = TEAM_DEFENDER;//manually assigned for now, this will be set via the main menu in the future.
UBYTE humanPlayerTeam = TEAM_ATTACKER;
UBYTE waitFrame = 0;

void gameGsCreate(void) {

    // tRayPos sRayPos = getRayPos();

    // s_pRandManager = randCreate(1+(sRayPos.bfPosY << 8), 1 + sRayPos.bfPosX);

    s_pView = viewCreate(0,TAG_VIEW_GLOBAL_PALETTE, 1,TAG_END);

    s_pVpMain = vPortCreate(0,
    TAG_VPORT_VIEW, s_pView,
    TAG_VPORT_BPP, 5, //needed 5 for 32 colours
    TAG_END);

    s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpMain,
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_INTERLEAVED,
    TAG_SIMPLEBUFFER_IS_DBLBUF, 1, //add this line in for double buffering
    TAG_END);

    paletteLoadFromPath("data/palette/GamePalettev2.plt", s_pVpMain->pPalette, 32);

    drawBoard(); //draw the board to the back buffer before loading the view, so it's visible when the view loads
    
    gFontSmall = fontCreateFromPath("data/font/myacefont.fnt");
    
    spriteManagerCreate(s_pView, 0, NULL);
    systemSetDmaBit(DMAB_SPRITE, 1);

    pBmMouseCursorSrc = bitmapCreateFromPath("data/GFX/mousepointer.bm",0);
    pBmMouseCursorData = bitmapCreate(CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,2,BMF_INTERLEAVED | BMF_CLEAR);
    pSMouseCursor = spriteAdd(CURSOR_SPRITE_CHANNEL,pBmMouseCursorData);

    gametextbitmapattack = fontCreateTextBitMapFromStr(gFontSmall, "ACK");
    gametextbitmapdefend = fontCreateTextBitMapFromStr(gFontSmall, "DEF");
    version = fontCreateTextBitMapFromStr(gFontSmall,"D"); //versioning so I know if the ADF disk updated correctly.

    spriteSetEnabled(pSMouseCursor, 1);
    
    blitCopy(pBmMouseCursorSrc, 0,0,
    pBmMouseCursorData,0,0,CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,MINTERM_COOKIE);

    gameWinner = 0; //reset the game winner at the start of the game, in case we're coming from the menu after a game has ended.

    loadAssets();
    setupPieces(&g_state); //sets up the pieces in their starting positions in the board array and in the piece structs
    setupBoard(); //sets up the draw positions for each square on the board in the draw_pos array
    buildBoard(&g_state); //sets up the board array with the pieces in their starting positions and the special squares marked
    drawPieces(); //draws the board and pieces to the screen, will need to be called again every time a piece moves or is captured
    g_state.currentPlayer = TEAM_ATTACKER;
    //currentPlayer = TEAM_ATTACKER; //reset the current player to the attackers at the start of the game, in case we're coming from the menu after a game has ended.
    
    systemUnuse();
    // Load the view
    viewLoad(s_pView);
  
}

void gameGsLoop(void) {
    // This will loop every frame
    if(keyCheck(KEY_ESCAPE)) {
      gameExit();
    }
    if(keyCheck(KEY_R)){
      if(g_state.currentPlayer == TEAM_ATTACKER){
        gameWinner = 2; //set the winner to defenders, so when we go to the menu it shows the correct win screen.
      } else {
        gameWinner = 1; //set the winner to attackers, so when we go to the menu it shows the correct win screen.
      }
      stateChange(g_pStateManager, g_pMenuState);
      return;
    }
    if(keyCheck(KEY_D)) drawPieces();

    short mouseX = mouseGetX(MOUSE_PORT_1);
    short mouseY = mouseGetY(MOUSE_PORT_1);
    
    //Do mouse stuff here to select and move pieces, check for captures and wins, etc.
    updateMousepos(mouseX, mouseY);

    //if the current player is the human player, allow them to move
    if(g_state.currentPlayer == humanPlayerTeam){
      if(mouseCheck(MOUSE_PORT_1, MOUSE_LMB)){
        onClick(mouseX, mouseY);

        getValidMoves(&g_state, highlightIndex); //get the valid moves for the selected piece and populate the validMoves array
        MoveResult result = {0};
        lastHighlightIndex[!s_ubBufferIndex] = lastHighlightIndex[s_ubBufferIndex]; 
        movePiece(&g_state, lastHighlightIndex[s_ubBufferIndex], highlightIndex, &result); //move the piece in the game state and update the board array, the old and new piece index will need to be passed in here once we have the selection and move working.
        
        if(result.moveComplete){
          //if the move is complete, we need to check if the CPU player needs to make a move next, and if so, we can set a flag to trigger the CPU move in the next frame, this is needed to prevent the CPU move from happening in the same frame as the human move which can cause issues with the game state and rendering.
          if(result.clearHighlight == 1){ //if the move function set the flag to clear the highlight, then we need to clear it
            hightlightActive = 0; //deactivate the highlight
            HLhasBGToRestore[s_ubBufferIndex] = 1; //set the flag to restore the background on the next frame, since the highlight is drawn directly to the back buffer and not as a sprite, we have to manually restore the background when it moves or is cleared.
            HLhasBGToRestore[!s_ubBufferIndex] = 1; //also set the other buffer to restore, since the highlight is drawn directly to the back buffer and not as a sprite, we have to manually restore the background when it moves or is cleared, and since we're double buffering we need to make sure both buffers are restored.
          }
          g_state.currentPlayer = cpuPlayerTeam; //swap the current player to the CPU player, so that in the next frame the CPU will make its move.
        }
        
        //the result struct is used to seperate the global render flags that were previously used so that when a CPU player wants to use the movePiece function and it's sub functions in a branching tree it doesn't affect the game rendering
        if(result.capturedCount[0] > 0 ){
          for(UBYTE i = 0; i < result.capturedCount[0]; i++){
            capturedPieceIndex[0][i] = result.capturedPieceIndexes[0][i];
            capturedPieceIndex[1][i] = result.capturedPieceIndexes[1][i]; 
          }
          capturedPieceCount[0] = result.capturedCount[0];
          capturedPieceCount[1] = result.capturedCount[1];
          pieceHasBGToRestore[0] = 1; //set the flag to restore the background
          pieceHasBGToRestore[1] = 1; //set the flag to restore the background 
        }
      }
    }
    
      //redraw the pieces every frame, 
    drawPieces();

    if (hightlightActive){ //if the highlight for valid moves is active, draw it
      drawSquareHighlight();
    }

    if(g_state.currentPlayer == cpuPlayerTeam){ //if it's the CPU player's turn, calculate the best move and make it
      //waitframe is added to allow both buffers clear from the human players turn.
      if(waitFrame){
        hightlightActive = 0;
        //pass the state to the AI to then play and get a return of what move (from/to) it wants to play
        cpuMove = getBestMove(&g_state);
        //update the board
        MoveResult cpuresult = {0};
        highlightIndex = cpuMove.fromIndex; 
        
        getValidMoves(&g_state,cpuMove.fromIndex);//this might be needed, it may increment valid generation and screw up the move

        lastHighlightIndex[s_ubBufferIndex] = cpuMove.fromIndex; //update the last highlighted index to the CPU move's from index, so that when the highlight moves to the to index we can restore the background of the from index.
        lastHighlightIndex[!s_ubBufferIndex] = cpuMove.fromIndex; 
        highlightIndex = cpuMove.toIndex;
        movePiece(&g_state, cpuMove.fromIndex, cpuMove.toIndex, &cpuresult);
        
        if(cpuresult.moveComplete){
          if(cpuresult.clearHighlight == 1){ //if the move function set the flag to clear the highlight, then we need to clear it
            hightlightActive = 0;
            HLhasBGToRestore[s_ubBufferIndex] = 1; //set the flag to restore the background on the next frame, since the highlight is drawn directly to the back buffer and not as a sprite, we have to manually restore the background when it moves or is cleared.
            HLhasBGToRestore[!s_ubBufferIndex] = 1; //also set the other buffer to restore, 
          }
          g_state.currentPlayer = humanPlayerTeam; //swap the current player back to the human player, so that in the next frame the human can make their move.
        }

        if(cpuresult.capturedCount[0] > 0 ){
            for(UBYTE i = 0; i < cpuresult.capturedCount[0]; i++){
              capturedPieceIndex[0][i] = cpuresult.capturedPieceIndexes[0][i];
              capturedPieceIndex[1][i] = cpuresult.capturedPieceIndexes[1][i]; 
            }
            capturedPieceCount[0] = cpuresult.capturedCount[0];
            capturedPieceCount[1] = cpuresult.capturedCount[1];
            pieceHasBGToRestore[0] = 1; //set the flag to restore the background
            pieceHasBGToRestore[1] = 1; //set the flag to restore the background 
          }
          //Logging
          UBYTE aliveCounter = 0;
          for(UBYTE i = 0; i < MAX_ATTACKERS; i++){
            if(!g_state.attackers[i].captured){
              logWrite("Attacker %d at position %d\n", i, g_state.attackers[i].pos);
              aliveCounter++;
            }
          }
          logWrite("Total alive attackers: %d\n", aliveCounter);

          UBYTE aliveCounterD = 0;
          for(UBYTE i = 1; i < MAX_DEFENDERS; i++){
            if(!g_state.defenders[i].captured){
              logWrite("Defender %d at position %d\n", i, g_state.defenders[i].pos);
              aliveCounterD++; 
            }
          }
          logWrite("Total alive defenders: %d\n", aliveCounterD);
          waitFrame = 0; //reset the wait frame flag, so that the CPU move only happens once per turn and not every frame while it's the CPU player's turn
        }
      else waitFrame = 1;
    }
    
    //these only need drawn once for each buffer frame and since they wont be writeen on again keft alone.
    fontDrawTextBitMap(s_pMainBuffer->pBack, gametextbitmapattack, 6,110,0,FONT_COOKIE);
    fontDrawTextBitMap(s_pMainBuffer->pBack, gametextbitmapdefend, 295,110,0,FONT_COOKIE);
    fontDrawTextBitMap(s_pMainBuffer->pBack, version, 8,8,0,FONT_COOKIE);

    s_ubBufferIndex = !s_ubBufferIndex; //toggle the buffer index for double buffering    

    viewProcessManagers(s_pView);
    copProcessBlocks();
    systemIdleBegin();
    vPortWaitForEnd(s_pVpMain);
    systemIdleEnd();
}
  /* On click
  find the board position given the mouse x/y and the draw_pos array
  check if there's a piece in that position that belongs to the current player using the boardState array
  if there is, calculate the valid moves for that piece and populate the validMoves array
  then when the player clicks a valid move, update the piece's position in its struct and update the boardState array, then call drawPieces() to update the screen. 
  After that, check for captures by looking at the squares around the moved piece in the boardState array, if there's an enemy piece there, 
  check if it's surrounded on the other side by a friendly piece or a special square, if it is, mark it as captured in its struct and update the boardState array to remove it from the board, 
  then call drawPieces() again to update the screen. 
  Finally, check for wins by seeing if the king is captured or if he reaches a corner, and if so, end the game and show a win screen.
  
  */
void gameGsDestroy(void) {
    systemUse();
    AIgameReset();
    for(UBYTE i = 0; i < MAX_ATTACKERS; i++){
      bitmapDestroy(pBmAttackers[i]);
      bitmapDestroy(pBmAttackers_Mask[i]);
    }
    for(UBYTE d = 0; d < MAX_DEFENDERS; d++){
      bitmapDestroy(pBmDefenders[d]);
      bitmapDestroy(pBmDefenders_Mask[d]);
    }
    bitmapDestroy(pBmKing);
    bitmapDestroy(pBmClashFX);
    bitmapDestroy(pBmSquareHighlight);
    bitmapDestroy(pBmSquareHighlight_BG[0]);
    bitmapDestroy(pBmSquareHighlight_BG[1]);
    bitmapDestroy(pBmSquareHighlight_Mask);
    fontDestroyTextBitMap(gametextbitmapattack);
    fontDestroyTextBitMap(gametextbitmapdefend);
    systemSetDmaBit(DMAB_SPRITE, 0);
    spriteManagerDestroy();
    bitmapDestroy(pBmBoard);
    viewDestroy(s_pView);
}
//loads in the game assets, including the piece sprites and their masks for blitting with transparency.
void loadAssets(void){
  for(UBYTE i = 0; i < MAX_ATTACKERS; i++){
    pBmAttackers[i] = bitmapCreateFromPath("data/GFX/attackers.bm",0);
    pBmAttackers_Mask[i] = bitmapCreateFromPath("data/GFX/attackers_mask.bm",0);
  }
  for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
    pBmDefenders[j] = bitmapCreateFromPath("data/GFX/defenders.bm",0);
    pBmDefenders_Mask[j] = bitmapCreateFromPath("data/GFX/defenders_mask.bm",0);
  }
  pBmKing = bitmapCreateFromPath("data/GFX/king.bm",0);
  pBmKing_Mask = bitmapCreateFromPath("data/GFX/king_mask.bm",0);
  pBmClashFX = bitmapCreateFromPath("data/GFX/clashFX.bm",0);
  pBmClashFX_Mask = bitmapCreateFromPath("data/GFX/clashFX_mask.bm",0);
  pBmSquareHighlight = bitmapCreateFromPath("data/GFX/squarehighlight.bm",0);
  pBmSquareHighlight_Mask = bitmapCreateFromPath("data/GFX/squarehighlight_mask.bm",0);
  pBmSquareHighlight_BG[0] = bitmapCreate(32,21,5,0); //size of the highlight sprite, for storing the background when drawing the highlight
  pBmSquareHighlight_BG[1] = bitmapCreate(32,21,5,0); //size of the highlight sprite, for storing the background when drawing the highlight
}

//sets up the pieces in their starting positions in the board array and in the piece structs
void setupPieces(GameState *state){
  
  UBYTE attackerPositions[MAX_ATTACKERS] = { //predefined starting positions for attackers
    17,18,19,20,21,32,
    63,76,88,89,102,115,  
    79,66,53,92,105,80,
    147,148,149,150,151,136
  };

  UBYTE defenderPositions[MAX_DEFENDERS] = { //predefined starting positions for defenders, including the king
    84,58,70,71,72,82,83,85,86,96,97,98,110
  };
 
  for(int i = 0; i < MAX_DEFENDERS; i++){
    state->defenders[i].type = (i == 0) ? KING : DEFENDER; // First piece is the king, the rest are defenders
    state->defenders[i].team = TEAM_DEFENDER;
    state->defenders[i].captured = 0;
    state->defenders[i].pos = defenderPositions[i]; // Assign starting positions from the predefined array First position is the throne in the middle, the rest are around it
  }

  // Set up the attackers
  for(int i = 0; i < MAX_ATTACKERS; i++){
    state->attackers[i].type = ATTACKER;
    state->attackers[i].team = TEAM_ATTACKER;
    state->attackers[i].captured = 0;
    state->attackers[i].pos = attackerPositions[i]; // Assign starting positions from the predefined array
  }
}

//sets up the draw positions for each square on the board in the draw_pos array, which is indexed the same as the board array, so we can easily get the screen position for any square on the board when we need to draw pieces or highlights.
void setupBoard(void){
  for(UBYTE y = 0; y < 13; y++){
    for(UBYTE x = 0; x < 13; x++){
      UBYTE i = y * 13 + x; //calculate the index in the board array for this position
      draw_pos[i].x = BOARD_ORIGIN_X + (x - 1) * SQUARE_X; //calculate the screen position for this square based on the origin and square size
      draw_pos[i].y = BOARD_ORIGIN_Y + (y - 1) * SQUARE_Y;
    }
  }

  //DEBUG: print the draw positions to the log to check they're correct
  #ifdef OUTPUT_LOGGING
  for(UBYTE i = 0; i < 169; i++){
    logWrite("Draw POS: %d is x: %d y: %d \n", i, draw_pos[i].x, draw_pos[i].y);
  }
  #endif
}

//sets up the board array with the pieces in their starting positions and the special squares marked
void buildBoard(GameState *state){
    //0 = empty square, 1 = occupied by defender, 2 = occupied by attacker, 3 = occupied by king, 4 = special square ,99 = dead zone for out of bounds
    //initialize the board array to all 0s
    for(int i = 0; i < 169; i++){
        state->boardState[i] = 0;
    }

    //set up the special positions for the corners and throne
    state->boardState[14] = 4; //top left corner
    state->boardState[24] = 4; //top right corner
    state->boardState[144] = 4; //bottom left corner
    state->boardState[154] = 4; //bottom right corner
    state->boardState[84] = 4; //throne in the middle

    //place the pieces on the board according to their positions in the piece structs
    for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
        if(state->defenders[j].type == KING){
            state->boardState[state->defenders[j].pos] = 3; //king
        } else {
            state->boardState[state->defenders[j].pos] = 1; //defender
        }
    }
    //place the attackers on the board according to their positions in the piece structs
    for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
        state->boardState[state->attackers[k].pos] = 2; //attacker
        //logWrite("Placing attacker at index %d\n", state->attackers[k].pos);
    }
    //sets the border squares to 99 to prevent out of bounds errors when checking for moves and captures
    for(UBYTE i = 0; i < 169; i++){
        if(i < 13 || i >= 156 || i % 13 == 0 || i % 13 == 12){
            state->boardState[i] = 99; //dead zone
            //logWrite("Setting board index %d to 99\n", i);
        }
    }

    //DEBUG: print the board to the log to check it's set up correctly
    #ifdef OUTPUT_LOGGING
    for(UBYTE i = 0; i < 169; i++){
        logWrite("Board POS: %d is %d \n", i, state->boardState[i]);
    }
    #endif
}

void drawPieces(void){ 
  //More efficent method, go through each defender or attacker in the respective arrays, if not captured, draw.
  for(UBYTE j = 0; j < MAX_DEFENDERS; j++){ //start at 1 since the king is zero
    UBYTE i = g_state.defenders[j].pos;
    //The first position in the array is always the king
    if(j == 0){
      blitCopyMask(pBmKing,0,0,
      s_pMainBuffer->pBack, draw_pos[i].x, draw_pos[i].y,
      PIECE_SPRITE_WIDTH,PIECE_SPRITE_HEIGHT,pBmKing_Mask->Planes[0]);
    }
    else{
      if(!g_state.defenders[j].captured){
        blitCopyMask(pBmDefenders[j],0,0, 
        s_pMainBuffer->pBack, draw_pos[i].x, draw_pos[i].y,
        PIECE_SPRITE_WIDTH,PIECE_SPRITE_HEIGHT,pBmDefenders_Mask[j]->Planes[0]); 
      }
    }
  }
  for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
    UBYTE i = g_state.attackers[k].pos;
    if(!g_state.attackers[k].captured){
      blitCopyMask(pBmAttackers[k],0,0,
      s_pMainBuffer->pBack, draw_pos[i].x, draw_pos[i].y,
      PIECE_SPRITE_WIDTH,PIECE_SPRITE_HEIGHT,pBmAttackers_Mask[k]->Planes[0]);
    }
  }

  //if the highlight for valid moves is not active, we can restore the background of the last highlighted square to erase the highlight
  if(hightlightActive == 0){ 
    if(HLhasBGToRestore[s_ubBufferIndex]){ //check if there's a background to restore
      blitCopy(pBmBoard, draw_pos[lastHighlightIndex[s_ubBufferIndex]].x, draw_pos[lastHighlightIndex[s_ubBufferIndex]].y,
      s_pMainBuffer->pBack, draw_pos[lastHighlightIndex[s_ubBufferIndex]].x, draw_pos[lastHighlightIndex[s_ubBufferIndex]].y,
      32, 21, MINTERM_COOKIE);
      
      HLhasBGToRestore[s_ubBufferIndex] = 0; //reset the flag after restoring
    }
 }
  //if the piece that was just captured has a background that needs to be restored, restore it to erase the piece from the screen
  if(pieceHasBGToRestore[s_ubBufferIndex]){ 
    for(UBYTE c = 0; c < capturedPieceCount[s_ubBufferIndex]; c++){
      blitCopy(pBmBoard, draw_pos[capturedPieceIndex[s_ubBufferIndex][c]].x, draw_pos[capturedPieceIndex[s_ubBufferIndex][c]].y,
      s_pMainBuffer->pBack, draw_pos[capturedPieceIndex[s_ubBufferIndex][c]].x, draw_pos[capturedPieceIndex[s_ubBufferIndex][c]].y,
      32, 21, MINTERM_COOKIE);
    }
    pieceHasBGToRestore[s_ubBufferIndex] = 0; //reset the flag after restoring
    capturedPieceCount[s_ubBufferIndex] = 0;
  }

  //this manages the clash symbol which shows who's turn it is.
  if(g_state.currentPlayer == TEAM_ATTACKER){
    //undraw the clash from the defender side and redraw it on the attacker side
    blitCopy(pBmBoard, 288, 119,
    s_pMainBuffer->pBack, 288, 119, PIECE_SPRITE_WIDTH, PIECE_SPRITE_HEIGHT, MINTERM_COOKIE);

    blitCopyMask(pBmClashFX,0,0,
    s_pMainBuffer->pBack, 0, 119, PIECE_SPRITE_WIDTH, 20, pBmClashFX_Mask->Planes[0]);
  } else {
    blitCopy(pBmBoard, 0, 119,
    s_pMainBuffer->pBack, 0, 119, PIECE_SPRITE_WIDTH, 20, MINTERM_COOKIE);

    blitCopyMask(pBmClashFX,0,0,
    s_pMainBuffer->pBack, 288, 119, PIECE_SPRITE_WIDTH, 20, pBmClashFX_Mask->Planes[0]);
  }
}

void drawBoard(void){
   pBmBoard = bitmapCreateFromPath("data/GFX/BG1.bm",0);
    //add asserts here later to check all this logic
    for(UWORD x = 0; x < s_pMainBuffer->uBfrBounds.uwX; x+=16){//fills out the background
    for(UWORD y = 0; y < s_pMainBuffer->uBfrBounds.uwY; y+=16){
      blitCopyAligned(pBmBoard,x,y,s_pMainBuffer->pBack,x,y,16,16);
        blitCopyAligned(pBmBoard,x,y,s_pMainBuffer->pFront,x,y,16,16);
        }
    }
   
}

void updateMousepos(short mouseX, short mouseY){
  pSMouseCursor->wX = mouseX;
  pSMouseCursor->wY = mouseY;
    
  spriteProcess(pSMouseCursor);
  spriteProcessChannel(CURSOR_SPRITE_CHANNEL);
}

void onClick(short mouseX, short mouseY){
  for(UBYTE i = 0; i < 169; i++){
    //check if the mouse is within the bounds of this square
    if(mouseX >= draw_pos[i].x && mouseX <= draw_pos[i].x + SQUARE_X &&
       mouseY >= draw_pos[i].y && mouseY <= draw_pos[i].y + SQUARE_Y){
         logWrite("Clicked on square index %d\n", i); 
         //If a square is already Highlighted, set to zero for it to be restored
         if(!hightlightActive && g_state.boardState[i] == 0) return; //if the highlight isn't active and the square clicked is empty, do nothing

         if(hightlightActive){ 
          //  logWrite("Undraw Highlighted Index = %d\n", highlightIndex);
            hightlightActive = 0; 
         }
         
         highlightIndex = i; //set the highlight index to the square that was clicked

         hightlightActive = 1; //activate the highlight for valid moves
         //logWrite("Highlighted Index = %d\n", highlightIndex);
        
         break; //exit the loop once we've found the square that was clicked
    }
  }
}
 //this function will be used to draw the highlight for valid moves and selected pieces, 
 //it will be called in the drawPieces function if the highlightActive variable is true, and the position will be determined by the highlightIndex variable which will be set when a piece is selected or a move is made.
void drawSquareHighlight(void){
 
  if(!hightlightActive) return;
  //First check if there's a background to restore from the last highlighted square, and if the highlight has moved to a new square, restore the background of the old highlighted square before drawing the new one
  if(highlightIndex != lastHighlightIndex[s_ubBufferIndex]){
    //logWrite("Restoring background for index %d\n", lastHighlightIndex[s_ubBufferIndex]);
    //redraw the background to erase the old highlight
    
    blitCopy(pBmBoard, draw_pos[lastHighlightIndex[s_ubBufferIndex]].x, draw_pos[lastHighlightIndex[s_ubBufferIndex]].y,
    s_pMainBuffer->pBack, draw_pos[lastHighlightIndex[s_ubBufferIndex]].x, draw_pos[lastHighlightIndex[s_ubBufferIndex]].y,
    32, 21, MINTERM_COOKIE);
    
    HLhasBGToRestore[s_ubBufferIndex] = 0;
  }
    
  //Then draw the highlight with the mask for transparency
  blitCopyMask(pBmSquareHighlight,0,0,
  s_pMainBuffer->pBack, draw_pos[highlightIndex].x, draw_pos[highlightIndex].y,
  32,21,pBmSquareHighlight_Mask->Planes[0]);

  HLhasBGToRestore[s_ubBufferIndex] = 1;
  lastHighlightIndex[s_ubBufferIndex] = highlightIndex; //update the last highlighted index to the current one
}

/* This function will calculate the valid moves for the currently highlighted piece and populate the validMoves array, which is indexed the same as the board array, with a value over 0 for valid moves and 0 for invalid moves.*/
void getValidMoves(GameState *state, UBYTE selectedIndex){
  //selectedIndex == highlightedindex
  //First check if the square is empty, a special square or out of bounds, there are no valid moves to calculate, so return
  if(state->boardState[selectedIndex] == 0 || state->boardState[selectedIndex] == 4 || state->boardState[selectedIndex] == 99){ 
    return;
  }
  //increment valid generation to mark the current valid moves, this way we can avoid having to clear the validMoves array every time.
  validGeneration++;

  //If generation is 0, it means that the UBYTE has >255 and the array needs reset
  if(validGeneration == 0){
    memset(validMoves, 0, sizeof(validMoves));
    validGeneration = 1;
  }
  
  //need to check whether the piece is the king, if so it's allowed on squares in boardState that = 4.
  UBYTE isKing = (state->boardState[selectedIndex] == 3);

  /* This is no risk of over or under flow. Only 13-154 are valid game squares and only 14-153 are valid for piece movement */

  /*lets check the rows which are +1 and -1 This needs the minus (or plus) so the index doesn't start on the piece selected and auto fails.*/
  for(UBYTE r = (selectedIndex +1); r < 169; r++){ //rows in the + direction
    //if the square is occupied, break
    if(state->boardState[r] > 0 && state->boardState[r] <=3){
      break; //if greater than 0 it means th square is occupied, a special square or out of bounds and invalid
    }
    else if(state->boardState[r] == 4 && !isKing){ //if it's a special square and the piece isn't the king, break
      break;
    }
    else if(state->boardState[r] == 99){ //if it's out of bounds, break
      break;
    }
    validMoves[r] = validGeneration; //add the current position to the valid moves array. 
  }
  
  for(UBYTE u = (selectedIndex - 1); u < 169; u--){ //rows in the - direction
    if(state->boardState[u] > 0 && state->boardState[u] <=3){
      break; 
    }
    else if(state->boardState[u] == 4 && !isKing){
      break;
    }
    else if(state->boardState[u] == 99){
      break;
    }
    validMoves[u] = validGeneration; 
  }

  /* **Check Coloums** */

  for(UBYTE c = (selectedIndex +13); c < 169; c=c+13){ 
    if(state->boardState[c] > 0 && state->boardState[c] <=3){
      break;
    }
    else if(state->boardState[c] == 4 && !isKing){
      break;
    }
    else if(state->boardState[c] == 99){
      break;
    }
    validMoves[c] = validGeneration; 
  }
  
  for(UBYTE y = (selectedIndex -13); y < 169; y=y-13){ 
    if(state->boardState[y] > 0 && state->boardState[y] <=3){
      break; 
    }
    else if(state->boardState[y] == 4 && !isKing){
      break;
    }
    else if(state->boardState[y] == 99){
      break;
    }
    validMoves[y] = validGeneration; 
  }
  
  /* Warning, this will not print the full array if you select a square and then quit, you must select another square (empty or not)*/
  #ifdef OUTPUT_LOGGING
  logWrite("validGeneration = %d\n", validGeneration);
  for(UBYTE i = 0; i < 169; i++){
    if(validMoves[i] == validGeneration){
      logWrite("Valid Moves at Square %d\n",i);
    }
  }
  #endif
}

/*
  This function will move the currently highlighted piece to a new square if it's a valid move,
  then update the boardState array and the piece's struct with the new position, 
  and finally call turn off the highligh and set the flag to restore the background.
*/
void movePiece(GameState *state, UBYTE oldIndex, UBYTE newIndex, MoveResult *result){
  
  //check if the selected new square is a valid move by checking the validMoves array at the highlightIndex, if it's not valid, return and do nothing
  if(validMoves[newIndex] != validGeneration){
    //logWrite("Invalid move attempted to index %d\n", highlightIndex);
    return;
  }

  //find the piece that is being moved by checking the boardState at the lastHighlightIndex to see if it's a defender or the king
  //then loop through the defenders array to find the piece with the matching position and update its position to the new highlightIndex
  if(state->currentPlayer == TEAM_DEFENDER){
   
    for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
      if(state->defenders[j].pos == oldIndex && !state->defenders[j].captured){
        state->defenders[j].pos = newIndex; //update the piece's position in its struct
        
        if(state->defenders[j].type == KING) state->boardState[newIndex] = 3; //update the boardState array with the new position of the piece, 3 for king
        else state->boardState[newIndex] = 1; //update the boardState array with the new position of the piece, 1 for defender
        
        state->boardState[oldIndex] = 0; //set the old position to 0 for empty *This will override the throne if the king moved off it.
        
        if (state->boardState[84] == 0) state->boardState[84] = 4; //if the king moves off the throne, set the throne to 4 which is a special square. 
        
        //state->currentPlayer = TEAM_ATTACKER; //swap current player here
        result->moveComplete = 1; //set the flag to indicate the move is complete, this will be used by the CPU player to know when to make its move after the human player moves.
        break;
      }
    }
  } else {
    //do the same for attackers
    for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
      if(state->attackers[k].pos == oldIndex && !state->attackers[k].captured){
        state->attackers[k].pos = newIndex;
       
        state->boardState[newIndex] = 2; //update the boardState array with the new position of the piece, 2 for attacker
        state->boardState[oldIndex] = 0; //set the old position to 0 for empty
   
        result->moveComplete = 1; //set the flag to indicate the move is complete, this will be used by the CPU player to know when to make its move after the human player moves.
        break;
      }
    }
  }

  //these will need removed or done some other way. A CPU player wouldn't be clicking to move piece so this would fire, but not actually do anything.
  result->clearHighlight = 1; //set the flag to clear the highlight after a move is made

  checkForCaptures(&g_state, newIndex, result);
  if(capturedPieceCount[s_ubBufferIndex] == 0){
    checkShieldWallCaptures(&g_state, newIndex, result);
    if(g_state.currentPlayer == TEAM_DEFENDER) checkExitFort(&g_state); //exit fort only applys to defenders
    if(g_state.currentPlayer == TEAM_ATTACKER) checkSurrounded(&g_state, newIndex); //only the attackers surround and no point checking when defenders move
  }
  
  checkGameEnd();
}

void checkForCaptures(GameState *state, UBYTE pieceIndex, MoveResult *result){
  //after a piece is moved, we need to check if it has captured any enemy pieces by looking at the squares around the moved piece in the boardState array, if there's an enemy piece there, 
  //check if it's surrounded on the other side by a friendly piece or a special square, if it is, mark it as captured in its struct and update the boardState array to remove it from the board, 
  //then call drawPieces() again to update the screen. 

  UBYTE RIGHT = state->boardState[pieceIndex +1]; 
  UBYTE LEFT = state->boardState[pieceIndex -1];
  UBYTE UP = state->boardState[pieceIndex -13];
  UBYTE DOWN = state->boardState[pieceIndex +13];

  UBYTE currentPieceTeam = state->boardState[pieceIndex]; //the piece that was just moved, used to check which team it belongs to for capture rules
  
  UBYTE RIGHTINDEX = pieceIndex +1; //the index of the square to the right of the moved piece, used to check for captures in that direction
  UBYTE LEFTINDEX = pieceIndex -1;
  UBYTE UPINDEX = pieceIndex -13;
  UBYTE DOWNINDEX = pieceIndex +13;
  
  UBYTE neighbours[4] = {RIGHT, LEFT, UP, DOWN};
  UBYTE neighbourIndexes[4] = {RIGHTINDEX, LEFTINDEX, UPINDEX, DOWNINDEX};
  BYTE directions[4] = {1, -1, -13, +13}; //used to check the square on the other side of the enemy piece for capture rules

  for(UBYTE i = 0; i < 4; i++){
    UBYTE neighbour = neighbours[i];
    UBYTE neighbourIndex = neighbourIndexes[i];
    BYTE direction = directions[i];
  
    if(neighbour > 0 && neighbour < 10 && neighbour != currentPieceTeam){ //if there's an enemy piece in this direction, check for capture
        //logWrite("Checking capture in direction %d at index %d\n", direction, neighbourIndex);
        UBYTE isKing = (neighbour == 3); //if the piece we are checking is the king, we need to check if it's captured differently, so we set a flag for that.

        if(isKing){
          //we need to check if the king is surrounded on all four sides by attackers or special squares, if he is, he's captured and the attackers win.
          //shouldn't trigger when the king is against the corners since the edge of the board is out of bounds and not a special square, so it won't count as a capture condition for the king.
          UBYTE rightOfKing = state->boardState[neighbourIndex +1];
          UBYTE leftOfKing = state->boardState[neighbourIndex -1];
          UBYTE upOfKing = state->boardState[neighbourIndex -13];
          UBYTE downOfKing = state->boardState[neighbourIndex +13];
          
          //logWrite("Checking if king is captured, right: %d, left: %d, up: %d, down: %d\n", rightOfKing, leftOfKing, upOfKing, downOfKing);
          if((rightOfKing == 2 || rightOfKing == 4) && (leftOfKing == 2 || leftOfKing == 4) && (upOfKing == 2 || upOfKing == 4) && (downOfKing == 2 || downOfKing == 4)){
            state->defenders[0].captured = 1; //mark the king as captured and needs removed from screen the king is always at index 0 in the defenders array
            result->capturedPieceIndexes[0][0] = state->defenders[0].pos; //store the index of the captured piece
            result->capturedPieceIndexes[1][0] = state->defenders[0].pos;
            result->capturedCount[0] = 1;
            result->capturedCount[1] = 1;
            state->boardState[neighbourIndex] = 0; //update the boardState array to remove it from the board
            state->kingState = KING_CAPTURED; //set the flag to indicate the king is captured
          }  
          //return; //exit the function since the game is over
        }

        if((state->boardState[neighbourIndex + direction]) == currentPieceTeam || //if the piece on the other side is on the same team
           (state->boardState[neighbourIndex + direction] == 4 || // Of next to a special square
           (currentPieceTeam == 3 && state->boardState[neighbourIndex + direction] == TEAM_DEFENDER) || //or the current piece is the king and the other piece is a defender
           (state->boardState[neighbourIndex + direction] == 3 && currentPieceTeam == TEAM_DEFENDER))){  //or the current piece is a defender and the other side is the king

          //mark the piece as captured in its struct and update the boardState array to remove it from the board 
          
          if(currentPieceTeam == TEAM_DEFENDER || currentPieceTeam == 3){ //if the piece that moved is a defender or the king, it can only capture attackers
            for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
              if(state->attackers[k].pos == (neighbourIndex) && !state->attackers[k].captured){
                
                state->attackers[k].captured = 1; //mark the piece as captured and needs removed from screen
                //state->attackers[k].pos = 39;
                UBYTE slot = result->capturedCount[s_ubBufferIndex];
                if(slot < MAX_CAPTURES_PM){
                
                result->capturedPieceIndexes[0][slot] = state->attackers[k].pos; //store the index of the captured piece
                result->capturedPieceIndexes[1][slot] = state->attackers[k].pos;
                result->capturedCount[0]++;
                result->capturedCount[1]++;
                state->boardState[neighbourIndex] = 0; //update the boardState array to remove it from the board
                }
                break;
              }
            }
          } 
          else if(currentPieceTeam == TEAM_ATTACKER){ //if the piece that moved is an attacker, it can only capture defenders and the king
            for(UBYTE j = 1; j < MAX_DEFENDERS; j++){ //start the array at 1 since 0 is always the king and the king can only be captured by the king capture rules.
              if(state->defenders[j].pos == (neighbourIndex) && !state->defenders[j].captured){ 
                
                state->defenders[j].captured = 1; 
                //state->defenders[j].pos = 51; //why did this go off randomly? This is being called in the AI so 
                UBYTE slot = result->capturedCount[s_ubBufferIndex]; //why is this tied to the buffer index? This is being used to store the captured pieces for the animation, so it needs to be per buffer to avoid conflicts between the two players when they both capture pieces on the same turn.
                if(slot < MAX_CAPTURES_PM){
                  result->capturedPieceIndexes[0][slot] = state->defenders[j].pos; 
                  result->capturedPieceIndexes[1][slot] = state->defenders[j].pos;
                  result->capturedCount[0]++;
                  result->capturedCount[1]++;
                  state->boardState[neighbourIndex] = 0; 
                }
                break;
              }
            }
          }
        }
      }
  }
}

void checkGameEnd(void){
  //if the game ends by either the king being captured or the king escaping, stop the game and restart.

  //if the king is captured, Attackers Win
  if(g_state.kingState == KING_CAPTURED){
    gameWinner = 1;
    stateChange(g_pStateManager, g_pMenuState);
    return;
  }

  //The King escapes, the Defenders Win
  else if(g_state.boardState[14] == 3 || g_state.boardState[24] == 3 || g_state.boardState[144] == 3 || g_state.boardState[154] == 3 || g_state.kingState == KING_ESCAPED){
    gameWinner = 2;
    stateChange(g_pStateManager, g_pMenuState);
    return;
  }
  
}

void checkShieldWallCaptures(GameState *state, UBYTE pieceIndex, MoveResult *result){

  UBYTE currentPieceTeam = state->boardState[pieceIndex];
  
  // Must be on an edge (one of the 4 neighbours is 99)
  UBYTE onEdge = (state->boardState[pieceIndex + 1] == 99 || 
                  state->boardState[pieceIndex - 1] == 99 ||
                  state->boardState[pieceIndex - 13] == 99 || 
                  state->boardState[pieceIndex + 13] == 99);
  if(!onEdge) return;

  // Must not be a corner (two perpendicular neighbours are 99)
  UBYTE isCorner = ((state->boardState[pieceIndex + 1] == 99 || state->boardState[pieceIndex - 1] == 99) &&
                    (state->boardState[pieceIndex - 13] == 99 || state->boardState[pieceIndex + 13] == 99));
  if(isCorner) return;

  // Determine front direction (perpendicular, into the board)
  BYTE frontDir = 0;
  if(state->boardState[pieceIndex - 13] == 99) frontDir = +13; // top edge, front is downward
  if(state->boardState[pieceIndex + 13] == 99) frontDir = -13; // bottom edge, front is upward
  if(state->boardState[pieceIndex - 1] == 99)  frontDir = +1;  // left edge, front is rightward
  if(state->boardState[pieceIndex + 1] == 99)  frontDir = -1;  // right edge, front is leftward

  // Directions to sweep along the edge
  BYTE edgeDirs[2];
  if(frontDir == +13 || frontDir == -13){ edgeDirs[0] = +1;  edgeDirs[1] = -1;  } // top/bottom edge, sweep horizontally
  else                                  { edgeDirs[0] = +13; edgeDirs[1] = -13; } // left/right edge, sweep vertically

  for(UBYTE d = 0; d < 2; d++){
    BYTE edgeDir = edgeDirs[d];

    UBYTE wallPieces[11];
    UBYTE wallCount = 0;
    UBYTE validWall = 1;
    UBYTE farEndBracketed = 0;

    UBYTE curr = pieceIndex;

    while(1){
      curr += edgeDir;

      UBYTE piece = state->boardState[curr];

      // Off the board entirely
      if(piece == 99) break;

      // Empty square, wall is broken
      if(piece == 0) break;

      // Friendly piece brackets the far end
      if(piece == currentPieceTeam ||
        (currentPieceTeam == 3 && piece == TEAM_DEFENDER) ||
        (currentPieceTeam == TEAM_DEFENDER && piece == 3)){
        farEndBracketed = 1;
        break;
      }

      // Check if the square past this piece along the edge is 99 (corner bracket)
      if(state->boardState[curr + edgeDir] == 99){
        // Check this piece has a friendly in front before counting it
        UBYTE frontPiece = state->boardState[curr + frontDir];
        UBYTE hasFrontFriendly = (frontPiece == currentPieceTeam ||
          (currentPieceTeam == 3 && frontPiece == TEAM_DEFENDER) ||
          (currentPieceTeam == TEAM_DEFENDER && frontPiece == 3));

        if(hasFrontFriendly){
          // King can't be captured by shield wall
          if(piece != 3) wallPieces[wallCount++] = curr;
        } else {
          validWall = 0;
        }
        farEndBracketed = 1;
        break;
      }

      // King can't be captured by shield wall
      if(piece == 3) break;

      // Enemy piece — check it has a friendly in front
      UBYTE frontPiece = state->boardState[curr + frontDir];
      UBYTE hasFrontFriendly = (frontPiece == currentPieceTeam ||
        (currentPieceTeam == 3 && frontPiece == TEAM_DEFENDER) ||
        (currentPieceTeam == TEAM_DEFENDER && frontPiece == 3));

      if(!hasFrontFriendly){
        validWall = 0;
        break;
      }

      wallPieces[wallCount++] = curr;
    }

    if(wallCount < 2 || !validWall || !farEndBracketed) continue;

    for(UBYTE w = 0; w < wallCount; w++){
      UBYTE idx = wallPieces[w];

      if(currentPieceTeam == TEAM_ATTACKER){
        for(UBYTE j = 0; j < MAX_DEFENDERS; j++){
          if(state->defenders[j].pos == idx && !state->defenders[j].captured){
            state->defenders[j].captured = 1;
            UBYTE slot = result->capturedCount[s_ubBufferIndex];
            if(slot < MAX_CAPTURES_PM){
              result->capturedPieceIndexes[0][slot] = idx;
              result->capturedPieceIndexes[1][slot] = idx;
              result->capturedCount[0]++;
              result->capturedCount[1]++;
            }
            state->boardState[idx] = 0;
            break;
          }
        }
      } else { // TEAM_DEFENDER or king capturing attackers
        for(UBYTE k = 0; k < MAX_ATTACKERS; k++){
          if(state->attackers[k].pos == idx && !state->attackers[k].captured){
            state->attackers[k].captured = 1;
            UBYTE slot = result->capturedCount[s_ubBufferIndex];
            if(slot < MAX_CAPTURES_PM){
              result->capturedPieceIndexes[0][slot] = idx;
              result->capturedPieceIndexes[1][slot] = idx;
              result->capturedCount[0]++;
              result->capturedCount[1]++;
            }
            state->boardState[idx] = 0;
            break;
          }
        }
      }
    }
  }
}
// Uses a flood fill from the king to check if the king is surrounded by a ring of defenders and is against the edge.
void checkExitFort(GameState *state){
  UBYTE kingPos = 0;
  //the king is always the first piece in the defenders array.
  kingPos = state->defenders[0].pos;

  // King must be touching the edge
  if(!(state->boardState[kingPos + 1] == 99 || state->boardState[kingPos - 1] == 99 ||
       state->boardState[kingPos - 13] == 99 || state->boardState[kingPos + 13] == 99)) return;

  // King must have at least one empty square to move to
  UBYTE canMove = (state->boardState[kingPos + 1] == 0 || state->boardState[kingPos - 1] == 0 ||
                   state->boardState[kingPos - 13] == 0 || state->boardState[kingPos + 13] == 0);
  if(!canMove) return;

  // Flood fill from king position through empty squares
  // If we only ever hit defenders or 99 on the boundary, the fort is sealed
  UBYTE visited[169] = {0};
  UBYTE queue[169];
  UBYTE qHead = 0;
  UBYTE qTail = 0;
  UBYTE fortValid = 1;

  queue[qTail++] = kingPos;
  visited[kingPos] = 1;

  BYTE dirs[4] = {1, -1, 13, -13};

  while(qHead < qTail){
    UBYTE curr = queue[qHead++];

    for(UBYTE i = 0; i < 4; i++){
      UBYTE next = curr + dirs[i];
      if(visited[next]) continue;
      visited[next] = 1;

      UBYTE piece = state->boardState[next];

      if(piece == 99) continue;           // board edge, fine
      if(piece == TEAM_DEFENDER) continue; // defender on boundary, fine
      if(piece == TEAM_ATTACKER){          // attacker can get in, fort broken
        fortValid = 0;
        break;
      }
      if(piece == 0){                      // empty square inside fort, keep filling
        queue[qTail++] = next;
      }
    }
    if(!fortValid) break;
  }

  if(fortValid){
    state->kingState = KING_ESCAPED; //set the flag to indicate the king has escaped
  }

}
/*Checks if the King and all defenders are surrounded. We use a flood fill algorithm to check if the king/defenders are srrounded
then we count the total amount of defenders inside the ring, if it matches the total number of alive defenders the king is caputred.*/
void checkSurrounded(GameState *state, UBYTE pieceIndex){
  //if current selected piece is a defender then break, this is for checking if the attackers have surrounded the king only.
  if (state->boardState[pieceIndex] == 1 || state->boardState[pieceIndex] == 3) return;
  //king is always the first in the defenders array, so we can just get the position from there.
  UBYTE kingPos = state->defenders[0].pos;
  
  // Count total non-captured defenders including king
  
  UBYTE totalDefenders = 0;
  for(UBYTE i = 0; i < MAX_DEFENDERS; i++){
    if(!state->defenders[i].captured) totalDefenders++;
  }
  // visited[] tracks which squares the flood fill has already checked,
  // so we don't visit the same square twice and loop forever.
  UBYTE visited[169] = {0};
  // queue[] is the list of squares waiting to be checked. 
  // qHead is the next square to process, qTail is where we add new squares.
  // When qHead == qTail, the queue is empty and the fill is complete.
  UBYTE queue[169];
  UBYTE qHead = 0;
  UBYTE qTail = 0;

  UBYTE reachedEdge = 0;  // set to 1 if the fill reaches a 99 border square
  UBYTE defendersInRing = 1; // start with 1 to count the king itself

  // Seed the flood fill from the king's position
  queue[qTail++] = kingPos;
  visited[kingPos] = 1;

  // The four directions to check from each square: right, left, up, down
  BYTE dirs[4] = {1, -1, 13, -13};

  // Process each square in the queue until the queue is empty or we reach the edge
  while(qHead < qTail){
    UBYTE curr = queue[qHead++]; // take the next square off the front of the queue

    for(UBYTE i = 0; i < 4; i++){
      UBYTE next = curr + dirs[i]; // the square in this direction
      
      if(visited[next]) continue; // already checked this square, skip it
      visited[next] = 1;          // mark it as visited so we don't check it again

      UBYTE piece = state->boardState[next];

      // Reached the board border — the fill has escaped the ring,
      // meaning the attackers have NOT fully surrounded everyone.
      if(piece == 99){
        reachedEdge = 1;
        break;
      }
       // Attacker square — this is the wall of the ring. 
      // Don't add it to the queue, the fill stops here in this direction.

      if(piece == TEAM_ATTACKER) continue; // boundary of the ring
      
      // Found a defender or the king inside the ring — count them and
      // add their square to the queue so we continue filling from there.
      if(piece == TEAM_DEFENDER || piece == 3){
        defendersInRing++;               // count defenders inside the ring
        queue[qTail++] = next;
      }
      // Empty square inside the ring — add to queue to keep filling outward
      if(piece == 0){
        queue[qTail++] = next;
      }
    }
    if(reachedEdge) break; // no point continuing if we've already escaped the ring
  }

  // Ring is only valid if all defenders are trapped inside it
  if(!reachedEdge && defendersInRing == totalDefenders){
    state->kingState = KING_CAPTURED;
  }
}