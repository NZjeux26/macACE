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
#define OPTION_BUTTON_WIDTH 64
#define OPTION_BUTTON_HEIGHT 22
#define OPTION_MENU_WIDTH 128
#define OPTION_MENU_HEIGHT 126
#define TEAM_ATTACKER 2
#define TEAM_DEFENDER 1
#define MAIN_MENU 0
#define OPTION_MENU 1

/*-----Seeting Up Viewports-----*/
static tView *s_pMenuView; //view for all viewports
static tVPort *s_pVpMain;//viewport for the menu
static tSimpleBufferManager *s_pMainBuffer;

/*-----GFX Setup-----*/
tFont *menufont;    //font
tTextBitMap *menutextbitmap; //bitmap for the font

// static tBitMap *pBDefwin;
// static tBitMap *pBmAckwin;
static tBitMap *menuBG;
static tBitMap *pBmMenuCursorSrc;
static tBitMap *pBmMenuCursorData;
static tBitMap *pBmTitleText;
static tBitMap *pBmTitleTextMask;
static tBitMap *pBmButtonOption;
static tBitMap *pBmButtonOptionMask;
static tBitMap *pBmButtonOptionH;
static tBitMap *pBmButtonPlay;
static tBitMap *pBmButtonPlayMask;
static tBitMap *pBMButtonPlayH;
static tBitMap *pBmButtonQuit;
static tBitMap *pBmButtonQuitMask;
static tBitMap *pBmOptionMenu;
static tBitMap *pBmOptionMenuMask;
static tBitMap *pBmbuttonAttacker;
static tBitMap *pBmbuttonAttackerMask;
static tBitMap *pBmbuttonDefender;
static tBitMap *pBmbuttonDefenderMask;

static tSprite *pSMenuCursor;

BYTE cpuType = 0; //cpu types for configuring depth and adding addtional difficulty levels for CPUS that can.
UBYTE PlayerTeam = TEAM_DEFENDER; 
UBYTE maxDepth = 1; //this will be used to set the depth of the minimax algorithm for the AI, based on the difficulty level selected in the options menu 1 = Novice, 2 = Apprentice, 3 = Warrior, 4 = Master
BYTE activeMenu = MAIN_MENU; //0 = main menu, 1 = options menu, this will be used to determine which menu to draw and which buttons to check for clicks on.
BOOL prevLMB = FALSE;

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
    
    loadMenuAssets(); //load the menu assets like the background and buttons

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
    drawMainMenu();
    

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
    BOOL currentLMB = mouseCheck(MOUSE_PORT_1, MOUSE_LMB);
    if(currentLMB && !prevLMB){ //if the left mouse button was just pressed this frame, check for clicks
        onMenuClick(mouseX, mouseY);
        return;
    }
    prevLMB = currentLMB; //update the previous left mouse button state for the next frame

    //Maybe invert this, check if you're in the option menu before checking clicks.
    //Maybe Replace this with a retuen button.
     if(activeMenu == OPTION_MENU){ 
        if(mouseCheck(MOUSE_PORT_1, MOUSE_RMB)){
        logWrite("Right click at %d, %d\n", mouseX, mouseY);
            drawBackground(); //redraw the background to clear the old highlighted button
            drawMainMenu(); //redraw the main menu to clear the old highlighted button
        }
    }
    
    copProcessBlocks();
    systemIdleBegin();
    vPortWaitUntilEnd(s_pVpMain);
}

