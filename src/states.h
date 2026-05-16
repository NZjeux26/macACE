#ifndef _STATES_H_
#define _STATES_H_

#include <ace/managers/state.h>

extern tStateManager *g_pStateManager;
extern tState *g_pGameState;
extern tState *g_pMenuState;
extern UBYTE gameWinner;
extern BYTE cpuType;
extern UBYTE CPUPlayerTeam; //this will be used to track which team the AI player is on in the options menu
#endif