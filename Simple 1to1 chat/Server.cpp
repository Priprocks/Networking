
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#define emp(x) memset(&x,0,sizeof(x));
using namespace std;

int main(int argc,char * argv[])
{
    /* ---------- INITIALIZING VARIABLES ---------- */

    /*
       1. client/server are two file descriptors
       These two variables store the values returned
       by the socket system call and the accept system call.
       2. portNum is for storing port number on which
       the accepts connections
       3. isExit is bool variable which will be used to
       end the loop
       4. The server reads characters from the socket
       connection into a dynamic variable (buffer).
       5. A sockaddr_in is a structure containing an internet
       address. This structure is already defined in netinet/in.h, so
       we don't need to declare it again.
        DEFINITION:
        struct sockaddr_in
        {
          short   sin_family;
          u_short sin_port;
          struct  in_addr sin_addr;
          char    sin_zero[8];
        };
        6. serv_addr will contain the address of the server
        7. socklen_t  is an intr type of width of at least 32 bits
    */
    int client, server;
    int portNum=atoi(argv[1]);
    int bufsize = 1024;
    char buffer[bufsize];

    struct sockaddr_in server_addr;
    socklen_t size;

    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    /* --------------- socket() function ------------------*/

    server = socket(AF_INET, SOCK_STREAM, 0);
    //cout<<AF_INET<<" "<<SOCK_STREAM<<"\n";
    if (server < 0)
    {
        cout << "\nError establishing socket!" << endl;
        exit(1);
    }

    /*
        The socket() function creates a new socket.
        It takes 3 arguments,
            a. AF_INET: address domain of the socket.
            b. SOCK_STREAM: Type of socket. a stream socket in
            which characters are read in a continuous stream (TCP)
            c. Third is a protocol argument: should always be 0. The
            OS will choose the most appropiate protocol.
            This will return a small integer and is used for all
            references to this socket. If the socket call fails,
            it returns -1.
    */

    cout << "\n=> Socket server has been created..." << endl;

    /*
        The variable server_addr is a structure of sockaddr_in.
        sin_family contains a code for the address family.
        It should always be set to AF_INET.
        INADDR_ANY contains the IP address of the host. For
        server code, this will always be the IP address of
        the machine on which the server is running.
        htons() converts the port number from host byte order
        to a port number in network byte order.
    */
    memset((char *)&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    /* ---------- BINDING THE SOCKET ---------- */
    /* ---------------- bind() ---------------- */
    cout<<"IP:"<<INADDR_ANY<<"\n";

    if ((bind(server, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0)
    {
        cout << "=> Error binding connection, the socket has already been established!" << endl;
        return -1;
    }

    /*
        The bind() system call binds a socket to an address,
        in this case the address of the current host and port number
        on which the server will run. It takes three arguments,
        the socket file descriptor. The second argument is a pointer
        to a structure of type sockaddr, this must be cast to
        the correct type.
    */

    size = sizeof(server_addr);
    cout << "=> Looking for clients..." << endl;

    /* ------------- LISTENING CALL ------------- */
    /* ---------------- listen() ---------------- */

    listen(server, 5);
    /*
        The listen system call allows the process to listen
        on the socket for connections.
        The program will be stay idle here if there are no
        incoming connections.
        The first argument is the socket file descriptor,
        and the second is the size for the number of clients
        i.e the number of connections that the server can
        handle while the process is handling a particular
        connection. The maximum size permitted by most
        systems is 5.
    */

    /* ------------- ACCEPTING CLIENTS  ------------- */
    /* ----------------- listen() ------------------- */

    /*
        The accept() system call causes the process to block
        until a client connects to the server. Thus, it wakes
        up the process when a connection from a client has been
        successfully established. It returns a new file descriptor,
        and all communication on this connection should be done
        using the new file descriptor. The second argument is a
        reference pointer to the address of the client on the other
        end of the connection, and the third argument is the size
        of this structure.
    */
    client = accept(server,(struct sockaddr *)&server_addr,&size);

    // first check if it is valid or not
    if (client < 0)
        cout << "=> Error on accepting..." << endl;

    while (1)
    {
        cout<<"Waiting for the Client to respond!\n";
        emp(buffer);
        recv(client,&buffer,sizeof(buffer),0);
        if(strcmp("Exit",buffer)==0)
        {
            cout<<"Client left! :(\n";
            break;
        }
        cout<<"Client: "<<buffer<<"\n";
        string text;
        getline(cin,text);
        emp(buffer);
        strcpy(buffer,text.c_str());
        if(text=="Exit")
        {
            send(client,&buffer,sizeof(buffer),0);
            break;
        }
        send(client,&buffer,sizeof(buffer),0);
    }
    close(client);
    close(server);
    return 0;
}
