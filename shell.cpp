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
using namespace std;

void server_run(){
    // start the server
    int listen_port = 4093; // port number
    //start a scoket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // assert the socket is valid
    if (sock < 0){
        cerr << "Error in socket: " << strerror(errno) << endl;
        return;    
    }

    // inititlize the server attributes
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in)); 
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

    

}

void client_run(){

}

int main(){

    string command; // a variable to save the user command
    char buff[256];

    while (true)
    {
        // ask the user for the next command 
        cout << "Please enter your next Command" << endl;
        // save the input as the next command
        getline(cin, command);

        if(command.compare("EXIT") == 0){
            // if the user typed in EXIT, break the loop and stop the program
            break;
        }

        else if(command.compare("getcwd") == 0){
            // present the files in the current directory
            getcwd(buff, 256);  // return the current working directory and stores it in the buff array
            cout << "Curent Working Directory is:  " << buff << endl;
        }

        else if (command.find("ECHO", 0) == 0){
            // ECHO command, print the rest of the string after the ECHO 
            command.erase(0,4);     //erase the ECHO from the command 
            cout << command << endl;    // print the string
        }
        
        else if (command.compare("TCP PORT") == 0){
            // TCP PORT command, open a TCP server and a client 

            thread thread_obj(server_run);  // run the server as a thread
            thread thread_obj(client_run);  // run the client as a thread
            cout << "Server and Client are both ready on the localhost" << endl;

        }


    }

    cout << "Thank you for using Alon Barak Shell" << endl;
    








    return 0;
}