#ifndef _GAME_H_
#define _GAME_H_

#include <ace/utils/file.h>

void gameGsCreate(void);
void gameGsLoop(void);
void gameGsDestroy(void);

typedef enum 
{
    DEFENDER,
    ATTACKER,
    BESERKER,
    KNIGHT,
    KING
} PieceType;
typedef enum{
    TEAM_ATTACKER,
    TEAM_DEFENDER
} Team;
typedef struct g_piece{
    int row;
    int col;
    PieceType type;  // KING, DEFENDER, ATTACKER
    Team team;       // 0 = Defneder LIGHT, Anything else = Attacker DARK
    UBYTE captured;  //0 not captured, anything else = captured.
} g_piece;


#endif // _GAME_H_