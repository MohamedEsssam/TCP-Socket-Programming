#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <sstream>
#define PORT 8080
#define BUFFER_SIZE 1024
using namespace std;
char buffer[1024] = {0};
char *responseStatus;
char badRequest[30] = "HTTP/1.0 400 Bad Request\r\n";
int contentsize = 0;
char tempName[100] = {0};
char fileType[100] = {0};
char** split(char* line)
{
    contentsize = 0;
    //Array of Strings Where Each Position Has A Part of The Command
    char **tokens = (char **)malloc(2 * BUFFER_SIZE * sizeof(char *));
    //A Pointer to Char To Hold Each Token
    char *token;
    //This Is Used To Eliminate New Line In The String
    //Because It Causes Issues For strtok
    //char *newline = strchr(line,'\n');
    //if ( newline )
    //*newline = 0;

    //strtok Is Used To Break Command Into a Series of Tokens Using
    //Delimeter " "->Space
    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[contentsize++] = token;
        token = strtok(NULL, " ");

    }
    tokens[contentsize] = NULL;
    return tokens;
}


void dotSplit(char* line)
{
    int index = 0;
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
    token = strtok(line, ".");
    while (token != NULL)
    {
        tokens[index++] = token;
        token = strtok(NULL, ".");

    }
    tokens[index] = NULL;
    strcpy(fileType,tokens[1]);
}


char* readFile(char* fileName)
{
    FILE *file = fopen(fileName,"r");
    if (!file)
    {
        printf("file not found\n");
        return "";
    }
    char *fileContent = (char *) malloc(sizeof(char) * 0);
    char *lineContent = (char *) malloc(sizeof(char) * 1024);
    while (!feof(file))
    {
        char *temp = (char*) realloc(fileContent, (strlen(fileContent) + 1024) *sizeof(char));
        fileContent = temp;
        fgets(lineContent, sizeof(file),file);
        strcat(fileContent,lineContent);
        strcpy(lineContent,"\0");
    }
    strcat(fileContent,lineContent);
    fclose(file);
    return fileContent;
}

void postFile(char ** requestContent)
{
    FILE *file = fopen(requestContent[1],"w");

    for (int i = 2; i< contentsize; i++)
    {
        fprintf(file, "%s ", requestContent[i]);
    }
    fclose(file);
}

int send_image(int socket)
{

    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;

    picture = fopen("3.jpg", "r");
    printf("Getting Picture Size\n");

    if(picture == NULL)
    {
        printf("Error Opening Image File");
    }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);

    //Send Picture Size
    printf("Sending Picture Size\n");
    write(socket, (void *)&size, sizeof(int));

    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");

    do   //Read while we get errors that are due to signals.
    {
        stat=read(socket, &read_buffer, 255);
        printf("Bytes read: %i\n",stat);
    }
    while (stat < 0);

    printf("Received data in socket\n");
    printf("Socket data: %c\n", read_buffer);

    while(!feof(picture))
    {

        //Read from the file into our send buffer
        read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

        //Send data through our socket
        do
        {
            stat = write(socket, send_buffer, read_size);
        }
        while (stat < 0);

        printf("Packet Number: %i\n",packet_index);
        printf("Packet Size Sent: %i\n",read_size);
        printf(" \n");
        printf(" \n");

        packet_index++;

        //Zero out our send buffer
        bzero(send_buffer, sizeof(send_buffer));
    }
}

char *respondToRequest(char *request, int socket)
{
    char ** requestContent = split(request);
    strcpy(tempName,requestContent[1]);
    printf("file name: %s\n", tempName);
    //char ** checkImageOrtext = dotSplit(request);
    char *fileContent = "" ;
    bool requestValid = true;

    printf("request type : %s\n", requestContent[0]);
    if(strcmp(requestContent[0], "GET") == 0)
    {
        dotSplit(tempName);
        printf("file type: %s\n", fileType);
        if(strcmp(fileType,"jpg")==0)
        {
            send_image(socket);
        }
        else
        {
            fileContent = readFile(requestContent[1]);
            responseStatus = (char *) malloc(sizeof(char) * (strlen(fileContent) + 35));


            if (strcmp(fileContent, "") == 0)
                strcpy(responseStatus, "HTTP/1.0 404 Not Found\r\n");
            else
                strcpy(responseStatus, "HTTP/1.0 200 OK\r\n");
        }
    }
    else if(strcmp(requestContent[0], "POST") == 0)
    {
        postFile(requestContent);
        responseStatus = (char*) malloc(sizeof(char) * (strlen(fileContent) + 35));
        strcpy(responseStatus, "HTTP/1.0 200 OK\r\n");
    }
    else
    {
        requestValid = false;
    }

    if(requestValid)
    {

        send(socket, responseStatus, strlen(responseStatus), 0);
        sleep(1);

        stringstream temp;
        temp << (int)strlen(fileContent);
        string str = temp.str();
        char const *responseSize = str.c_str();

        send(socket, responseSize, strlen(responseSize), 0);
        temp.str("");

        sleep(1);
        send(socket, fileContent, strlen(fileContent), 0);

        close(socket);
    }
    else
    {
        send(socket, badRequest, strlen(badRequest), 0 );
        printf("%s", badRequest);
        close(socket);
    }

}

void *respond(void *new_socket)
{
    int *temp = (int *) new_socket;
    int newSocketFD = *temp;

    char requestSizeAsString[1024] = {0};
    read(newSocketFD,requestSizeAsString,1024);
    requestSizeAsString[strlen(requestSizeAsString)] = '\0';

    int requestSizeAsInt = atoi(requestSizeAsString);
    char buffer[requestSizeAsInt];
    buffer[requestSizeAsInt] = '\0';


    read(newSocketFD, buffer, 1024);

    printf("New Request : %s\n", buffer);

    respondToRequest(buffer, newSocketFD);
    memset(buffer, 0, sizeof(buffer));
}


int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t respondThread;
        pthread_create(&respondThread, NULL, respond, (void*) &new_socket);
        pthread_detach(respondThread);
    }

    return 0;
}
