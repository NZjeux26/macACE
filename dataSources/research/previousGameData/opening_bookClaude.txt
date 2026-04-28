/* opening_book.h — generated 2026-04-27 21:40 */
/* Smoothed-weight opening book (K=16, SCALE=128) */
/* Size: ~5188 / 100000 bytes */

#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

#include "game.h"
#include "ai.h"
#include <ace/managers/rand.h>

#define BOOK_ATTACKER_D1_LINES               27
#define BOOK_DEFENDER_D1_LINES               31
#define BOOK_ATTACKER_D2_LINES               82
#define BOOK_DEFENDER_D2_LINES               104
#define BOOK_ATTACKER_D3_LINES               35
#define BOOK_DEFENDER_D3_LINES               51
#define BOOK_ATTACKER_D4_LINES               3
#define BOOK_DEFENDER_D4_LINES               13
#define BOOK_ATTACKER_D5_LINES               2
#define BOOK_DEFENDER_D5_LINES               2

/* Opening line structure */

typedef struct {
    AIMove moves[1];
    UBYTE  length;
    UWORD  weight;     /* selection weight */
    UBYTE  win_rate;   /* 0–100 (debug) */
    UWORD  frequency;  /* raw count */
} OpeningLine_D1;

typedef struct {
    AIMove moves[2];
    UBYTE  length;
    UWORD  weight;     /* selection weight */
    UBYTE  win_rate;   /* 0–100 (debug) */
    UWORD  frequency;  /* raw count */
} OpeningLine_D2;

typedef struct {
    AIMove moves[3];
    UBYTE  length;
    UWORD  weight;     /* selection weight */
    UBYTE  win_rate;   /* 0–100 (debug) */
    UWORD  frequency;  /* raw count */
} OpeningLine_D3;

typedef struct {
    AIMove moves[4];
    UBYTE  length;
    UWORD  weight;     /* selection weight */
    UBYTE  win_rate;   /* 0–100 (debug) */
    UWORD  frequency;  /* raw count */
} OpeningLine_D4;

typedef struct {
    AIMove moves[5];
    UBYTE  length;
    UWORD  weight;     /* selection weight */
    UBYTE  win_rate;   /* 0–100 (debug) */
    UWORD  frequency;  /* raw count */
} OpeningLine_D5;

/* ---- ATTACKER depth-1 ---- */
static const OpeningLine_D1 attacker_book_d1[BOOK_ATTACKER_D1_LINES] = {
    {
        /* moves     */ {
        { 151, 125, 0 }
    },
        /* length    */ 1,
        /* weight    */ 42739,
        /* win_rate  */ 57,
        /* frequency */ 587
    },
    {
        /* moves     */ {
        { 147, 121, 0 }
    },
        /* length    */ 1,
        /* weight    */ 19632,
        /* win_rate  */ 47,
        /* frequency */ 322
    },
    {
        /* moves     */ {
        {  53,  55, 0 }
    },
        /* length    */ 1,
        /* weight    */ 6270,
        /* win_rate  */ 57,
        /* frequency */ 87
    },
    {
        /* moves     */ {
        {  63,  61, 0 }
    },
        /* length    */ 1,
        /* weight    */ 5156,
        /* win_rate  */ 80,
        /* frequency */ 55
    },
    {
        /* moves     */ {
        { 150, 124, 0 }
    },
        /* length    */ 1,
        /* weight    */ 4747,
        /* win_rate  */ 49,
        /* frequency */ 75
    },
    {
        /* moves     */ {
        { 105, 107, 0 }
    },
        /* length    */ 1,
        /* weight    */ 3242,
        /* win_rate  */ 73,
        /* frequency */ 38
    },
    {
        /* moves     */ {
        {  17,  43, 0 }
    },
        /* length    */ 1,
        /* weight    */ 2524,
        /* win_rate  */ 61,
        /* frequency */ 34
    },
    {
        /* moves     */ {
        { 150, 137, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1836,
        /* win_rate  */ 46,
        /* frequency */ 30
    },
    {
        /* moves     */ {
        { 136, 133, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1816,
        /* win_rate  */ 80,
        /* frequency */ 21
    },
    {
        /* moves     */ {
        {  21,  47, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1547,
        /* win_rate  */ 39,
        /* frequency */ 28
    },
    {
        /* moves     */ {
        { 136, 139, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1404,
        /* win_rate  */ 40,
        /* frequency */ 25
    },
    {
        /* moves     */ {
        { 151, 112, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1389,
        /* win_rate  */ 63,
        /* frequency */ 19
    },
    {
        /* moves     */ {
        {  88,  36, 0 }
    },
        /* length    */ 1,
        /* weight    */ 822,
        /* win_rate  */ 58,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        { 136, 140, 0 }
    },
        /* length    */ 1,
        /* weight    */ 787,
        /* win_rate  */ 80,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 147, 146, 0 }
    },
        /* length    */ 1,
        /* weight    */ 783,
        /* win_rate  */ 100,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        { 115, 113, 0 }
    },
        /* length    */ 1,
        /* weight    */ 645,
        /* win_rate  */ 66,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        { 148, 122, 0 }
    },
        /* length    */ 1,
        /* weight    */ 597,
        /* win_rate  */ 75,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  63,  60, 0 }
    },
        /* length    */ 1,
        /* weight    */ 545,
        /* win_rate  */ 85,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  80,  28, 0 }
    },
        /* length    */ 1,
        /* weight    */ 512,
        /* win_rate  */ 50,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  80, 132, 0 }
    },
        /* length    */ 1,
        /* weight    */ 467,
        /* win_rate  */ 57,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  92,  94, 0 }
    },
        /* length    */ 1,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  17,  16, 0 }
    },
        /* length    */ 1,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 136, 123, 0 }
    },
        /* length    */ 1,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  32,  29, 0 }
    },
        /* length    */ 1,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  32,  35, 0 }
    },
        /* length    */ 1,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  53,  56, 0 }
    },
        /* length    */ 1,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 134, 0 }
    },
        /* length    */ 1,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
};

