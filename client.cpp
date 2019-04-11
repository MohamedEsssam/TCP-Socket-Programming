#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
using namespace std;
int main() {
    int mySocket, connStatus, valread;
    struct sockaddr_in serv_addr;
    char *message = "Hello from client";
    char input[100];
    char buffer[1024] = {0};
    FILE *received_file;
    ssize_t len;
    int file_size,remain_data;
    mySocket = socket(AF_INET,SOCK_STREAM,0);
    if(mySocket<0)
    {
        cout<<"ERROR opening socket\n";
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        cout<<"\nInvalid address/ Address not supported \n";
        return -1;
    }

    connStatus = connect(mySocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(connStatus < 0)
    {
        cout<<"\nConnection Failed \n";
        return -1;
    }

    else
    {
        cout<<"Good Connection\n";
    }

    cout<<"Enter Command: ";
    cin.get(input, 100);
    send(mySocket , input , strlen(input) , 0 );
//    printf("Hello message sent\n");
//
//    recv(mySocket, buffer, BUFSIZ, 0);
//    file_size = atoi(buffer);
//    //fprintf(stdout, "\nFile size : %d\n", file_size);
//
//    received_file = fopen("index.txt", "w");
//    if (received_file == NULL)
//    {
//        fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));
//
//        exit(EXIT_FAILURE);
//    }
//
//    remain_data = file_size;
//
//    while ((remain_data > 0) && ((len = recv(mySocket, buffer, BUFSIZ, 0)) > 0))
//    {
//        fwrite(buffer, sizeof(char), len, received_file);
//        remain_data -= len;
//        fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
//    }
//    fclose(received_file);

    close(mySocket);
    return 0;
}
