#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include "./ClientHelperFunctions.h"
#define PORT 8080

using namespace std;

int main()
{
    int mySocket, connStatus;
    struct sockaddr_in serv_addr;
    char input[100];

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
    cout<<input<<"\n";
    char input2[100] = {0};
    strcpy(input2,input);
    //send(mySocket, input, strlen(input), 0 );
    char* requestType = checkRequest(input);
    if(strcmp(requestType,"GET") == 0)
    {
        stringstream temp;
        temp << (int)strlen(input2);
        string str = temp.str();
        char const *requestSize = str.c_str();

        printf("request size = %s\n", requestSize);

        send(mySocket, requestSize, strlen(requestSize), 0);
        temp.str("");

        printf("request : %s\n", input2);

        sleep(2);

        send(mySocket, input2, strlen(input2), 0 );
        split(input2);
        recieveFile(mySocket);
    }

    else if(strcmp(requestType, "POST") == 0)
    {
        char postedFile[1024];
        strcpy(postedFile, input2);

        split(postedFile);
        strcat(input2, " ");
        char *temp2 = (char *)malloc(strlen(input2) * sizeof(char));
        strcpy(temp2,input2);
        char *fileContent =readSentFile();

        printf("%d", strlen(fileContent));

        char *requestContent = (char *)malloc((strlen(fileContent) + 50) * sizeof(char));

        strcpy(requestContent, input2);
        strcat(requestContent, fileContent);
        requestContent[strlen(fileContent) + strlen(input2)] = '\0';



        if(strcmp(requestContent,temp2) == 0)
        {
            close(mySocket);
            return 0;
        }



        stringstream temp;
        temp << (int)strlen(requestContent);
        string str = temp.str();
        char const *requestSize = str.c_str();

        printf("request size = %s\n", requestSize);

        send(mySocket, requestSize, strlen(requestSize), 0);
        temp.str("");

        sleep(2);

        printf("request : %s\n", requestContent);
        send(mySocket, requestContent, strlen(requestContent), 0 );
        memset(input2,0,100);
        read(mySocket,input2,100);
        printf("%s",input2);

    }
    else{
        printf("Not a Request\n");
    }

    close(mySocket);
    return 0;
}
