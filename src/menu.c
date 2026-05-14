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
#include <ace/managers/mouse.h>
#include <ace/utils/file.h>
#include <ace/utils/font.h>
#include <ace/utils/string.h>
#include <ace/utils/palette.h>
#include <stdbool.h>
#include <stdlib.h>
#include <exec/execbase.h>
#include <proto/exec.h>

#define MENU_HEIGHT	256 //y
#define MENU_WIDTH	320 //x
#define CURSOR_SPRITE_CHANNEL 5
#define CURSOR_SPRITE_WIDTH 16
#define CURSOR_SPRITE_HEIGHT 18
#define BUTTON_WIDTH 64
#define BUTTON_HEIGHT 47
/*-----Seeting Up Viewports-----*/
static tView *s_pMenuView; //view for all viewports
static tVPort *s_pVpMain;//viewport for the menu
static tSimpleBufferManager *s_pMainBuffer;

/*-----GFX Setup-----*/
tFont *menufont;    //font
tTextBitMap *menutextbitmap; //bitmap for the font

static tBitMap *pBDefwin;
static tBitMap *pBmAckwin;
static tBitMap *menuBG;
static tBitMap *pBmMenuCursorSrc;
static tBitMap *pBmMenuCursorData;
static tBitMap *pBmTitleText;
static tBitMap *pBmTitleTextMask;
static tBitMap *pBmOptionButton;
static tBitMap *pBmOptionButtonH;
static tBitMap *pBmButtonPlay;
static tBitMap *pBMButtonPlayH;

static tSprite *pSMenuCursor;

BYTE cpuType = 0; //cpu types for configuring depth and adding addtional difficulty levels for CPUS that can.

void menuGsCreate(void){
    // //create view port and the display buffer for the main viewport
    s_pMenuView = viewCreate(0, TAG_VIEW_GLOBAL_PALETTE, 1, TAG_END);

    s_pVpMain = vPortCreate(0, 
    TAG_VPORT_VIEW, s_pMenuView, 
    TAG_VPORT_BPP, 5, TAG_END);

    s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpMain, 
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_INTERLEAVED, 
    BMF_CLEAR, TAG_END);

    //colour palette for the menu
    paletteLoadFromPath("data/palette/mainmenuPalette2.plt", s_pVpMain->pPalette, 32);
        
    drawBackground(); //draw the menu background to the back buffer before loading the view.
    
    //enanble the sprite manager and the DMA bit for sprites
    spriteManagerCreate(s_pMenuView, 0, NULL);
    systemSetDmaBit(DMAB_SPRITE, 1);

    //setup the sprite for the mouse cursor in the menu
    setupMouseCursor();

    //copy in the mouse cursor sprite data
    blitCopy(pBmMenuCursorSrc, 0,0,
    pBmMenuCursorData,0,0,CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,MINTERM_COOKIE);
    
    //load the font for the menu
    menufont = fontCreateFromPath("data/font/myacefont.fnt");
    menutextbitmap = fontCreateTextBitMapFromStr(menufont, "XXXXXXXXXXXXXXXXXXX");

    //Get the CPU type.
    findCPUType();
    setupTitleMenu();

    //fontDrawStr(menufont, s_pMainBuffer->pBack, 168, 155, "L", 10, FONT_COOKIE, menutextbitmap);

    systemUnuse();
    viewLoad(s_pMenuView);
}

void menuGsLoop(void){

    if(keyCheck(KEY_N)){
        logWrite("getting outta here!\n");
        gameExit();
    }
    //update the mouse cursor position.
    short mouseX = mouseGetX(MOUSE_PORT_1);
    short mouseY = mouseGetY(MOUSE_PORT_1);
    
    updateMenuMousepos(mouseX, mouseY);


    copProcessBlocks();
    systemIdleBegin();
    vPortWaitUntilEnd(s_pVpMain);
}

