#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "game_logic.h"
#include "player.h"


typedef struct arguments {
    int newsockfd;
    int* playersTurn;
    int playerNumber;
    bool *gameOver;
    int numberOfPlayers;
    char* map;
    char (*ends)[4][4];
    PLAYER *player;
    PLAYER* playersArray;
    pthread_mutex_t* mutex;
} ARGUMENTS;


void* clientHandler(void* parameters) {
    ARGUMENTS* args = (ARGUMENTS*) parameters;
    printf("Player %d has a turn\n", *args->playersTurn);
    int newsockfd = args->newsockfd;
    int n;
    char buffer[256];
    bool writeMap = true;
    bzero(buffer,256);
    if (newsockfd <= 0) {
        perror("ERROR on accept");
        return NULL;
    }

    while(*args->gameOver == false) {
        sleep(1);
        pthread_mutex_lock(args->mutex);

        if (*args->gameOver == true) {
            pthread_mutex_unlock(args->mutex);
            break;
        }
        if(writeMap == true) {
            n = write(newsockfd, args->map, strlen(args->map) + 1);
            if (n <= 0) {
                perror("Error writing to socket");
                return NULL;
            }

            char buf2[16];
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    buf2[i * 4 + j] = (*args->ends)[i][j];
                }
            }
            n = write(newsockfd, buf2, 16);
            if (n < 0) {
                perror("Error writing to socket");
                return NULL;
            }
            writeMap = false;
        }
       if (*args->playersTurn == args->playerNumber) {                                              //klient je na rade
            const char *msg2 = "yourTurn";
            n = write(newsockfd, msg2, strlen(msg2) + 1);
            printf("Player %d has a turn\n", *args->playersTurn);
           if (n <= 0) {
               perror("Error writing to socket");
               return NULL;
           }
           bzero(buffer,256);
           n = read(newsockfd, buffer, 255);
           if (n < 0) {
               perror("Error reading from socket");
               return NULL;
           }
           if(strcmp(buffer,"y")) {                                                                             //klient poslal ze chce hodit kockou
               char result[4];
               int num = diceRoll();
               sprintf(result, "%d", num);
               const char *rollDiceMsg = result;
               n = write(newsockfd, rollDiceMsg, strlen(rollDiceMsg) + 1);
               bzero(buffer,256);
               n = read(newsockfd, buffer, 255);
               if (n <= 0) {
                   perror("Error reading from socket");
                   return NULL;
               }
               int pieceToMove = buffer[0] - '0';
               printf("Player wants to move piece %d \n", pieceToMove);

               if(checkCollision(args->playersArray, args->playerNumber - 1, pieceToMove-1,num) == 'e') {
                   //enemy collision
                   movePiece(pieceToMove-1, num, args->player->pieces, args->map, *args->ends, args->playerNumber - 1);   //nastavime panacika na miesto podla toho kolko hrac hodil
                   printf("Enemy collision detected\n");
               } else if (checkCollision(args->playersArray, args->playerNumber - 1, pieceToMove-1,num) =='f') {
                    //friendly collision
                   drawMap(args->map, *args->ends);
                   printf("Friendly collision detected\n");
               } else if(checkCollision(args->playersArray, args->playerNumber - 1, pieceToMove-1,num) == 'n') {
                    //neni kolizia
                   movePiece(pieceToMove-1, num, args->player->pieces, args->map, *args->ends, args->playerNumber - 1);   //nastavime panacika na miesto podla toho kolko hrac hodil
                   printf("No collision \n");
               } else {
                   printf("Chyba pri detekovani kolizie \n");
               }

               n = write(newsockfd, args->map, strlen(args->map) + 1);
               printf("Player %d has a turn\n", *args->playersTurn);
               if (n <= 0) {
                   perror("Error writing to socket");
                   return NULL;
               }
               char buf[16];
               for (int i = 0; i < 4; i++) {
                   for (int j = 0; j < 4; j++) {
                       buf[i * 4 + j] = (*args->ends)[i][j];
                   }
               }
               n = write(newsockfd, buf, 16);
               printf("Player %d has a turn\n", *args->playersTurn);
               if (n <= 0) {
                   perror("Error writing to socket");
                   return NULL;
               }

               drawMap(args->map, *args->ends);
               if (checkWin(args->playerNumber - 1, *args->ends, args->player->pieces[0].sign)) {
                   printf("Player %d is the winner\n", args->playerNumber);
                   *args->gameOver = true;
                   const char *winMessage = "y";
                   n = write(newsockfd, winMessage, strlen(winMessage) + 1);
                   if (n <= 0) {
                       perror("Error writing to socket");
                       return NULL;
                   }
                   char result2[4];
                   sprintf(result2, "%d", args->playerNumber);
                   const char *winMessage2 = result2;
                   n = write(newsockfd, winMessage2, strlen(winMessage2) + 1);
                   if (n < 0) {
                       perror("Error writing to socket");
                       return NULL;
                   }

               } else {
                   const char *winMessage = "n";
                   n = write(newsockfd, winMessage, strlen(winMessage) + 1);
                   if (n <= 0) {
                       perror("Error writing to socket");
                       return NULL;
                   }
               }
               writeMap=true;
               (*args->playersTurn)++;
           }


        } else if(*args->playersTurn > args->numberOfPlayers ) {                //preslo kolo a na radu ide prvy hrac
            *args->playersTurn = 1;
            if (n <= 0) {
                perror("Error writing to socket");
                return NULL;
            }
        } else {                                                                    //na rade je niekto iny
            printf("Player %d has a turn\n", *args->playersTurn);
            if (n <= 0) {
                perror("Error writing to socket");
                return NULL;
            }
        }
        pthread_mutex_unlock(args->mutex);
    }

}

