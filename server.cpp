#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

char * readFile();
char *  split();

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";


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
    char *lineContent = readFile();
    send(new_socket , lineContent , strlen(lineContent) , 0 );
    printf("Hello message sent\n");
    return 0;
}

char* split(char* clientMessage){
    char **fileName;
    for(int i = 0; i < sizeof(clientMessage); i++) {
        fileName[i] = strsep(&clientMessage, " ");
        if(fileName[i] == NULL) break;
    }
    return fileName[1];
}

char* readFile(char* clientMessage){
    char *fileName = split(clientMessage);
    FILE *file = fopen(fileName,"r");
    if (!file) {printf("file not found");}
    char *fileContent = (char *) malloc(sizeof(char) * 1024);
    char *lineContent = (char *) malloc(sizeof(char) * 1024);
    while (!feof(file)) {
        fgets(lineContent, sizeof(file),file);
        strcat(fileContent,lineContent);
        strcpy(lineContent,"\0");
    }
    return lineContent;
}