/* ---- DEFENDER depth-1 ---- */
static const OpeningLine_D1 defender_book_d1[BOOK_DEFENDER_D1_LINES] = {
    {
        /* moves     */ {
        { 110, 113, 0 }
    },
        /* length    */ 1,
        /* weight    */ 9456,
        /* win_rate  */ 43,
        /* frequency */ 166
    },
    {
        /* moves     */ {
        { 110, 107, 0 }
    },
        /* length    */ 1,
        /* weight    */ 8131,
        /* win_rate  */ 46,
        /* frequency */ 136
    },
    {
        /* moves     */ {
        {  82,  43, 0 }
    },
        /* length    */ 1,
        /* weight    */ 8078,
        /* win_rate  */ 49,
        /* frequency */ 128
    },
    {
        /* moves     */ {
        {  58,  55, 0 }
    },
        /* length    */ 1,
        /* weight    */ 7954,
        /* win_rate  */ 42,
        /* frequency */ 145
    },
    {
        /* moves     */ {
        {  86, 125, 0 }
    },
        /* length    */ 1,
        /* weight    */ 7883,
        /* win_rate  */ 45,
        /* frequency */ 133
    },
    {
        /* moves     */ {
        {  82, 121, 0 }
    },
        /* length    */ 1,
        /* weight    */ 7059,
        /* win_rate  */ 35,
        /* frequency */ 151
    },
    {
        /* moves     */ {
        {  86,  47, 0 }
    },
        /* length    */ 1,
        /* weight    */ 6727,
        /* win_rate  */ 46,
        /* frequency */ 113
    },
    {
        /* moves     */ {
        {  58,  61, 0 }
    },
        /* length    */ 1,
        /* weight    */ 6404,
        /* win_rate  */ 42,
        /* frequency */ 115
    },
    {
        /* moves     */ {
        {  98, 137, 0 }
    },
        /* length    */ 1,
        /* weight    */ 4542,
        /* win_rate  */ 32,
        /* frequency */ 103
    },
    {
        /* moves     */ {
        {  96, 135, 0 }
    },
        /* length    */ 1,
        /* weight    */ 2784,
        /* win_rate  */ 43,
        /* frequency */ 48
    },
    {
        /* moves     */ {
        {  70,  67, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1459,
        /* win_rate  */ 45,
        /* frequency */ 24
    },
    {
        /* moves     */ {
        {  96, 122, 0 }
    },
        /* length    */ 1,
        /* weight    */ 1152,
        /* win_rate  */ 62,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        {  98, 124, 0 }
    },
        /* length    */ 1,
        /* weight    */ 918,
        /* win_rate  */ 61,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        {  72,  33, 0 }
    },
        /* length    */ 1,
        /* weight    */ 877,
        /* win_rate  */ 66,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  70,  31, 0 }
    },
        /* length    */ 1,
        /* weight    */ 836,
        /* win_rate  */ 42,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        {  70,  44, 0 }
    },
        /* length    */ 1,
        /* weight    */ 822,
        /* win_rate  */ 58,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  72,  74, 0 }
    },
        /* length    */ 1,
        /* weight    */ 803,
        /* win_rate  */ 46,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        {  72,  75, 0 }
    },
        /* length    */ 1,
        /* weight    */ 768,
        /* win_rate  */ 25,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        {  96,  94, 0 }
    },
        /* length    */ 1,
        /* weight    */ 645,
        /* win_rate  */ 66,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  70,  68, 0 }
    },
        /* length    */ 1,
        /* weight    */ 640,
        /* win_rate  */ 50,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  72,  59, 0 }
    },
        /* length    */ 1,
        /* weight    */ 603,
        /* win_rate  */ 25,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  70,  57, 0 }
    },
        /* length    */ 1,
        /* weight    */ 603,
        /* win_rate  */ 25,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  98, 100, 0 }
    },
        /* length    */ 1,
        /* weight    */ 469,
        /* win_rate  */ 37,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  72,  46, 0 }
    },
        /* length    */ 1,
        /* weight    */ 426,
        /* win_rate  */ 25,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  98, 111, 0 }
    },
        /* length    */ 1,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  58,  54, 0 }
    },
        /* length    */ 1,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  86, 138, 0 }
    },
        /* length    */ 1,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 136, 0 }
    },
        /* length    */ 1,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  96,  93, 0 }
    },
        /* length    */ 1,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  98, 101, 0 }
    },
        /* length    */ 1,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  72,  73, 0 }
    },
        /* length    */ 1,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
};

