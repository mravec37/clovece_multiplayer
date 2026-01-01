//
//  Created by tomas on 6. 1. 2022.
//

#ifndef UNTITLED5_GAME_LOGIC_H
#define UNTITLED5_GAME_LOGIC_H


#include "piece.h"
#include "player.h"
void drawLineMap(char *paMap);
void initializeMap(char* paMap, char paEnds[4][4]);
void drawMap(char* paMap, char paEnds[4][4]);
int diceRoll();
void movePiece(int pieceNumber, int positions, PIECE* pieces, char* map, char paEnds[4][4], int player);
void initializePlayerPieces(PLAYER* paPlayer, PIECE paPiece);
char checkCollision(PLAYER* playersArray, int playerNumber, int pieceNumber, int diceRoll);
bool checkWin(int player, char paEnds[4][4], char sign);
#endif //UNTITLED5_GAME_LOGIC_H
