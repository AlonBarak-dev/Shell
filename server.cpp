#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <thread>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sstream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <sys/un.h>
#include <stdio.h>

#define PORT 55050;
using namespace std;

// inspired from : https://www.technical-recipes.com/2014/getting-started-with-client-server-applications-in-c/

int main(){


    int ret_code;
    // start the server
    int listen_port = PORT; // port number
    //start a scoket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // assert the socket is valid
    if (sock < 0){
        cerr << "Error in socket: " << strerror(errno) << endl;
        exit(1);   
    }

    // inititlize the server attributes
    struct sockaddr_in myaddr;
    memset(&myaddr, '0', sizeof(struct sockaddr_in)); 
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(listen_port);        // Port to listen
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // bind the socket to the address
    int bnd = bind(sock, (struct sockaddr*) &myaddr, sizeof(myaddr));
    if (bnd < 0)
    {
        std::cerr << "Error in bind: " << strerror(errno) << std::endl;
        exit(1);
    }

    while(true){
        // listen for connection from client
        int listen_res = listen(sock, 10);
        if (listen_res < 0)
        {
            std::cerr << "Error listen: " << strerror(errno) << std::endl;
            exit(1);
        }

        // accept connection request from client
        struct sockaddr_in peeraddr;
        socklen_t peeraddr_len;
        int new_sock = accept(sock, (struct sockaddr*) &peeraddr, &peeraddr_len);
        if (new_sock < 0)
        {
            std::cerr << "Error connection: " << strerror(errno) << std::endl;
            exit(1);
        }
        
        char buffer[1024];      // buffer that holds the messages from the client
        int port = PORT;
        cout << "Listening on Port: " << port << endl; 
        while(true){

            // clean the buffer
            memset(&buffer, 0, strlen(buffer));
            // receive messages from the client
            listen_res = recv(new_sock, buffer, 1024, 0);
            if (listen_res == 0)
            {
                close(new_sock);
                break;
            }
            // exit and close the socket if the message is LOCAL
            if (strcmp(buffer, "LOCAL") == 0)
            {
                close(new_sock);
                break;
            }
            // print the message 
            cout << "Server: " << buffer << endl;   
        }   
        cout << "Client disconnected from POrt: " << port << endl; 
    }
    close(sock);
    return 0;
}