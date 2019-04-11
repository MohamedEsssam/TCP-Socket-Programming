#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

#define PORT 8080
#define BUFFER_SIZE 1024
using namespace std;
char ok[1024] = "HTTP/1.0 200 OK\r\n";
char badRequest[1024] = "HTTP/1.0 400 Bad Request\r\n";

char** split(char* line){
    //Index Variable
    int position = 0;
    //Array of Strings Where Each Position Has A Part of The Command
    char **tokens = (char **)malloc(BUFFER_SIZE * sizeof(char *));
    //A Pointer to Char To Hold Each Token
    char *token;
    //This Is Used To Eliminate New Line In The String
    //Because It Causes Issues For strtok
    char *newline = strchr(line,'\n');
    if ( newline )
        *newline = 0;

    //strtok Is Used To Break Command Into a Series of Tokens Using
    //Delimeter " "->Space
    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        token = strtok(NULL, " ");

    }
    tokens[position] = NULL;
    return tokens;
}

char* readFile(char* fileName){
    FILE *file = fopen(fileName,"r");
    if (!file) {printf("file not found");}
    char *fileContent = (char *) malloc(sizeof(char) * 1024);
    char *lineContent = (char *) malloc(sizeof(char) * 1024);
    while (!feof(file)) {
        fgets(lineContent, sizeof(file),file);
        strcat(fileContent,lineContent);
        strcpy(lineContent,"\0");
    }
    fclose(file);
    return fileContent;
}

void postFile(char ** requestContent){
    FILE *file = fopen(requestContent[1],"a+");
    for (int i = 2; i< sizeof(requestContent); i++){
        fprintf(file, "%s", requestContent[i]);
    }
    fclose(file);
}

void respondToRequest(char *request, int socket){
    char ** requestContent = split(request);
    char *fileContent = "" ;
    bool responsStatus = true;
    if(strcmp(requestContent[0], "GET") == 0){
        fileContent = readFile(requestContent[1]);
    }
    else if(strcmp(requestContent[0], "POST") == 0){
        postFile(requestContent);
    }
    else{
        responsStatus = false;
    }

    if(responsStatus){
        strcat(ok,fileContent);
        printf("200 ok send\n");
    }
    else{
        send(socket , badRequest , strlen(badRequest) , 0 );
        printf("400 bad request send\n");
    }

}

int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    char buffer[1024] = {0};


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    respondToRequest(buffer, new_socket);
    return 0;
}