void menuGsDestroy(void){
    systemUse();
    bitmapDestroy(menuBG);
    bitmapDestroy(pBmTitleText);
    bitmapDestroy(pBmTitleTextMask);
    bitmapDestroy(pBmButtonOption);
    bitmapDestroy(pBmButtonOptionMask);
    bitmapDestroy(pBmButtonOptionH);
    bitmapDestroy(pBmButtonPlay);
    bitmapDestroy(pBmButtonPlayMask);
    bitmapDestroy(pBMButtonPlayH);
    bitmapDestroy(pBmOptionMenu);
    bitmapDestroy(pBmOptionMenuMask);
    bitmapDestroy(pBmbuttonAttacker);
    bitmapDestroy(pBmbuttonAttackerMask);
    bitmapDestroy(pBmbuttonDefender);
    bitmapDestroy(pBmbuttonDefenderMask);
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
        fontDrawStr(menufont, s_pMainBuffer->pBack, 6, 10, "060", 1, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68060\n");
    } 
    else if(attnFlags & AFF_68040){
        cpuType = 3;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 6, 10, "040", 1, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68040\n");
    } 
    else if(attnFlags & AFF_68030){
        cpuType = 2;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 6, 10, "030", 1, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68030\n");
    }
    else if(attnFlags & AFF_68020){
        cpuType = 1;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 6, 10, "020", 1, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68020\n");
    } 
    else {
        cpuType = 0;
        fontDrawStr(menufont, s_pMainBuffer->pBack, 6, 10, "000", 1, FONT_COOKIE, menutextbitmap);
        logWrite("CPU: 68000\n");
    }
}
void loadMenuAssets(void){
    //load menu background
    menuBG = bitmapCreateFromPath("data/GFX/menuBG.bm",0);

     //load title text
    pBmTitleText = bitmapCreateFromPath("data/GFX/titletext.bm",0);
    pBmTitleTextMask = bitmapCreateFromPath("data/GFX/titletext_mask.bm",0);

    //load play Button
    pBmButtonPlay = bitmapCreateFromPath("data/GFX/buttonPlay.bm",0);
    pBmButtonPlayMask = bitmapCreateFromPath("data/GFX/buttonPlay_mask.bm",0);

    //load MMoption button
    pBmButtonOption = bitmapCreateFromPath("data/GFX/buttonOptions.bm",0);
    pBmButtonOptionMask = bitmapCreateFromPath("data/GFX/buttonOptions_mask.bm",0);

    pBmButtonQuit = bitmapCreateFromPath("data/GFX/buttonQuit.bm",0);
    pBmButtonQuitMask = bitmapCreateFromPath("data/GFX/buttonQuit_mask.bm",0);

    pBmOptionMenu = bitmapCreateFromPath("data/GFX/optionMenu.bm",0);
    pBmOptionMenuMask = bitmapCreateFromPath("data/GFX/optionMenu_mask.bm",0);

    pBmbuttonAttacker = bitmapCreateFromPath("data/GFX/buttonAttacker.bm",0);
    pBmbuttonAttackerMask = bitmapCreateFromPath("data/GFX/buttonAttacker_mask.bm",0);  

    pBmbuttonDefender = bitmapCreateFromPath("data/GFX/buttonDefender.bm",0);
    pBmbuttonDefenderMask = bitmapCreateFromPath("data/GFX/buttonDefender_mask.bm",0);

     //load highlighted versions for reference
    pBmButtonOptionH = bitmapCreateFromPath("data/GFX/buttonOptionsH.bm",0);
   // pBmButtonPlayH = bitmapCreateFromPath("data/GFX/buttonPlayH.bm",0);

}

void setupMouseCursor(void){
    pBmMenuCursorSrc = bitmapCreateFromPath("data/GFX/menuPointer.bm",0);
    pBmMenuCursorData = bitmapCreate(CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,2,BMF_INTERLEAVED | BMF_CLEAR);
    pSMenuCursor = spriteAdd(CURSOR_SPRITE_CHANNEL,pBmMenuCursorData);

    spriteSetEnabled(pSMenuCursor, 1);
    
    blitCopy(pBmMenuCursorSrc, 0,0,
    pBmMenuCursorData,0,0,CURSOR_SPRITE_WIDTH,CURSOR_SPRITE_HEIGHT,MINTERM_COOKIE);
}

void drawBackground(void){
    for(UWORD x = 0; x < s_pMainBuffer->uBfrBounds.uwX; x+=16){//fills out the background
        for(UWORD y = 0; y < s_pMainBuffer->uBfrBounds.uwY; y+=16){
            blitCopyAligned(menuBG,x,y,s_pMainBuffer->pBack,x,y,16,16);
            blitCopyAligned(menuBG,x,y,s_pMainBuffer->pFront,x,y,16,16);
        }
    }
}