void menuGsDestroy(void){
    systemUse();
    bitmapDestroy(pBmAckwin);
    bitmapDestroy(pBDefwin);
    bitmapDestroy(pBmMenuCursorSrc);
    bitmapDestroy(pBmMenuCursorData);
    systemSetDmaBit(DMAB_SPRITE, 0);
    spriteManagerDestroy();
    fontDestroyTextBitMap(menutextbitmap);
    viewDestroy(s_pMenuView);
}

void updateMenuMousepos(short mouseX, short mouseY){
  pSMenuCursor->wX = mouseX;
  pSMenuCursor->wY = mouseY;
    
  spriteProcess(pSMenuCursor);
  spriteProcessChannel(CURSOR_SPRITE_CHANNEL);
}

void findCPUType(void){
    struct ExecBase *SysBase = *((struct ExecBase **)4); // ExecBase is always at address 4
    
    UWORD attnFlags = SysBase->AttnFlags;
    
    if(attnFlags & AFF_68060){
        cpuType = 4;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 5, 10, "060", 10, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68060\n");
    } 
    else if(attnFlags & AFF_68040){
        cpuType = 3;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 5, 10, "040", 10, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68040\n");
    } 
    else if(attnFlags & AFF_68030){
        cpuType = 2;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 5, 10, "030", 10, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68030\n");
    }
    else if(attnFlags & AFF_68020){
        cpuType = 1;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 5, 10, "020", 10, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68020\n");
    } 
    else {
        cpuType = 0;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 5, 10, "000", 10, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68000\n");
    }
}

void drawBackground(void){
    menuBG = bitmapCreateFromPath("data/GFX/menuBG.bm",0);
    for(UWORD x = 0; x < s_pMainBuffer->uBfrBounds.uwX; x+=16){//fills out the background
        for(UWORD y = 0; y < s_pMainBuffer->uBfrBounds.uwY; y+=16){
            blitCopyAligned(menuBG,x,y,s_pMainBuffer->pBack,x,y,16,16);
            blitCopyAligned(menuBG,x,y,s_pMainBuffer->pFront,x,y,16,16);
        }
    }
}

void setupMouseCursor(void){
    pBmMenuCursorSrc = bitmapCreateFromPath("data/GFX/menuPointer.bm",0);
    pBmMenuCursorData = bitmapCreate(CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,2,BMF_INTERLEAVED | BMF_CLEAR);
    pSMenuCursor = spriteAdd(CURSOR_SPRITE_CHANNEL,pBmMenuCursorData);

    spriteSetEnabled(pSMenuCursor, 1);
    
    blitCopy(pBmMenuCursorSrc, 0,0,
    pBmMenuCursorData,0,0,CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,MINTERM_COOKIE);
}

void setupTitleMenu(void){
    
    //load title text
    pBmTitleText = bitmapCreateFromPath("data/GFX/titletext.bm",0);
    pBmTitleTextMask = bitmapCreateFromPath("data/GFX/titletext_mask.bm",0);
    
    blitCopyMask(pBmTitleText,0,0,
    s_pMainBuffer->pBack, 50, 17, 224, 96, pBmTitleTextMask->Planes[0]);

    //load play Button
    pBmButtonPlay = bitmapCreateFromPath("data/GFX/buttonPlay.bm",0);
    blitCopy(pBmButtonPlay,0,0,
    s_pMainBuffer->pBack,50,200,BUTTON_WIDTH,BUTTON_HEIGHT,MINTERM_COOKIE);

    //load options button
    pBmOptionButton = bitmapCreateFromPath("data/GFX/buttonOptions.bm",0);
     blitCopy(pBmOptionButton,0,0,
    s_pMainBuffer->pBack,206,200,BUTTON_WIDTH,BUTTON_HEIGHT,MINTERM_COOKIE);

    //load highlighted versions for reference
    pBmOptionButtonH = bitmapCreateFromPath("data/GFX/buttonOptionsH.bm",0);
    pBMButtonPlayH = bitmapCreateFromPath("data/GFX/buttonPlayH.bm",0);

}