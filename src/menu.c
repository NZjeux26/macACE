#include "menu.h"
#include "input.h"
#include "states.h"
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/blit.h> // Blitting fns
#include <ace/managers/state.h>
#include <ace/managers/sprite.h>
#include <ace/utils/file.h>
#include <ace/utils/font.h>
#include <ace/utils/string.h>
#include <ace/utils/palette.h>
#include <stdbool.h>
#include <stdlib.h>

#define MENU_HEIGHT	256 //y
#define MENU_WIDTH	320 //x

/*-----Seeting Up Viewports-----*/
static tView *s_pMenuView; //view for all viewports
static tVPort *s_pVpMain;//viewport for the menu
static tSimpleBufferManager *s_pMainBuffer;

/*-----GFX Setup-----*/
tFont *menufont;    //font
tTextBitMap *menutextbitmap; //bitmap for the font
tBitMap *pBDefwin;
tBitMap *pBmAckwin;
tBitMap *menuBG;

void menuGsCreate(void){
    // //create view port and the display buffer for the main viewport
    s_pMenuView = viewCreate(0, TAG_VIEW_GLOBAL_PALETTE, 1, TAG_END);

    s_pVpMain = vPortCreate(0, TAG_VPORT_VIEW, s_pMenuView, TAG_VPORT_BPP, 5, TAG_END);
    s_pMainBuffer = simpleBufferCreate(0,TAG_SIMPLEBUFFER_VPORT, s_pVpMain, TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_INTERLEAVED, BMF_CLEAR, TAG_END);
    
    //colour palette for the menu
    paletteLoadFromPath("data/palette/mainmenuPalette2.plt", s_pVpMain->pPalette, 32);
    
    //pBDefwin = bitmapCreateFromPath("data/GFX/defwin.bm",0);
    //pBmAckwin = bitmapCreateFromPath("data/GFX/ackwin.bm",0);
    menuBG = bitmapCreateFromPath("data/GFX/menubg.bm", 0);
    
    menufont = fontCreateFromPath("data/font/menufont8.fnt");
    menutextbitmap = fontCreateTextBitMapFromStr(menufont, "XXXXXXXXXXXXXXXXXXX");
    
    systemUnuse();
    viewLoad(s_pMenuView);
}

void menuGsLoop(void){


    copProcessBlocks();
    systemIdleBegin();
    vPortWaitUntilEnd(s_pVpMain);
}

void menuGsDestroy(void){
    systemUse();
    bitmapDestroy(pBmAckwin);
    bitmapDestroy(pBDefwin);
    fontDestroyTextBitMap(menutextbitmap);
    viewDestroy(s_pMenuView);
}