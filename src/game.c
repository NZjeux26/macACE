#include "game.h"
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/blit.h> // Blitting fns
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include <ace/managers/sprite.h>
#include <ace/utils/file.h>
#include <ace/utils/font.h>
#include <ace/utils/string.h>
#include <ace/utils/palette.h>

/*------DEFINES Setup------*/
//Amiga Pal LOWRES is 320x256
#define SCREEN_HEIGHT 256 //32 for the top viewport height
#define SCREEN_WIDTH 320
#define MAX_DEFENDERS 13  //12 Defenders + King
#define MAX_ATTACKERS 24  //24 Attackers

/*------Setting Up Viewports-------*/
static tView *s_pView; // View containing all the viewports
static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer; //only a main screen in this, no score ribbon
static tRandManager *s_pRandManager;

/*-----Game Piece Setup-----*/
g_piece attackers[MAX_ATTACKERS];
g_piece defenders[MAX_DEFENDERS];

/*-----GFX Setup-----*/
static tBitMap *pBmBackground;
static tBitMap *pBmAttackers;
static tBitMap *pBmDefenders;
static tBitMap *pBmKing; //since the king is a different graphic
static tBitMap *pBmClashFX; //FX to flash when one piece takes another

tFont *gFontSmall; //global font for screen
tTextBitMap *testingbitmap;
/*-----Global Vars-----*/
ULONG startTime;

void gameGsCreate(void) {

    tRayPos sRayPos = getRayPos();

    s_pRandManager = randCreate(1+(sRayPos.bfPosY << 8), 1 + sRayPos.bfPosX);

    s_pView = viewCreate(0,TAG_VIEW_GLOBAL_PALETTE, 1,TAG_END);

    s_pVpMain = vPortCreate(0,
    TAG_VPORT_VIEW, s_pView,
    TAG_VPORT_BPP, 5, //needed 5 for 32 colours
    TAG_END);
    
    s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpMain,
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
    TAG_END);

    // s_pMainBuffer = simpleBufferCreate(0,
    // TAG_SIMPLEBUFFER_VPORT, s_pVpMain,
    // TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_INTERLEAVED,
    // TAG_SIMPLEBUFFER_IS_DBLBUF, 1, //add this line in for double buffering
    // TAG_END);

    paletteLoadFromPath("/data/palette/GamePalette.plt", s_pVpMain->pPalette, 32);

    pBmBackground = bitmapCreateFromPath("/data/GFX/BG1.bm",0);
    
    for(UWORD x = 0; x < s_pMainBuffer->uBfrBounds.uwX; x+=16){//fills out the background
    for(UWORD y = 0; y < s_pMainBuffer->uBfrBounds.uwY; y+=16){
      blitCopyAligned(pBmBackground,x,y,s_pMainBuffer->pBack,x,y,16,16);
        blitCopyAligned(pBmBackground,x,y,s_pMainBuffer->pFront,x,y,16,16);
        }
    }
    
    gFontSmall = fontCreateFromPath("/data/font/myacefont.fnt");
    
    tTextBitMap *testingbitmap = fontCreateTextBitMapFromStr(gFontSmall, "THIS IS A TEST");
    fontDrawTextBitMap(s_pMainBuffer->pBack, testingbitmap, 100, 100, 9,FONT_COOKIE);

    systemUnuse();
    // Load the view
    viewLoad(s_pView);
}

void gameGsLoop(void) {
    // This will loop every frame
  if(keyCheck(KEY_ESCAPE)) {
    gameExit();
  }
}

void gameGsDestroy(void) {
    systemUse();
    bitmapDestroy(pBmBackground);
    viewDestroy(s_pView);
}