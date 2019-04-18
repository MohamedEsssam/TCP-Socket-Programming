//
// Created by rami96 on 4/18/19.
//

#ifndef CLIENT_SERVERHELPERFUNCTIONS_H
#define CLIENT_SERVERHELPERFUNCTIONS_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>

char** split(char* line);
void dotSplit(char* line);
char* readFile(char* fileName);
void postFile(char ** requestContent);
int send_image(int socket);
char *respondToRequest(char *request, int socket);
void *respond(void *new_socket);
using namespace std;
#endif //CLIENT_SERVERHELPERFUNCTIONS_H
