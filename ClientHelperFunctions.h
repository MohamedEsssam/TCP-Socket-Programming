//
// Created by rami96 on 4/13/19.
//

#ifndef TCP_SOCKET_PROGRAMMING_CLIENTHELPERFUNCTIONS_H
#define TCP_SOCKET_PROGRAMMING_CLIENTHELPERFUNCTIONS_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
void split(char *line);
void dotSplit(char* line);
int recieveImage(int socket);
int recieveFile(int socket);
char* checkRequest(char* line);
char* readSentFile();
#endif //TCP_SOCKET_PROGRAMMING_CLIENTHELPERFUNCTIONS_H