void drawMainMenu(void){
    activeMenu = MAIN_MENU; //set the active menu to main menu so we know which buttons to check for clicks on and which menu to draw in the loop.
    //Draw Title Text
    blitCopyMask(pBmTitleText,0,0,
    s_pMainBuffer->pBack, 50, 17, 224, 96, pBmTitleTextMask->Planes[0]);
 
    //load play button
    blitCopyMask(pBmButtonPlay,0,0,
    s_pMainBuffer->pBack,50,200,BUTTON_WIDTH,BUTTON_HEIGHT,pBmButtonPlayMask->Planes[0]);

    //draw option button
    blitCopyMask(pBmButtonOption,0,0,
    s_pMainBuffer->pBack,206,200,BUTTON_WIDTH,BUTTON_HEIGHT,pBmButtonOptionMask->Planes[0]);
    
    //draw the quit text
    blitCopyMask(pBmButtonQuit,0,0,
    s_pMainBuffer->pBack,2,18,32,14,pBmButtonQuitMask->Planes[0]);

}

void drawOptionMenu(void){
    activeMenu = OPTION_MENU; //set the active menu to option menu so we know which buttons to check for clicks on and which menu to draw in the loop.
    
    //draw option menu
    blitCopyMask(pBmOptionMenu,0,0,
    s_pMainBuffer->pBack,94,65,OPTION_MENU_WIDTH,OPTION_MENU_HEIGHT,pBmOptionMenuMask->Planes[0]);  

    //draw drop shadow for team button
    blitRect(s_pMainBuffer->pBack, 128, 112, OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT, 0); //black shadow
    
    //This needs code to find the current player team and then draw the correct button for it, and then if thhe button is click swap both the button and the team.
    if(PlayerTeam == TEAM_ATTACKER){
        //draw the button for attacker team selected
        blitCopyMask(pBmbuttonAttacker,0,0,
        s_pMainBuffer->pBack,129,111,OPTION_BUTTON_WIDTH,OPTION_BUTTON_HEIGHT,pBmbuttonAttackerMask->Planes[0]);
        
    } else {
        //draw the button for defender team selected
        blitCopyMask(pBmbuttonDefender,0,0,
        s_pMainBuffer->pBack,129,111,OPTION_BUTTON_WIDTH,OPTION_BUTTON_HEIGHT,pBmbuttonDefenderMask->Planes[0]);
    }

    //Draw the A.I difficultly button Here.

}

void onMenuClick(short mouseX, short mouseY){
    
    if(activeMenu == 0){ //if we're in the main menu, check for clicks on the play and options buttons
        //check if the click is within the bounds of the play button
        if(mouseX >= 50 && mouseX <= 50 + BUTTON_WIDTH && mouseY >= 200 && mouseY <= 200 + BUTTON_HEIGHT){
            logWrite("Play button clicked!\n");
            stateChange(g_pStateManager, g_pGameState);
            return;//If statechange return immediately.
        }
        //check if the click is within the bounds of the option button
        else if(mouseX >= 206 && mouseX <= 206 + BUTTON_WIDTH && mouseY >= 200 && mouseY <= 200 + BUTTON_HEIGHT){
            logWrite("Option button clicked!\n");
            drawBackground();
            //unDrawMainMenu();
            drawOptionMenu();
        }
        else if(mouseX >= 7 && mouseX <= 7 + 32 && mouseY >= 18 && mouseY <= 18 + 14){
            gameExit();
        }
    }       
    else if(activeMenu == 1){ //if we're in the options menu, check for clicks on the team select button
        //check if the click is within the bounds of the team select button
        if(mouseX >= 129 && mouseX <= 129 + OPTION_BUTTON_WIDTH && mouseY >= 111 && mouseY <= 111 + OPTION_BUTTON_HEIGHT){
            logWrite("Team select button clicked!\n");
            //swap the team that the player is on
            if(PlayerTeam == TEAM_ATTACKER){
                PlayerTeam = TEAM_DEFENDER;
            } else {
                PlayerTeam = TEAM_ATTACKER;
            }
            drawOptionMenu(); //redraw the option menu to update the button
        }
    }
}