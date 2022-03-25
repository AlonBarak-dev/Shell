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

int client_run(){
    // start the client
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
    int peerPort = PORT;  //port number
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
    // thread thread_obj(server_run);  // run the server as a thread

    while (true)
    {
        // ask the user for the next command 
        cout << "Please enter your next Command" << endl;
        // save the input as the next command
        getline(cin, command);
        // convert the string to a C string
        command_c = command.c_str();
        

        if(command.compare("EXIT") == 0){
            // if the user typed in EXIT, break the loop and stop the program

            if (connected_to_server)
            {
                write(sock, "EXIT", sizeof("EXIT"));    // close server
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
                sleep(1);
            }
        }

        else if (command.find("ECHO", 0) == 0){
            // ECHO command, print the rest of the string after the ECHO 
            

            command.erase(0,5);     //erase the ECHO from the command 
            if(!connected_to_server){
                cout << command << endl;    // print the string
            }
            else{
                write(sock, command_c, command.length());   // send the content to the server
                sleep(1);
            }
        }
        
        else if (command.compare("TCP PORT") == 0){
            // TCP PORT command, open a client 


            sock = client_run();
            if(sock < 0){
                cout << "Error occured" << endl;
            }
            else{
                cout << "Server and Client are both ready on the localhost" << endl;
                connected_to_server = true;
            }
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
            
            DIR* cur_dir = opendir(".");    // open the current directory
            struct dirent* db;
            while((db = readdir(cur_dir)) != NULL){

                if (!connected_to_server)
                {
                    // print the files on the standart output
                    cout << db->d_name << endl;
                }
                else{
                    // send the name of the file to the server
                    write(sock, db->d_name ,sizeof(db->d_name));
                }
            }
            (void)closedir(cur_dir);    // close the directory
            sleep(1);
        }

        else if (command.find("CD", 0) == 0){


            // CD command, move to the folder that appear after the /
            // chdir() is a system call.
            command.erase(0,3);     // extract the name of the folder 
            int res = chdir(command.c_str());
            
            // folder doesnt exists
            if (res < 0)
            {
                cout << command << ": No such file/directory!" << endl;
            }
            else{
                cout << "Moved to :" << command << endl;
            }
        }
        else{
            // implement rest of the commands using the system function
            // system(command_c);

            pid_t pid = fork(); // create a new process
            if (pid < 0)
            {
                cout << "fork failed" << endl;
            }
            
            if(pid == 0){
                // child process
                char * argv[2]; argv[0] = NULL;
                // run the command
                execve(command_c, argv, NULL);

                exit(0);
            }
            else{
                // parent process
                // wait(NULL);
            }



        }

    }

    cout << "Thank you for using Alon Barak Shell" << endl;
    
    








    return 0;
}