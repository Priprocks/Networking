#include<bits/stdc++.h>
#include <string>
#include<sys/time.h>
#define ll long long int
using namespace std;

template<class T>
string to_string(const T& t)
{
    stringstream ss;
    ss<<t;
    return ss.str();
}



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
        cout<<"TS:"<<TS<<endl;
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


            cout<<s<<" "<<(int)s[i]<<" "<<a<<endl;
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


int main()
{
    /*memset(msg,'\0',sizeof(msg));
    cin>>msg;
    if(strlen(msg)>0)
        sendto(clientSd,(char*)msg,strlen(msg),0,(sockaddr*)&sendSockAddr,sizeof(sendSockAddr));*/
        Encode e;
        Decode d;
        cin>>e.seq>>e.TTL;
        string s=e.makeheader();
        cout<<s;
        d.process(s);
        cout<<endl<<d.seq<<endl<<d.TTL<<endl<<d.TS<<endl;
        //char c=254;
        //cout<<(int)c;
}