/* ---- ATTACKER depth-2 ---- */
static const OpeningLine_D2 attacker_book_d2[BOOK_ATTACKER_D2_LINES] = {
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 }
    },
        /* length    */ 2,
        /* weight    */ 5130,
        /* win_rate  */ 49,
        /* frequency */ 81
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 3774,
        /* win_rate  */ 80,
        /* frequency */ 41
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 105, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 3267,
        /* win_rate  */ 71,
        /* frequency */ 39
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  17,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 3062,
        /* win_rate  */ 68,
        /* frequency */ 38
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1979,
        /* win_rate  */ 55,
        /* frequency */ 29
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  92,  94, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1933,
        /* win_rate  */ 31,
        /* frequency */ 41
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  21,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1848,
        /* win_rate  */ 55,
        /* frequency */ 27
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  53,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1768,
        /* win_rate  */ 51,
        /* frequency */ 27
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 150, 152, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1711,
        /* win_rate  */ 28,
        /* frequency */ 38
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 150, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1528,
        /* win_rate  */ 73,
        /* frequency */ 19
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 125, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1408,
        /* win_rate  */ 87,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  21,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1307,
        /* win_rate  */ 47,
        /* frequency */ 21
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 125, 138, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1300,
        /* win_rate  */ 86,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        { 150, 124, 0 },
        { 124, 120, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1280,
        /* win_rate  */ 50,
        /* frequency */ 20
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  32,  35, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1194,
        /* win_rate  */ 85,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 148, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1088,
        /* win_rate  */ 56,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  17,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1056,
        /* win_rate  */ 26,
        /* frequency */ 23
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 136, 139, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1052,
        /* win_rate  */ 60,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 148, 122, 0 }
    },
        /* length    */ 2,
        /* weight    */ 964,
        /* win_rate  */ 14,
        /* frequency */ 27
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  32,  29, 0 }
    },
        /* length    */ 2,
        /* weight    */ 896,
        /* win_rate  */ 50,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  80,  41, 0 }
    },
        /* length    */ 2,
        /* weight    */ 896,
        /* win_rate  */ 37,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 148, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 886,
        /* win_rate  */ 81,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        {  63,  61, 0 },
        {  17,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 836,
        /* win_rate  */ 90,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 150, 124, 0 },
        { 136, 123, 0 }
    },
        /* length    */ 2,
        /* weight    */ 832,
        /* win_rate  */ 31,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        { 147, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 782,
        /* win_rate  */ 63,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 115, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 768,
        /* win_rate  */ 50,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        { 115, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 730,
        /* win_rate  */ 54,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        {  21,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 730,
        /* win_rate  */ 54,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 102, 100, 0 }
    },
        /* length    */ 2,
        /* weight    */ 713,
        /* win_rate  */ 41,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  63,  61, 0 },
        { 151, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 691,
        /* win_rate  */ 77,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 132, 0 }
    },
        /* length    */ 2,
        /* weight    */ 682,
        /* win_rate  */ 100,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  88,  49, 0 }
    },
        /* length    */ 2,
        /* weight    */ 645,
        /* win_rate  */ 66,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        {  55,  54, 0 }
    },
        /* length    */ 2,
        /* weight    */ 640,
        /* win_rate  */ 50,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  53,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 640,
        /* win_rate  */ 87,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 150, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 631,
        /* win_rate  */ 23,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 150, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 631,
        /* win_rate  */ 23,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  32,  35, 0 }
    },
        /* length    */ 2,
        /* weight    */ 599,
        /* win_rate  */ 55,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 115, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 599,
        /* win_rate  */ 55,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  18,  31, 0 }
    },
        /* length    */ 2,
        /* weight    */ 597,
        /* win_rate  */ 75,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        { 151, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 590,
        /* win_rate  */ 40,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 105, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 554,
        /* win_rate  */ 62,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 139, 0 }
    },
        /* length    */ 2,
        /* weight    */ 554,
        /* win_rate  */ 62,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  63,  61, 0 },
        { 102, 101, 0 }
    },
        /* length    */ 2,
        /* weight    */ 545,
        /* win_rate  */ 85,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  17,  30, 0 }
    },
        /* length    */ 2,
        /* weight    */ 512,
        /* win_rate  */ 50,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  80, 119, 0 }
    },
        /* length    */ 2,
        /* weight    */ 506,
        /* win_rate  */ 71,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  88,  49, 0 }
    },
        /* length    */ 2,
        /* weight    */ 492,
        /* win_rate  */ 20,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  63,  61, 0 },
        { 105, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 488,
        /* win_rate  */ 100,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 102, 100, 0 }
    },
        /* length    */ 2,
        /* weight    */ 469,
        /* win_rate  */ 9,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        { 105, 107, 0 },
        { 151, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 453,
        /* win_rate  */ 83,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  53,  54, 0 }
    },
        /* length    */ 2,
        /* weight    */ 418,
        /* win_rate  */ 66,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 102, 101, 0 }
    },
        /* length    */ 2,
        /* weight    */ 418,
        /* win_rate  */ 66,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  17,  43, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 396,
        /* win_rate  */ 100,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 148, 122, 0 }
    },
        /* length    */ 2,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 349,
        /* win_rate  */ 33,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 151, 112, 0 },
        { 102, 100, 0 }
    },
        /* length    */ 2,
        /* weight    */ 349,
        /* win_rate  */ 33,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  88, 127, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 121, 134, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 136, 133, 0 },
        {  88, 127, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  92,  94, 0 }
    },
        /* length    */ 2,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 115, 112, 0 }
    },
        /* length    */ 2,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 150, 137, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 150, 124, 0 },
        { 151, 112, 0 }
    },
        /* length    */ 2,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  80,  28, 0 }
    },
        /* length    */ 2,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  63,  50, 0 }
    },
        /* length    */ 2,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 136, 133, 0 },
        {  32,  35, 0 }
    },
        /* length    */ 2,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 136, 139, 0 },
        {  80, 119, 0 }
    },
        /* length    */ 2,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  32,  29, 0 }
    },
        /* length    */ 2,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 151, 138, 0 }
    },
        /* length    */ 2,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 150, 124, 0 },
        { 105, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 105, 107, 0 },
        {  32,  35, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 121, 122, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  17,  43, 0 },
        { 105, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 148, 122, 0 },
        { 122, 126, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 136, 139, 0 },
        {  32,  29, 0 }
    },
        /* length    */ 2,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  20,  33, 0 }
    },
        /* length    */ 2,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 150, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  20,  46, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  17,  43, 0 },
        { 151, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  76,  75, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 147, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 136, 139, 0 },
        { 147, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 134, 0 },
        { 148, 122, 0 }
    },
        /* length    */ 2,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
};

