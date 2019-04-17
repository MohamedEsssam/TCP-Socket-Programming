//
// Created by rami96 on 4/13/19.
//
#include "ClientHelperFunctions.h"
char* fileName;
char fileType[100] = {0} ;

void split(char* line)
{   int index = 0;
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
        tokens[index++] = token;
        token = strtok(NULL, " ");

    }
    tokens[index] = NULL;
    fileName = tokens[1];
    char file[100] = {0};
    strcpy(file,fileName);
    dotSplit(file);
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

char* checkRequest(char* line)
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
    token = strtok(line, " ");
    while (token != NULL)
    {
        tokens[index++] = token;
        token = strtok(NULL, " ");

    }
    tokens[index] = NULL;
    return tokens[0];
}

int recieveImage(int socket)
{ // Start function

    int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
    char sizeChar[1024] = {0};
    char imagearray[10241],verify = '1';
    FILE *image;

//Find the size of the image
//    do{
//        stat = read(socket, &size, sizeof(int));
//    }while(stat<0);

    size = 114270;
    printf("Packet received.\n");
    printf("Packet size: %i\n",stat);
    printf("Image size: %i\n",size);
    printf(" \n");

    char buffer[] = "Got it";

//Send our verification signal
    do{
        stat = write(socket, &buffer, sizeof(int));
    }while(stat<0);

    printf("Reply sent\n");
    printf(" \n");

    image = fopen("capture2.jpg", "w");

    if( image == NULL) {
        printf("Error has occurred. Image file could not be opened\n");
        return -1; }

//Loop while we have not received the entire file yet


    int need_exit = 0;
    struct timeval timeout = {10,0};

    fd_set fds;
    int buffer_fd, buffer_out;

    while(recv_size < size) {
//while(packet_index < 2){

        FD_ZERO(&fds);
        FD_SET(socket,&fds);

        buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

        if (buffer_fd < 0)
            printf("error: bad file descriptor set.\n");

        if (buffer_fd == 0)
            printf("error: buffer read timeout expired.\n");

        if (buffer_fd > 0)
        {
            do{
                read_size = read(socket,imagearray, 10241);
            }while(read_size <0);

            printf("Packet number received: %i\n",packet_index);
            printf("Packet size: %i\n",read_size);


            //Write the currently read data into our image file
            write_size = fwrite(imagearray,1,read_size, image);
            printf("Written image size: %i\n",write_size);

            if(read_size !=write_size) {
                printf("error in read write\n");    }


            //Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n",recv_size);
            printf(" \n");
            printf(" \n");
        }

    }


    fclose(image);
    printf("Image successfully Received!\n");
    return 1;
}

char *readSentFile(){

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

int recieveFile(int socket)
{

    char respondStatus[30];
    read(socket, respondStatus, 30);

    respondStatus[strlen(respondStatus) - 1] = '\0';
    printf("%s",respondStatus);


    char filesize[1024] = {0};
    read(socket,filesize,1024);
    filesize[strlen(filesize)] = '\0';

    int fsize = atoi(filesize);
    char buffer[fsize];


    if(strcmp(fileType,"txt")==0 || strcmp(fileType,"html")==0)
    {
        read(socket,buffer,fsize);
        buffer[fsize] = '\0';

        FILE *file;
        file = fopen(fileName, "w");
        fprintf(file,"%s",buffer);
        memset(buffer,0,strlen(buffer));
        memset(respondStatus, 0, strlen(respondStatus));
        fclose(file);
    }

    else if(strcmp(fileType,"jpg")==0)
    {
        recieveImage(socket);
    }
}
