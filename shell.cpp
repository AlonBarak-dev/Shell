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
using namespace std;

// inspired from : https://www.technical-recipes.com/2014/getting-started-with-client-server-applications-in-c/

void server_run(){
    // start the server
    int listen_port = 55002; // port number
    //start a scoket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // assert the socket is valid
    if (sock < 0){
        cerr << "Error in socket: " << strerror(errno) << endl;
        return;    
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
        return;
    }

    
    // listen for connection from client
    int listen_res = listen(sock, 10);
    if (listen_res < 0)
    {
        std::cerr << "Error listen: " << strerror(errno) << std::endl;
        return;
    }

    // accept connection request from client
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len;
    int new_sock = accept(sock, (struct sockaddr*) &peeraddr, &peeraddr_len);
    if (new_sock < 0)
    {
        std::cerr << "Error connection: " << strerror(errno) << std::endl;
        return;
    }

    
    char buffer[1024];
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
    close(sock);
    return; 

}


// inspired from : https://www.technical-recipes.com/2014/getting-started-with-client-server-applications-in-c/

int client_run(){
    //start the client
    // create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return -1;
    }

    // Fill in server IP address
    struct sockaddr_in server;
    int serverAddrLen;
    bzero( &server, sizeof( server ) );

    
    // set the server address and port
    server.sin_family = AF_INET;
    int peerPort = 55002;  //port number
    server.sin_port = htons(peerPort);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

 
    // Connect to the remote server
    int res = connect(sock, (struct sockaddr*) &server, sizeof(server));
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return -1;
    }
    // return the socket number
    return sock;

}

int main(){

    string command; // a variable to save the user command
    char buff[256];
    int sock;   // TCP socket 
    bool connected_to_server = false;
    const char* command_c;
    thread thread_obj(server_run);  // run the server as a thread

    while (true)
    {
        // ask the user for the next command 
        cout << "Please enter your next Command" << endl;
        // save the input as the next command
        getline(cin, command);
        // convert the string to a C string
        if(connected_to_server){
            command_c = command.c_str();
        }

        if(command.compare("EXIT") == 0){
            // if the user typed in EXIT, break the loop and stop the program
            if (connected_to_server)
            {
                write(sock, "LOCAL", 5);    // close server
                sleep(1);
                close(sock);    // close socket
            }
            
            break;      // end program
        }

        else if(command.compare("getcwd") == 0){
            // present the files in the current directory
            getcwd(buff, 256);  // return the current working directory and stores it in the buff array
            if(!connected_to_server){
                cout << "Curent Working Directory is:  " << buff << "\n" << endl;
            }
            else{
                write(sock, buff, 300);   // send the content to the server
            }
        }

        else if (command.find("ECHO", 0) == 0){
            // ECHO command, print the rest of the string after the ECHO 
            command.erase(0,4);     //erase the ECHO from the command 
            if(!connected_to_server){
                cout << command << endl;    // print the string
            }
            else{
                write(sock, command_c, command.length());   // send the content to the server
            }
        }
        
        else if (command.compare("TCP PORT") == 0){
            // TCP PORT command, open a client 
            sock = client_run();
            if(sock < 0){
                cout << "Error occured" << endl;
            }
            cout << "Server and Client are both ready on the localhost" << endl;
            connected_to_server = true;
        }

        else if (command.compare("LOCAL") == 0){
            // LOCAL command, the user switches back to standart output
            if (!connected_to_server){
                // in case the user is not connected to a server
                cout << "Already on Standart Output!" << endl;
            }
            else{
                // close the server socket
                write(sock, "LOCAL", 5);
                // close the user socket
                close(sock);
                connected_to_server = false;
            }
        }
        
        else if (command.compare("DIR") == 0){
            // DIR command, prints the list of files in the current directory
            
            
        }
        


    }

    cout << "Thank you for using Alon Barak Shell" << endl;
    








    return 0;
}