/* ---- DEFENDER depth-2 ---- */
static const OpeningLine_D2 defender_book_d2[BOOK_DEFENDER_D2_LINES] = {
    {
        /* moves     */ {
        {  82, 121, 0 },
        { 110, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 2218,
        /* win_rate  */ 56,
        /* frequency */ 32
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1932,
        /* win_rate  */ 40,
        /* frequency */ 35
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1891,
        /* win_rate  */ 34,
        /* frequency */ 38
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1402,
        /* win_rate  */ 31,
        /* frequency */ 29
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1280,
        /* win_rate  */ 21,
        /* frequency */ 32
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1216,
        /* win_rate  */ 68,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  82,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1120,
        /* win_rate  */ 52,
        /* frequency */ 17
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  70,  68, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1114,
        /* win_rate  */ 66,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  72,  33, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1088,
        /* win_rate  */ 56,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1052,
        /* win_rate  */ 60,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  98, 101, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1042,
        /* win_rate  */ 36,
        /* frequency */ 19
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1024,
        /* win_rate  */ 50,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 1016,
        /* win_rate  */ 38,
        /* frequency */ 18
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  44, 0 }
    },
        /* length    */ 2,
        /* weight    */ 990,
        /* win_rate  */ 53,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  72,  75, 0 }
    },
        /* length    */ 2,
        /* weight    */ 990,
        /* win_rate  */ 53,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 960,
        /* win_rate  */ 43,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 948,
        /* win_rate  */ 33,
        /* frequency */ 18
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  58,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 948,
        /* win_rate  */ 33,
        /* frequency */ 18
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  72,  75, 0 }
    },
        /* length    */ 2,
        /* weight    */ 948,
        /* win_rate  */ 33,
        /* frequency */ 18
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  82, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 929,
        /* win_rate  */ 46,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        { 110, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 905,
        /* win_rate  */ 17,
        /* frequency */ 23
    },
    {
        /* moves     */ {
        {  70,  67, 0 },
        {  71,  67, 0 }
    },
        /* length    */ 2,
        /* weight    */ 896,
        /* win_rate  */ 50,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 853,
        /* win_rate  */ 20,
        /* frequency */ 20
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  82,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 836,
        /* win_rate  */ 42,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 803,
        /* win_rate  */ 46,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 803,
        /* win_rate  */ 46,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        { 110, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 803,
        /* win_rate  */ 46,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        {  96, 135, 0 },
        { 110, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 803,
        /* win_rate  */ 46,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  58,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 791,
        /* win_rate  */ 23,
        /* frequency */ 17
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 783,
        /* win_rate  */ 100,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  58,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 776,
        /* win_rate  */ 35,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  96,  93, 0 }
    },
        /* length    */ 2,
        /* weight    */ 768,
        /* win_rate  */ 25,
        /* frequency */ 16
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  72,  33, 0 }
    },
        /* length    */ 2,
        /* weight    */ 745,
        /* win_rate  */ 16,
        /* frequency */ 18
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        { 110, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 738,
        /* win_rate  */ 70,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        { 110, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 713,
        /* win_rate  */ 41,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  96, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 691,
        /* win_rate  */ 77,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        { 110, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 688,
        /* win_rate  */ 30,
        /* frequency */ 13
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  82,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 658,
        /* win_rate  */ 33,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 640,
        /* win_rate  */ 50,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  82, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 619,
        /* win_rate  */ 13,
        /* frequency */ 15
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  96, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 603,
        /* win_rate  */ 25,
        /* frequency */ 12
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        { 110, 107, 0 }
    },
        /* length    */ 2,
        /* weight    */ 599,
        /* win_rate  */ 55,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  96, 135, 0 },
        { 135, 132, 0 }
    },
        /* length    */ 2,
        /* weight    */ 599,
        /* win_rate  */ 55,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  85, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 597,
        /* win_rate  */ 14,
        /* frequency */ 14
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 573,
        /* win_rate  */ 27,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  58,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 554,
        /* win_rate  */ 62,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 554,
        /* win_rate  */ 62,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  85, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 521,
        /* win_rate  */ 18,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 512,
        /* win_rate  */ 50,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  70,  67, 0 }
    },
        /* length    */ 2,
        /* weight    */ 512,
        /* win_rate  */ 50,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 506,
        /* win_rate  */ 71,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 112, 0 }
    },
        /* length    */ 2,
        /* weight    */ 506,
        /* win_rate  */ 71,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 492,
        /* win_rate  */ 20,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  72,  74, 0 }
    },
        /* length    */ 2,
        /* weight    */ 492,
        /* win_rate  */ 20,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 469,
        /* win_rate  */ 9,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  70,  67, 0 }
    },
        /* length    */ 2,
        /* weight    */ 467,
        /* win_rate  */ 57,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 2,
        /* weight    */ 467,
        /* win_rate  */ 57,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 428,
        /* win_rate  */ 42,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  97, 123, 0 }
    },
        /* length    */ 2,
        /* weight    */ 428,
        /* win_rate  */ 42,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  58,  61, 0 }
    },
        /* length    */ 2,
        /* weight    */ 428,
        /* win_rate  */ 42,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  96, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 418,
        /* win_rate  */ 66,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  98, 100, 0 }
    },
        /* length    */ 2,
        /* weight    */ 418,
        /* win_rate  */ 66,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 2,
        /* weight    */ 414,
        /* win_rate  */ 11,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  98, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 396,
        /* win_rate  */ 100,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  97, 136, 0 }
    },
        /* length    */ 2,
        /* weight    */ 389,
        /* win_rate  */ 28,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  71,  32, 0 }
    },
        /* length    */ 2,
        /* weight    */ 389,
        /* win_rate  */ 28,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  82, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 389,
        /* win_rate  */ 28,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  96, 122, 0 }
    },
        /* length    */ 2,
        /* weight    */ 389,
        /* win_rate  */ 28,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  82,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  96, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  97, 100, 0 }
    },
        /* length    */ 2,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  96,  94, 0 }
    },
        /* length    */ 2,
        /* weight    */ 384,
        /* win_rate  */ 12,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  98, 124, 0 }
    },
        /* length    */ 2,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  82, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 365,
        /* win_rate  */ 80,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  70,  31, 0 },
        {  83,  31, 0 }
    },
        /* length    */ 2,
        /* weight    */ 365,
        /* win_rate  */ 80,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  72,  33, 0 },
        {  85,  33, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  97, 123, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        { 110, 113, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  72,  46, 0 }
    },
        /* length    */ 2,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  70,  57, 0 },
        {  58,  62, 0 }
    },
        /* length    */ 2,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  72,  46, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 101, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  85,  88, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  96, 135, 0 },
        {  83, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  96, 122, 0 },
        {  97,  94, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 136, 0 },
        {  97, 123, 0 }
    },
        /* length    */ 2,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  72,  59, 0 },
        {  58,  54, 0 }
    },
        /* length    */ 2,
        /* weight    */ 279,
        /* win_rate  */ 0,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  96, 135, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  71,  32, 0 }
    },
        /* length    */ 2,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  82,  43, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  82, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  72,  75, 0 },
        {  71,  75, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  67, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  82, 121, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  98, 124, 0 },
        {  97,  99, 0 }
    },
        /* length    */ 2,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  72,  75, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 2,
        /* weight    */ 243,
        /* win_rate  */ 0,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  72,  74, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  96,  94, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  96, 135, 0 }
    },
        /* length    */ 2,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  83,  80, 0 }
    },
        /* length    */ 2,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
};

