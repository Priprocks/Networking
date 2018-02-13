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
#include <algorithm>
#include <math.h>
#define ll long long int
using namespace std;
sockaddr_in sendSockAddr;
int gPackets;
int cummRTT=0;
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


void *Receive(void *arg)
{
    char msg[1500];
    int clientSd=*((int *)arg);
    Decode D;
    int counter=0;
    while(1)
    {
        memset(&msg, '\0', sizeof(msg));     //clear the buffer
        recv(clientSd, (char*)&msg, sizeof(msg), 0);
        if(strlen(msg)==0)
            continue;
        string buff(msg);
        D.process(buff);
        if(D.TTL==1)
        {
            counter++;
            struct timeval tp;
            gettimeofday(&tp, NULL);
            ll TS = tp.tv_sec * 1000 + tp.tv_usec / 1000;
            TS=TS%((ll)pow(10,9));
            int diff=TS-D.TS;
            cummRTT+=diff;
            //cout<<"Seq:"<<D.seq<<" Diff:"<<diff<<" Counter:"<<counter<<endl;
            if(counter==gPackets)
                return (void *)0;
        }
        else
        {
            Encode E;
            E.seq=D.seq;
            E.TTL=D.TTL-1;
            //cout<<E.TTL<<" "<<E.seq<<"\n";
            E.payload=D.payload;
            string buff=E.makeheader();
            memset(msg,0,sizeof(msg));
            strncpy(msg,buff.c_str(),buff.length());
            msg[buff.length()]=0;
            sendto(clientSd,(char*)msg,strlen(msg),0,
                (sockaddr*)&sendSockAddr,sizeof(sendSockAddr));
        }
    }
}
int main(int argc, char *argv[])
{
    //Checking the correctness of command line inputs:--------------------------------------
    if(argc != 6)
    {
        cerr << "Usage: <ServerIP> <ServerPort> <P> <TTL> <NumPackets>" << endl;
        exit(0);
    }
    //--------------------------------------------------------------------------------------
    //Making header---
    Encode header;
    //----------------
    struct timeval tp;
    gettimeofday(&tp, NULL);
    ll TS1 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    cout<<TS1<<endl;
    TS1=TS1%((ll)pow(10,9));
    //Storing command line inputs:---------
    char *serverIp = argv[1];
    int port = atoi(argv[2]);
    int P = atoi(argv[3]);
    string pay(P,'A');
    header.payload = pay;
    header.TTL = atoi(argv[4]);
    int NumPackets = atoi(argv[5]);
    gPackets=NumPackets;
    //--------------------------------------


    //create a message buffer
    char msg[1500];


    //setup a socket and connection tools-------------------------------
    struct hostent* host = gethostbyname(serverIp);
    memset((char*)&sendSockAddr, 0,sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_DGRAM, 0);
    //------------------------------------------------------------------

    //Connecting:-------------------------------------------------------
    int status = connect(clientSd,
                    (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
	       return 0;
    }
    cout << "Connected to the server!" << endl;
    //------------------------------------------------------------------
    pthread_t thread;
    int rc=pthread_create(&thread, NULL, Receive, (void *)&clientSd);
    if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         return 0;
     }
    //Sending the message:----------------------------------------------
    for(int i=1;i<=NumPackets;i++)
    {
        header.seq=i;
        string buff=header.makeheader();
        memset(msg,0,sizeof(msg));
        strncpy(msg,buff.c_str(),buff.length());
        msg[buff.length()]=0;
        //cout<<buff<<" "<<buff.length()<<" "<<strlen(msg)<<endl;
        //getchar();
        sendto(clientSd,(char*)msg,strlen(msg),0,(sockaddr*)&sendSockAddr,sizeof(sendSockAddr));
    }
    pthread_join(thread,NULL);
    cout<<"Cummulative RTT: "<<cummRTT<<"\n";

    return 0;
}
