//
//  Created by tomas on 6. 1. 2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "piece.h"
#include "player.h"

drawLineMap(char *paMap) {
    for (int i = 0; i < 40; ++i) {
        printf("%c ",paMap[i]);
    }
    printf("\n");
}

void spaces() {
    for (int a = 0; a < 8; a++) {
        printf (" ");
    }
}

void typ1(int r, char* paMap, int c1, int c2, int c3) {
    printf ("%d ", r);
    spaces();
    printf("%c ",paMap[c1]);
    printf("%c ",paMap[c2]);
    printf("%c ",paMap[c3]);
    spaces();
    printf("\n");
}

void typ2(int r, char* paMap, int c1, int c2, char paEnds[4][4], int d1, int d2) {
    printf ("%d ", r);
    spaces();
    printf("%c ",paMap[c1]);
    printf("%c ",paEnds[d1][d2]);
    printf("%c ",paMap[c2]);
    spaces();
    printf("\n");
}

void typ3(int r, char* paMap, int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9, int c10, char paEnds[4][4], int d1, int d2) {
    printf ("%d ", r);
    printf("%c ",paMap[c1]);
    printf("%c ",paMap[c2]);
    printf("%c ",paMap[c3]);
    printf("%c ",paMap[c4]);
    printf("%c ",paMap[c5]);
    printf("%c ",paEnds[d1][d2]);
    printf("%c ",paMap[c6]);
    printf("%c ",paMap[c7]);
    printf("%c ",paMap[c8]);
    printf("%c ",paMap[c9]);
    printf("%c ",paMap[c10]);
    printf("\n");
}

void typ4(int r, char* paMap, int c1, int c2, char paEnds[4][4]) {
    printf ("%d ", r);
    printf("%c ",paMap[c1]);
    for (int i = 0; i < 4; i++) {
        printf("%c ",paEnds[3][i]);
    }
    printf("  ");
    for (int i = 3; i > -1; i--) {
        printf("%c ",paEnds[1][i]);
    }
    printf("%c ",paMap[c2]);
    printf("\n");
}

void initializeMap(char* paMap, char paEnds[4][4]) {

    for (int a = 0; a < 40; a++) {
        paMap[a] = '*';
    }
    paMap[40] = 0;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; j++){
            paEnds[i][j] = 'X';
        }
    }
}

void drawMap(char* paMap, char paEnds[4][4]) {

    printf ("  ");
    for (int a = 0; a < 11; a++) {
        printf("%d ", a%10);
    }

    printf ("\n");

    typ1(0,paMap, 38, 39, 0);
    typ2(1,paMap, 37, 1, paEnds, 0,0);
    typ2(2,paMap, 36, 2, paEnds, 0,1);
    typ2(3,paMap, 35, 3, paEnds, 0,2);
    typ3(4, paMap, 30,31,32,33,34,4,5,6,7,8,paEnds, 0,3);
    typ4(5,paMap, 29,9,paEnds);
    typ3(6, paMap, 28,27,26,25,24,14,13,12,11,10,paEnds, 2,3);
    typ2(7,paMap, 23, 15, paEnds, 2,2);
    typ2(8,paMap, 22, 16, paEnds, 2,1);
    typ2(9,paMap, 21, 17, paEnds, 2,0);
    typ1(0,paMap, 20, 19, 18);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%c", paEnds[i][j]);
        }
        printf("\n");
    }
}

int diceRoll() {
    return (rand() % (6) + 1);
}

bool checkWin(int player, char paEnds[4][4], char sign) {
    printf("checkWin(player=%d, ends=%c %c %c %c, sign=%c): ", player, paEnds[player][0], paEnds[player][1], paEnds[player][2], paEnds[player][3], sign);

    for (int i = 0; i < 4; i++) {
        if (paEnds[player][i] != sign) {
            printf("false\n");
            return false;
        }
    }

    printf("true\n");
    return true;
}

//pohne panaka o tolko miest kolko hodil hrac na kocke a upravi mapu
void movePiece(int pieceNumber, int positions, PIECE* pieces, char* map, char ends[4][4], int player) {
    int tmp;
    if(pieces[pieceNumber].onBoard == false) {
        positions--;
    }
   pieces[pieceNumber].onBoard = true;
    for ( int i = 0; i < positions+1; i++) {
        if ( pieces[pieceNumber].pos+i == (pieces[pieceNumber].startPos+39)%40) {
            pieces[pieceNumber].toHome = true;
            tmp = positions - i;
            break;
        }
    }

    if ( pieces[pieceNumber].toHome ) {
        if ( ends[player][tmp-1] != pieces[pieceNumber].sign && tmp < 5 && tmp > 0) {
            map[pieces[pieceNumber].pos] = '*';
            pieces[pieceNumber].pos = -1;
            ends[player][tmp-1] = pieces[pieceNumber].sign;
        } else if ( ends[player][tmp-1] == pieces[pieceNumber].sign || tmp >= 5 ){
            printf("NEMOZNY TAH\n");
        } else {
            map[pieces[pieceNumber].pos] = '*';
            pieces[pieceNumber].pos += positions;
            map[pieces[pieceNumber].pos] = pieces[pieceNumber].sign;
        }
    } else {
        map[pieces[pieceNumber].pos] = '*';
        pieces[pieceNumber].pos += positions;
        if(pieces[pieceNumber].pos >39) {
            pieces[pieceNumber].pos = pieces[pieceNumber].pos - 40;
        }
        map[pieces[pieceNumber].pos] = pieces[pieceNumber].sign;
    }
    }

void initializePlayerPieces(PLAYER* paPlayer, PIECE paPiece) {
    for (int i = 0; i < 4; i++) {
      paPlayer->pieces[i] = paPiece;
    }
}



char checkCollision(PLAYER* playersArray, int playerNumber, int pieceNumber, int diceRoll) {
    int projectedPosition = (playersArray[playerNumber].pieces[pieceNumber].pos) + diceRoll;
    if(projectedPosition > 39) {
        projectedPosition = projectedPosition - 40;
    }
    char collisionSign = playersArray[playerNumber].pieces[pieceNumber].sign;
    for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; j++) {
                if(playersArray[i].pieces[j].pos == projectedPosition && playersArray[i].pieces[j].onBoard == true ) { //kontrola kolizie s vlastnym return e-enemy
                    if (playersArray[i].pieces[j].sign == collisionSign) {
                        return 'f';
                    }
                    else  {
                        playersArray[i].pieces[j].pos=playersArray[i].pieces[j].startPos;
                        playersArray[i].pieces[j].onBoard = false;
                        return 'e';
                    }
                }
            }
    }
    return 'n';                                                                            //return n- no collision
}