/* ---- ATTACKER depth-3 ---- */
static const OpeningLine_D3 attacker_book_d3[BOOK_ATTACKER_D3_LINES] = {
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        { 150, 152, 0 }
    },
        /* length    */ 3,
        /* weight    */ 1259,
        /* win_rate  */ 40,
        /* frequency */ 22
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  63,  61, 0 },
        { 148, 122, 0 }
    },
        /* length    */ 3,
        /* weight    */ 554,
        /* win_rate  */ 62,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 105, 107, 0 },
        {  17,  43, 0 }
    },
        /* length    */ 3,
        /* weight    */ 545,
        /* win_rate  */ 85,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 150, 152, 0 },
        { 136, 133, 0 }
    },
        /* length    */ 3,
        /* weight    */ 443,
        /* win_rate  */ 10,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        {  88, 140, 0 }
    },
        /* length    */ 3,
        /* weight    */ 396,
        /* win_rate  */ 100,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  92,  94, 0 },
        {  18,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 125, 138, 0 },
        { 136, 133, 0 }
    },
        /* length    */ 3,
        /* weight    */ 365,
        /* win_rate  */ 80,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  63,  61, 0 },
        {  92,  94, 0 }
    },
        /* length    */ 3,
        /* weight    */ 365,
        /* win_rate  */ 80,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 3,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        { 150, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  63,  61, 0 },
        { 105, 107, 0 },
        {  21,  47, 0 }
    },
        /* length    */ 3,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 125, 124, 0 },
        { 124, 120, 0 }
    },
        /* length    */ 3,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  63,  61, 0 },
        { 136, 133, 0 }
    },
        /* length    */ 3,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 105, 107, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 3,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 148, 122, 0 },
        {  17,  30, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  80,  41, 0 },
        { 136, 133, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 150, 124, 0 },
        { 124, 122, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 125, 124, 0 },
        { 136, 123, 0 }
    },
        /* length    */ 3,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        {  18,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 136, 139, 0 },
        { 148, 146, 0 }
    },
        /* length    */ 3,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 115, 112, 0 },
        { 125, 124, 0 }
    },
        /* length    */ 3,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  92,  94, 0 },
        {  80,  41, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 150, 152, 0 },
        { 125, 126, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  21,  47, 0 },
        { 136, 133, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        {  55,  54, 0 },
        {  92,  94, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        { 102, 100, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  53,  55, 0 },
        {  55,  54, 0 },
        {  80, 132, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 105, 107, 0 },
        {  20,  46, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        {  80, 119, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  32,  35, 0 },
        {  88,  49, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        {  17,  43, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 150, 152, 0 },
        {  18,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        {  21,  47, 0 },
        { 105, 107, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 150, 124, 0 },
        { 136, 123, 0 },
        {  63,  61, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 147, 121, 0 },
        {  88,  49, 0 },
        {  32,  35, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
};

/* ---- DEFENDER depth-3 ---- */
static const OpeningLine_D3 defender_book_d3[BOOK_DEFENDER_D3_LINES] = {
    {
        /* moves     */ {
        {  82, 121, 0 },
        { 110, 113, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 3,
        /* weight    */ 730,
        /* win_rate  */ 54,
        /* frequency */ 11
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  72,  33, 0 },
        {  85,  33, 0 }
    },
        /* length    */ 3,
        /* weight    */ 597,
        /* win_rate  */ 75,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 137, 0 },
        { 137, 140, 0 }
    },
        /* length    */ 3,
        /* weight    */ 590,
        /* win_rate  */ 40,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 124, 0 },
        { 113, 126, 0 }
    },
        /* length    */ 3,
        /* weight    */ 552,
        /* win_rate  */ 44,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  58,  55, 0 },
        {  86,  47, 0 }
    },
        /* length    */ 3,
        /* weight    */ 541,
        /* win_rate  */ 30,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  70,  31, 0 },
        {  83,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 512,
        /* win_rate  */ 50,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 124, 0 },
        {  85, 124, 0 }
    },
        /* length    */ 3,
        /* weight    */ 469,
        /* win_rate  */ 37,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 124, 0 },
        {  97, 100, 0 }
    },
        /* length    */ 3,
        /* weight    */ 460,
        /* win_rate  */ 22,
        /* frequency */ 9
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  58,  61, 0 },
        {  72,  33, 0 }
    },
        /* length    */ 3,
        /* weight    */ 443,
        /* win_rate  */ 10,
        /* frequency */ 10
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  58,  55, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 426,
        /* win_rate  */ 25,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  86,  47, 0 },
        {  72,  75, 0 }
    },
        /* length    */ 3,
        /* weight    */ 426,
        /* win_rate  */ 25,
        /* frequency */ 8
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  58,  61, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        { 110, 113, 0 },
        {  96,  93, 0 }
    },
        /* length    */ 3,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  83,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        { 110, 113, 0 },
        {  72,  33, 0 }
    },
        /* length    */ 3,
        /* weight    */ 384,
        /* win_rate  */ 50,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  72,  75, 0 },
        {  71,  75, 0 }
    },
        /* length    */ 3,
        /* weight    */ 365,
        /* win_rate  */ 80,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  71,  32, 0 },
        {  84,  45, 0 }
    },
        /* length    */ 3,
        /* weight    */ 349,
        /* win_rate  */ 33,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  58,  55, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 349,
        /* win_rate  */ 33,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  70,  67, 0 },
        {  71,  67, 0 }
    },
        /* length    */ 3,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        { 110, 107, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 335,
        /* win_rate  */ 60,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  83,  44, 0 }
    },
        /* length    */ 3,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 107, 0 },
        {  82,  43, 0 }
    },
        /* length    */ 3,
        /* weight    */ 314,
        /* win_rate  */ 16,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        { 110, 113, 0 },
        { 137, 140, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  82,  43, 0 },
        {  58,  61, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  71,  67, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  97, 123, 0 },
        {  84, 110, 0 }
    },
        /* length    */ 3,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  70,  31, 0 },
        {  83,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  70,  31, 0 },
        {  31,  28, 0 }
    },
        /* length    */ 3,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  96, 135, 0 },
        { 110, 107, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 3,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  31,  28, 0 }
    },
        /* length    */ 3,
        /* weight    */ 279,
        /* win_rate  */ 0,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  86,  47, 0 },
        {  58,  55, 0 }
    },
        /* length    */ 3,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 137, 0 },
        {  85, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 107, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  82, 121, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  70,  68, 0 },
        {  83,  31, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 113, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  86,  47, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  70,  67, 0 },
        {  67,  28, 0 }
    },
        /* length    */ 3,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  72,  74, 0 },
        {  47,  48, 0 }
    },
        /* length    */ 3,
        /* weight    */ 243,
        /* win_rate  */ 0,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 137, 0 },
        {  86, 125, 0 }
    },
        /* length    */ 3,
        /* weight    */ 243,
        /* win_rate  */ 0,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        { 110, 107, 0 },
        {  98, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  72,  75, 0 },
        {  75,  36, 0 }
    },
        /* length    */ 3,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  96,  93, 0 },
        {  83, 122, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  82, 121, 0 },
        {  70,  67, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  98, 137, 0 },
        {  86, 125, 0 },
        {  85, 137, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        {  98, 101, 0 },
        { 101, 140, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  61, 0 },
        {  71,  32, 0 },
        {  84,  45, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  96,  94, 0 },
        {  97,  94, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        { 110, 107, 0 },
        {  70,  67, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  83,  80, 0 },
        {  84,  81, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  58,  61, 0 },
        { 110, 107, 0 }
    },
        /* length    */ 3,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
};

