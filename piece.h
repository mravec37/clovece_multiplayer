//
// Created by tomas on 6. 1. 2022.
//

#ifndef UNTITLED5_PIECE_H
#define UNTITLED5_PIECE_H

#include <stdbool.h>
typedef struct piece {
    char sign;
    int startPos;
    int pos;
    bool onBoard;
    bool toHome;
} PIECE;


#endif  //UNTITLED5_PIECE_H