int server(int argc, char *argv[])
{
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];
    pthread_t pthread;
    pthread_t pthread2;
    pthread_t pthread3;
    pthread_t pthread4;
    PLAYER players[4];
    int playersTurn;
    playersTurn = 1;
    bool gameOver= false;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    int connectedPlayers = 0;
    int numberOfPlayers =4; //*(argv[2]) - '0';
    char map[49];
    char ends[4][4];
    PLAYER playersArray[4];
    initializeMap(map, ends);
    drawMap(map, ends);


    PIECE pPl1 = {'A', 0, 0,false, false};
    PIECE pPl2 = {'B', 10,  10,false, false};
    PIECE pPl3 = {'C', 20, 20,false, false};
    PIECE pPl4 = {'D', 30,30,false, false};

    PLAYER player1;
    PLAYER player2;
    PLAYER player3;
    PLAYER player4;

    initializePlayerPieces(&player1, pPl1);
    initializePlayerPieces(&player2, pPl2);
    initializePlayerPieces(&player3, pPl3);
    initializePlayerPieces(&player4, pPl4);

    playersArray[0] = player1;
    playersArray[1] = player2;
    playersArray[2] = player3;
    playersArray[3] = player4;

    printf("Znak hraca a je: %c \n", player1.pieces->sign);


    ARGUMENTS args = {0, &playersTurn,1,&gameOver, numberOfPlayers, map,&ends, &playersArray[0],playersArray, &mutex};
    ARGUMENTS args2 = {0, &playersTurn,2, &gameOver,numberOfPlayers, map, &ends,  &playersArray[1],playersArray, &mutex};
    ARGUMENTS args3 = {0, &playersTurn,3,&gameOver, numberOfPlayers, map, &ends,  &playersArray[2], playersArray, &mutex};
    ARGUMENTS args4 = {0, &playersTurn,4, &gameOver,numberOfPlayers, map, &ends,  &playersArray[3], playersArray, &mutex};
    srand(time(NULL));

   /* if (numberOfPlayers < 1 || numberOfPlayers > 4) {
        puts("Wrong number of players argument");
        return 1;
    }*/

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));  //vycisti celu strukturu
    serv_addr.sin_family = AF_INET;                //nastavi strukturu
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);  //vytvori socket a vrati jeho ID
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)  //namapujem na ten socket(ponocou ID) tu strukturu co som nastavil
    {
        perror("Error binding socket address!");
        return 2;
    }

    listen(sockfd, 5);      //urobi z toho pasivny socket-> urceny na to aby sa niekto nanho pripojil a nie na komunikaciu s klientom
    //po pripojeni si server vytvori novy socket pomocou kt. bude komunikovat s klientom, cislo udava kolko klientov sa moze pripojit v jednom okamziku na server?, front neobsluzenych klientov
    cli_len = sizeof(cli_addr);

    while(connectedPlayers != numberOfPlayers) {
        puts("Waiting for all players to connect");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
                           &cli_len);    //vrati novy socket pomocou ktoreho budem komunikovat s klientom ktory sa pripojil na server
        connectedPlayers++;
        //aa
        //pthread_create(&pthread[connectedPlayers-1], NULL, fun1, &args);
        if (connectedPlayers == 1) {                                                                  //nastavi thready
            args.newsockfd = newsockfd;
        }
        if (connectedPlayers == 2) {
            args2.newsockfd = newsockfd;
        }
        if (connectedPlayers == 3) {
            args3.newsockfd = newsockfd;
        }
        if (connectedPlayers == 4) {
            args4.newsockfd = newsockfd;
        }
    }
    pthread_create(&pthread, NULL, clientHandler, &args);
    pthread_create(&pthread2, NULL,  clientHandler, &args2);
    pthread_create(&pthread3, NULL,  clientHandler, &args3);
    pthread_create(&pthread4, NULL,  clientHandler, &args4);
    puts("All players connected");

    pthread_join(pthread, NULL);
    pthread_join(pthread2, NULL);
    pthread_join(pthread3, NULL);
    pthread_join(pthread4, NULL);
    puts("end");
    close(newsockfd);
    close(sockfd);

    return 0;

}


