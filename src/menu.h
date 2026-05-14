#ifndef _MENU_H_
#define _MENU_H_

#include <ace/managers/state.h>

void menuGsCreate(void);
void menuGsLoop(void);
void menuGsDestroy(void);
void updateMenuMousepos(short mouseX, short mouseY);
void onMenuClick(short mouseX, short mouseY);
void findCPUType(void);
void drawBackground(void);
void loadAssets(void);
void setupMouseCursor(void);
void setupTitleMenu(void);
#endif