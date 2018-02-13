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
#include<bits/stdc++.h>
#include <algorithm>
#include <math.h>
#define ll long long int
using namespace std;

class Encode
{
public:
    int seq,TTL;
    ll TS;
    string payload;
    string fixstring(string val, int digits = 3)
    {
        string s(digits,255);
        int len=digits-val.length();
        s.replace(s.begin()+len,s.end(),val);
        return s;
    }
    string ascii(ll n)
    {
        // char array to store hexadecimal number
        string ascii="";


        while(n!=0)
        {
            // temporary variable to store remainder
            ll temp  = 0;

            // storing remainder in temp variable.
            temp = n % 254;
            if(temp==0)
                temp=254;
            // check if temp < 10
            unsigned char t=(temp);
            //cout<<"conv:"<<(int)t<<endl;
            ascii+=t;

            n = n/254;
        }
        reverse(ascii.begin(),ascii.end());
        // return string
        return ascii;
    }
    string makeheader()
    {
        /*
            Making header file:
            seq-----> 2 Bytes
            TS------> 4 Bytes
            TTL-----> 2 Bytes
            Payload-> P Bytes
        */
        string header=""; //Creating header


        //Getting sequence:--------------------------------
        string sseq=ascii(seq);
        header+=fixstring(sseq,2);  //Adding Sequence
        //-------------------------------------------------


        //Getting timestamp:-------------------------------
        struct timeval tp;
        gettimeofday(&tp, NULL);
        TS = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        //cout<<"TS:"<<TS<<endl;
        TS=TS%((ll)pow(10,9));
        string ts=ascii(TS);
        header+=fixstring(ts,4);    //Adding timestamp
        //-------------------------------------------------

        //Getting TTL:-------------------------------------
        string ttl=ascii(TTL);
        header+=fixstring(ttl,2); //Adding TTL
        //-------------------------------------------------

        //Payload:-----------------------------------------
        header+=payload;
        //-------------------------------------------------

        return header;
    }
};
class Decode
{
public:
    int seq,TTL;
    ll TS;
    string payload;
    ll asconv(string s)
    {
        ll l=s.length();
        ll a=0;
        for(int i=0;i<l;i++)
        {
            if((int)s[i]>=0 && (int)s[i]!=-1)
                a+=(s[i])*pow(254,l-i-1);
            else if(s[i]<0 && (int)s[i]!=-1 && (int)s[i]!=-2)
                a+=(s[i]+256)*pow(254,l-i-1);


            //cout<<s<<" "<<(int)s[i]<<" "<<a<<endl;
            //cout<<(int)s[i]<<" "<<pow(256,l-i-1)<<" "<<a<<endl;
        }
        return a;
    }
    void process(string header)
    {
        /*
            Header file:
            seq-----> 2 Bytes
            TS------> 4 Bytes
            TTL-----> 2 Bytes
            payload-> P Bytes
        */

        //Getting sequence:--------------------------------
        string sseq=header.substr(0,2);
        seq=asconv(sseq);  //Adding Sequence
        //-------------------------------------------------


        //Getting timestamp:-------------------------------
        string ts=header.substr(2,4);
        TS = asconv(ts);  //Adding timestamp
        //-------------------------------------------------

        //Getting TTL:-------------------------------------
        string ttl=header.substr(6,2);
        TTL= asconv(ttl); //Adding TTL
        //-------------------------------------------------

        //Getting Payload:---------------------------------
        payload=header.substr(8);
    }
};



int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    int port = atoi(argv[1]);
    char msg[1500];
    struct timeval tp;
    gettimeofday(&tp, NULL);
    ll TS1 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    cout<<TS1<<endl;
    TS1=TS1%((ll)pow(10,9));
    //setup a socket and connection tools
    sockaddr_in servAddr;
    memset((char*)&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_DGRAM, 0);
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
    cout << "Connection established!" << endl;
    socklen_t newSockAddrSize = sizeof(servAddr);
    Decode D;
    Encode E;
    while(1)
    {
        //ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
        //                struct sockaddr *src_addr, socklen_t *addrlen);
        memset(msg,'\0',sizeof(msg));
        recvfrom(serverSd,(char *)msg,sizeof(msg),0,(sockaddr *)&servAddr, &newSockAddrSize);
        if(strlen(msg)>0)
        {
                //cout<<">>"<<msg<<endl;
            string s(msg);
            D.process(s);
            E.seq=D.seq;
            E.TTL=D.TTL-1;
            cout<<E.TTL<<" "<<E.seq<<"\n";
            E.payload=D.payload;
            string buff=E.makeheader();
            memset(msg,0,sizeof(msg));
            strncpy(msg,buff.c_str(),buff.length());
            msg[buff.length()]=0;
            //ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
            //          const struct sockaddr *dest_addr, socklen_t addrlen);
            sendto(serverSd,(char*)msg,strlen(msg),0,
                        (sockaddr*)&servAddr, sizeof(servAddr));
            /*
            header.process(s);
            struct timeval tp;
            gettimeofday(&tp, NULL);
            ll TS = tp.tv_sec * 1000 + tp.tv_usec / 1000;
            TS=TS%((ll)pow(10,9));
            int diff=TS-header.TS;
            cout<<TS<<" "<<header.TS<<" "<<diff<<endl;
            //Printing purpose:
            cout<<"-------------------------------\n";
            cout<<"Sequence:"<<header.seq<<"\n";
            cout<<"TTL:"<<header.TTL<<"\n";
            cout<<"TS:"<<header.TS<<"\n";
            cout<<"Payload:"<<header.payload<<"\n\n";
            */
        }
        memset(msg,'\0',sizeof(msg));
    }
    return 0;
}
