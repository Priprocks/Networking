#include<bits/stdc++.h>
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
#include<pthread.h>
using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    int port = atoi(argv[1]);
    char msg[1500];

    //setup a socket and connection tools
    sockaddr_in servAddr;
    memset((char*)&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        return 0;
    }
    //bind the socket to its local address
    int b = bind(serverSd, (struct sockaddr*) &servAddr,
        sizeof(servAddr));
    if(b < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        return 0;
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to
    //handle the new connection with client
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        return 0;
    }
    cout << "Connected with client!" << endl;
    char msg1[50];
    vector<string> v;
    memset(msg1,0,sizeof(msg1));
    recv(newSd, (char*)&msg1, sizeof(msg1), 0);
    printf("Char: %s\n",msg1);
    cout<<strlen(msg1)<<endl;
    string temp(msg1);
    v.push_back(temp);
    printf("String: %s \n",temp.str());

}
