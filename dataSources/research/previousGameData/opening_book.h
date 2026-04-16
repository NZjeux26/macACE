/* opening_book.h  —  generated 2026-04-16 04:54 */
/* Top 20 opening sequences, first 10 moves each side. */
/* Move tokens are at most 7 chars + null terminator. */

#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

#define BOOK_PLIES      10
#define BOOK_MOVE_LEN   8
#define ATTACKER_LINES  20
#define DEFENDER_LINES  20

typedef struct {
    char     moves[BOOK_PLIES][BOOK_MOVE_LEN]; /* move tokens, empty string = not played */
    unsigned char  length;                     /* how many moves are actually set        */
    unsigned short frequency;                  /* times seen in training data            */
} OpeningLine;

/* ---- ATTACKER opening book ---- */
static const OpeningLine attacker_book[ATTACKER_LINES] = {
    {
        /* moves */ {
        "k8-i8",
        "a4-c4",
        "h11-h9",
        "e11-e10",
        "g11-i11",
        "j6-j9xi9",
        "k7-k9",
        "f2-j2",
        "h1-i1",
        "g1-h1"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "a5-c5",
        "d11-d6",
        "b6-c6",
        "f10-b10",
        "k8-f8",
        "h11-j11",
        "j6-j9xj10",
        "k7-i7",
        "e11-c11"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "g1-g3",
        "j6-j9",
        "e11-e10",
        "a8-b8xb7",
        "a7-a9",
        "e10-b10",
        "d11-c11",
        "f10-i10",
        "d1-b1"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "k8-i8",
        "j6-j2",
        "d11-c11",
        "f10-j10",
        "e1-e3",
        "b6-b8",
        "f2-c2",
        "b8-b9",
        "a8-a9",
        ""
    },
        /* length */ 9,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f2-c2",
        "h11-h10xg10",
        "e11-e9",
        "k4-j4xj5",
        "j6-i6",
        "a8-i8xj8",
        "i6-i3",
        "i8-i7xj7",
        "e9-g9xg10",
        "f10-c10"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "f2-c2",
        "k8-i8",
        "h11-h9",
        "f10-f9xg9",
        "d11-d9xe9",
        "a8-c8",
        "b6-b3",
        "k4-j4xj5",
        "g1-i1"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "k8-i8",
        "h1-h3",
        "a7-b7",
        "a8-c8",
        "f2-c2",
        "a4-b4xb5",
        "c2-a2",
        "a2-b2",
        "b2-a2",
        "d1-b1"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "f2-c2",
        "k8-i8",
        "a7-c7",
        "h11-h9",
        "a4-b4xb5",
        "a8-d8",
        "a5-a3",
        "k4-j4xj5",
        "c2-b2"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "k8-i8",
        "a8-c8",
        "g1-g2",
        "b6-b2",
        "a4-a3",
        "a7-b7",
        "d1-d2xe2",
        "d2-b2",
        "b2-b1",
        "a3-a2"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "f2-c2",
        "h3-f3xf2",
        "k4-i4",
        "g1-i1",
        "i4-j4xj5",
        "k8-i8",
        "f3-i3",
        "h11-h9",
        "f10-j10"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "j6-j9",
        "f10-i10",
        "a5-c5",
        "a8-b8xb7",
        "d11-d10",
        "d10-c10",
        "e11-c11",
        "c10-c9",
        "b8-b10"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "a8-b8",
        "g11-g9",
        "d11-d8xc8",
        "d8-d9",
        "a4-b4",
        "b4-b2",
        "d9-c9",
        "b2-b1",
        "b1-b2"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "a5-c5",
        "j6-j9",
        "f10-c10",
        "k4-i4",
        "k8-i8",
        "f2-j2",
        "k7-k9",
        "i4-i3",
        "g1-i1"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "k8-i8",
        "e1-e3",
        "d11-d10xe10",
        "a8-c8",
        "h11-h10",
        "j6-j9",
        "k7-j7",
        "f2-i2",
        "h1-h2"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "h11-h9",
        "k4-i4",
        "k7-j7",
        "f2-h2",
        "f1-f2xg2",
        "k8-i8",
        "j6-j3",
        "j7-j4",
        "d3-i3"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "e1-e3",
        "b6-b9",
        "k8-j8xj7",
        "h11-h9",
        "k4-i4",
        "k6-i6xi5",
        "h1-h3",
        "g1-j1",
        "j1-j4"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "h11-h9",
        "k4-i4",
        "k7-i7",
        "f2-h2",
        "f1-f2xg2",
        "k5-i5",
        "f10-i10",
        "k8-j8",
        "j8-k8"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "e1-e3",
        "k5-i5",
        "d11-d10xe10",
        "a8-c8",
        "h11-h10",
        "i5-i9",
        "b6-b9",
        "f2-i2",
        "j6-j3"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d1-d3",
        "h11-h9",
        "k8-i8",
        "g1-g2",
        "a4-c4",
        "d3-c3",
        "f10-c10",
        "e1-c1xc2",
        "a8-a9",
        "d11-d10"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h1-h3",
        "a8-c8",
        "h11-h9",
        "e1-e2",
        "e11-e9",
        "f10-i10",
        "k8-k10xj10",
        "g11-g10",
        "h3-h2xg2",
        "i10-i7xj7"
    },
        /* length */ 10,
        /* frequency */ 1
    },
};