/* ---- ATTACKER depth-4 ---- */
static const OpeningLine_D4 attacker_book_d4[BOOK_ATTACKER_D4_LINES] = {
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 148, 122, 0 },
        {  17,  30, 0 },
        {  53,  55, 0 }
    },
        /* length    */ 4,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        { 150, 152, 0 },
        {  80,  41, 0 }
    },
        /* length    */ 4,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        { 150, 152, 0 },
        {  66,  68, 0 }
    },
        /* length    */ 4,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
};

/* ---- DEFENDER depth-4 ---- */
static const OpeningLine_D4 defender_book_d4[BOOK_DEFENDER_D4_LINES] = {
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  72,  33, 0 },
        {  85,  33, 0 },
        {  70,  31, 0 }
    },
        /* length    */ 4,
        /* weight    */ 506,
        /* win_rate  */ 71,
        /* frequency */ 7
    },
    {
        /* moves     */ {
        { 110, 107, 0 },
        {  70,  31, 0 },
        {  83,  31, 0 },
        {  72,  33, 0 }
    },
        /* length    */ 4,
        /* weight    */ 418,
        /* win_rate  */ 66,
        /* frequency */ 6
    },
    {
        /* moves     */ {
        {  82,  43, 0 },
        {  72,  75, 0 },
        {  71,  75, 0 },
        {  98, 101, 0 }
    },
        /* length    */ 4,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  58,  55, 0 },
        {  98, 137, 0 },
        { 137, 140, 0 }
    },
        /* length    */ 4,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  83,  31, 0 },
        {  55,  29, 0 }
    },
        /* length    */ 4,
        /* weight    */ 304,
        /* win_rate  */ 40,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        { 110, 113, 0 },
        {  96,  93, 0 },
        {  93, 132, 0 }
    },
        /* length    */ 4,
        /* weight    */ 281,
        /* win_rate  */ 75,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 137, 0 },
        { 137, 140, 0 },
        {  97, 101, 0 }
    },
        /* length    */ 4,
        /* weight    */ 274,
        /* win_rate  */ 20,
        /* frequency */ 5
    },
    {
        /* moves     */ {
        {  86,  47, 0 },
        {  70,  67, 0 },
        {  71,  67, 0 },
        {  96,  93, 0 }
    },
        /* length    */ 4,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 137, 0 },
        { 137, 140, 0 },
        {  85, 124, 0 }
    },
        /* length    */ 4,
        /* weight    */ 256,
        /* win_rate  */ 50,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  98, 124, 0 },
        {  85, 124, 0 },
        {  96, 122, 0 }
    },
        /* length    */ 4,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  82, 121, 0 },
        {  96,  93, 0 },
        {  83, 122, 0 },
        {  84,  81, 0 }
    },
        /* length    */ 4,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  31,  28, 0 },
        {  71,  67, 0 }
    },
        /* length    */ 4,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  86, 125, 0 },
        { 110, 107, 0 },
        {  82,  43, 0 },
        {  72,  75, 0 }
    },
        /* length    */ 4,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
};

