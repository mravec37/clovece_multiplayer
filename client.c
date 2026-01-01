#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "game_logic.h"

int client(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char gameOver = 'n';
    char buffer[256];
    bool receiveMap = true;

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd <= 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }

 while(gameOver=='n') {
     if(receiveMap == true) {
         bzero(buffer, 256);
         n = read(sockfd, buffer, 41);
         if (n <= 0) {
             perror("Error reading from socket");
             return 6;
         }
         char *map = buffer;
         //printf("Chcem precitat mapu \n");
         drawLineMap(map);

         char ends[4][4];
         n = read(sockfd, ends, 16);
         if (n <= 0) {
             perror("Error reading from socket");
             return 6;
         }
         //printf("Chcem precitat velku mapu \n");
         drawMap(map, ends);
         receiveMap = false;
     }
    bool chooseRoll = true;
    bool choosePiece = true;
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n <= 0) {
        perror("Error reading from socket");
        return 6;
    }
    if (strcmp(buffer, "yourTurn") == 0) {
        while (chooseRoll == true) {                     //cakam kym uzivatel nestlaci y aby dal prikaz na hodenie kocky
            bzero(buffer, 256);
            printf("Your turn, press 'y' if you want to roll the dice: \n");
            fgets(buffer, 255, stdin);
            if (strcmp(buffer, "y\n") == 0) {
                chooseRoll = false;
                n = write(sockfd, buffer, strlen(buffer) + 1);
                if (n <= 0) {
                    perror("Error writing to socket");
                    return 5;
                }
            }
        }
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n <= 0) {
            perror("Error reading from socket");
            return 6;
        }
        printf("You rolled number %s, choose which piece you want to move (1-4) \n", buffer);
        while (choosePiece == true) {                //cakam kym si uzivatel vyberie panacika s ktorym chce pohnut
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            choosePiece = false;
            int input = buffer[0] - '0';
            if (input > 4 || input < 1) {
                choosePiece = true;
                printf("Choose number between 1-4 to move that piece: \n");
            } else {
                n = write(sockfd, buffer, strlen(buffer) + 1);
                if (n <= 0) {
                    perror("Error writing to socket");
                    return 5;
                }
            }

        }
        bzero(buffer, 256);
        n = read(sockfd, buffer, 41);
        if (n <= 0) {
            perror("Error reading from socket");
            return 6;
        }

        char* map = buffer;
        drawLineMap(map);

        char ends[4][4];
        n = read(sockfd, ends, 16);
        if (n <= 0) {
            perror("Error reading from socket");
            return 6;
        }

        drawMap(map, ends);
        receiveMap = true;

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n <= 0) {
            perror("Error reading from socket");
            return 6;
        }
        if (buffer[0]=='y') {
            gameOver = 'y';
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n <= 0) {
                perror("Error reading from socket");
                return 6;
            }
            printf("The game is over, player %d won \n", buffer[0] + '0');
        }
    }
        else {
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n <= 0) {
                perror("Error reading from socket");
                return 6;
            }
            printf(" %s\n", buffer);
        }

    }

    close(sockfd);

    return 0;
}