/* ---- DEFENDER opening book ---- */
static const OpeningLine defender_book[DEFENDER_LINES] = {
    {
        /* moves */ {
        "g5-i5",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    },
        /* length */ 1,
        /* frequency */ 2
    },
    {
        /* moves */ {
        "d6-d9",
        "g7-i7",
        "g6-g10",
        "g10-i10",
        "i10-i9xi8",
        "i7-i9",
        "h6-h3",
        "g5-g9xh9",
        "h3-k3",
        "f6-i6"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d6-d3",
        "f4-c4",
        "e7-c7",
        "f8-c8",
        "f7-f10",
        "f10-j10",
        "g7-g9",
        "f6-f7",
        "f7-d7",
        "g9-a9"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f4-i4",
        "h6-h9",
        "d6-d9",
        "e7-b7",
        "f7-b7",
        "d9-b9xb8",
        "e5-b5xb6",
        "e6-e10",
        "b5-b2",
        "b7-b3"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f4-i4",
        "d6-d9",
        "e7-e10",
        "e10-b10",
        "e6-e9",
        "f6-c6",
        "c6-c10",
        "f8-b8xb9",
        "b10-b9",
        ""
    },
        /* length */ 9,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "g7-g10",
        "g6-g10",
        "g5-j5",
        "h6-h2",
        "f8-j8",
        "f6-h6",
        "f7-j7",
        "h6-h8",
        "h8-c8",
        "c8-i8"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d6-d3",
        "g7-i7",
        "g6-g9",
        "e7-c7",
        "e6-e9",
        "f6-c6",
        "c6-c3",
        "g5-j5",
        "i7-i2",
        "d3-f3"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d6-d3",
        "e5-b5",
        "e6-e3",
        "f6-c6",
        "c6-c3",
        "c3-a3",
        "a3-b3",
        "b3-a3",
        "d3-b3",
        "f5-b5xb4"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d6-d3",
        "g7-i7",
        "e5-b5",
        "g6-g9",
        "e6-e2",
        "f6-c6",
        "c6-c3",
        "g5-j5",
        "c3-b3",
        "f5-b5xb4"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f4-c4",
        "e5-e2",
        "f5-c5",
        "f6-f3",
        "f3-b3",
        "c4-c1",
        "c1-b1xb2",
        "b1-c1",
        "b3-b2",
        "c1-c3"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f4-c4",
        "f5-f2",
        "f6-f4",
        "f4-h4",
        "g5-j5",
        "g6-g1",
        "h4-i4",
        "i4-i7",
        "i7-j7",
        "j7-j8"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f8-i8",
        "h6-h9",
        "e7-b7",
        "f7-c7",
        "f6-f10",
        "f10-e10",
        "h9-a9",
        "e10-e9",
        "e9-e10",
        "f5-f10"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f8-c8",
        "e7-e10",
        "e6-e9",
        "f7-b7",
        "e5-b5xb6",
        "d6-d3",
        "f6-c6",
        "c6-c1",
        "c1-c2",
        "d3-a3"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d6-d9",
        "h6-h9",
        "e7-c7",
        "g6-j6",
        "f6-i6",
        "j6-j3",
        "j3-k3",
        "g5-g3",
        "g3-g2",
        "g7-i7"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "h6-h3",
        "f4-c4",
        "e7-e10",
        "e6-e10",
        "g7-g10xf10",
        "f8-f10",
        "g6-g9",
        "f6-i6",
        "h3-j3",
        "f7-f9"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f8-c8",
        "h6-h3",
        "g5-j5",
        "j5-j2",
        "g6-g2",
        "f6-i6",
        "i6-i5",
        "h3-i3xi4",
        "i3-i1",
        "i5-i4"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f4-c4",
        "d6-d9",
        "g7-j7",
        "f7-j7",
        "g5-j5xj6",
        "f5-i5",
        "j7-j6xi6",
        "f6-f4",
        "f4-h4",
        "g6-g1"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f8-c8",
        "h6-h3",
        "g5-j5",
        "j5-j2",
        "g6-g2",
        "f6-i6",
        "c8-i8xi7",
        "i6-i7",
        "i7-k7",
        "k7-j7"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "f4-c4",
        "h6-h9",
        "e7-e10",
        "e6-e10",
        "g7-g10xf10",
        "f8-f10",
        "g6-g9",
        "f6-i6",
        "g5-g3",
        "d6-d4"
    },
        /* length */ 10,
        /* frequency */ 1
    },
    {
        /* moves */ {
        "d6-d9",
        "f4-i4",
        "e5-e2",
        "e6-e3",
        "e2-c2",
        "f6-c6",
        "f5-b5",
        "e7-b7xb6",
        "b7-b10",
        "b5-b2"
    },
        /* length */ 10,
        /* frequency */ 1
    },
};

/*
 * book_lookup(book, book_size, played, n_played)
 *   Returns the next move string for the given sequence of played moves,
 *   or NULL if no book entry matches.  Linear scan — fine for 20 entries.
 */
#include <string.h>
static const char *book_lookup(
    const OpeningLine *book, int book_size,
    const char played[][BOOK_MOVE_LEN], int n_played)
{
    int i, j, match;
    for (i = 0; i < book_size; i++) {
        if (book[i].length <= n_played) continue;
        match = 1;
        for (j = 0; j < n_played; j++) {
            if (strncmp(book[i].moves[j], played[j], BOOK_MOVE_LEN) != 0) {
                match = 0; break;
            }
        }
        if (match) return book[i].moves[n_played];
    }
    return NULL; /* fall back to minimax */
}

#endif /* OPENING_BOOK_H */