/* ---- ATTACKER depth-5 ---- */
static const OpeningLine_D5 attacker_book_d5[BOOK_ATTACKER_D5_LINES] = {
    {
        /* moves     */ {
        { 147, 121, 0 },
        { 148, 122, 0 },
        {  17,  30, 0 },
        {  53,  55, 0 },
        {  21,  34, 0 }
    },
        /* length    */ 5,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        { 151, 125, 0 },
        { 136, 133, 0 },
        { 150, 152, 0 },
        {  66,  68, 0 },
        { 147, 134, 0 }
    },
        /* length    */ 5,
        /* weight    */ 230,
        /* win_rate  */ 25,
        /* frequency */ 4
    },
};

/* ---- DEFENDER depth-5 ---- */
static const OpeningLine_D5 defender_book_d5[BOOK_DEFENDER_D5_LINES] = {
    {
        /* moves     */ {
        { 110, 113, 0 },
        {  72,  33, 0 },
        {  85,  33, 0 },
        {  70,  31, 0 },
        {  33,  46, 0 }
    },
        /* length    */ 5,
        /* weight    */ 307,
        /* win_rate  */ 100,
        /* frequency */ 4
    },
    {
        /* moves     */ {
        {  58,  55, 0 },
        {  70,  31, 0 },
        {  31,  28, 0 },
        {  71,  67, 0 },
        {  84,  45, 0 }
    },
        /* length    */ 5,
        /* weight    */ 204,
        /* win_rate  */ 0,
        /* frequency */ 4
    },
};

#endif /* OPENING_BOOK_H */
