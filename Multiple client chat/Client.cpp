#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
//Client side
int rbit=0,sbit=0,exbit=0;
void * Send(void *arg)
{
    char msg[1500];
    int clientSd=*((int *)arg);
    while(1)
    {
        if(rbit==1)
        {
            cout<<"Receive over!";
            break;
        }
        //while(exbit);
        cout << ">>";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, data.c_str());
        if(data=="bye")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            sbit=1;
            exit(0);
        }
        send(clientSd, (char*)&msg, strlen(msg), 0);
    }
}

void *Receive(void *arg)
{
    char msg[1500];
    int clientSd=*((int *)arg);
    while(1)
    {
        if(sbit==1)
        {
            cout<<"Send Over!";
            break;
        }
        memset(&msg, '\0', sizeof(msg));     //clear the buffer
        recv(clientSd, (char*)&msg, sizeof(msg), 0);
        if(!strcmp(msg, "bye"))
        {
            cout << "Server has quit the session" << endl;
            rbit=1;
            exit(0);
        }
        else if(strlen(msg)!=0)
            printf("\t\t%s\n",msg);
    }
}
int main(int argc, char *argv[])
{
    //we need 2 things: ip address and port number, in that order
    if(argc != 3)
    {
        cerr << "Usage: ip_address port" << endl; exit(0);
    } //grab the IP address and port number
    char *serverIp = argv[1]; int port = atoi(argv[2]);
    //create a message buffer
    char msg[1500];
    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    memset((char*)&sendSockAddr, 0,sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_DGRAM, 0);
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
	       return 0;
    }


    cout << "Connected to the server!" << endl;
    cout<<"Enter Username:";
    string data;
    cin>>data;
    memset(&msg, '\0', sizeof(msg));//clear the buffer
    strcpy(msg, data.c_str());
    send(clientSd, (char*)&msg, strlen(msg), 0);
    cout<<"Enter Recipient's Username:";
    cin>>data;
    memset(&msg, '\0', sizeof(msg));//clear the buffer
    strcpy(msg, data.c_str());
    send(clientSd, (char*)&msg, strlen(msg), 0);
    pthread_t threads;
    int rc=pthread_create(&threads, NULL, Send, (void *)&clientSd);
    if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         return 0;
     }
     cout << "Begin:" << endl;
     Receive((void *)&clientSd);
     close(clientSd);
     cout << "********Session********" << endl;
     cout << "Connection closed" << endl;
    return 0;
}
