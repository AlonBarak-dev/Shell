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
#include <sys/wait.h>

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
    int saved_fd;
    // thread thread_obj(server_run);  // run the server as a thread

    while (true)
    {
        // ask the user for the next command 
        std::cout << "Please enter your next Command" << endl;
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
            std::cout << "Curent Working Directory is:  " << buff << "\n" << endl;
            
        }

        else if (command.find("ECHO", 0) == 0){
            // ECHO command, print the rest of the string after the ECHO 
            

            command.erase(0,5);     //erase the ECHO from the command 
            std::cout << command << endl;    // print the string
            
        }
        
        else if (command.compare("TCP PORT") == 0){
            // TCP PORT command, open a client 


            sock = client_run();
            if(sock < 0){
                std::cout << "Error occured" << endl;
            }
            else{
                std::cout << "Server and Client are both ready on the localhost" << endl;
                saved_fd = dup(1);
                close(1);
                dup2(sock, 1);
                connected_to_server = true;
            }
        }

        else if (command.compare("LOCAL") == 0){
            // LOCAL command, the user switches back to standart output
            if (!connected_to_server){
                // in case the user is not connected to a server
                std::cout << "Already on Standart Output!" << endl;
            }
            else{
                // close the server socket
                write(sock, "LOCAL", 5);
                // close the user socket
                close(1);
                dup2(saved_fd, 1);
                close(sock);
                connected_to_server = false;
            }
        }
        
        else if (command.compare("DIR") == 0){

            // DIR command, prints the list of files in the current directory
            
            DIR* cur_dir = opendir(".");    // open the current directory
            struct dirent* db;
            while((db = readdir(cur_dir)) != NULL){

                // print the files on the standart output
                std::cout << db->d_name << endl;
            }
            (void)closedir(cur_dir);    // close the directory
            sleep(1);
        }

        else if (command.find("CD", 0) == 0){


            // CD command, move to the folder that appear after the /
            // chdir() is a system call.
            command.erase(0,3);     // extract the name of the folder 
            int res = chdir(command.c_str());
            
            
            std::cout << command << ": No such file/directory!" << endl;
        }
        // THIS IS THE METHOD OF QUES. 10- IDAN
        else if (command.find("COPY", 0) == 0)
        {
            // copy files from source folder into dest folders using fopen, fwrite, fread
            char cmd[command.size() + 1];
            strcpy(cmd, command_c);
            char *token = strtok(cmd, " ");
            // EXTRACT the src and dst paths
            string src, dst;
            int i = 0;
            while (token != NULL)
            {
                if (i == 1)
                {
                    src = token;
                    std::cout << src << endl;
                }
                if(i == 2){
                    dst = token;
                    std::cout << dst << endl;
                }
                i++;
                token = strtok(NULL, " ");
            }

            // open the files located at src and dst and read the src file
            FILE * src_file = fopen(src.c_str(), "rb");
            FILE * dst_file = fopen(dst.c_str(), "ab+");

            if(src_file == NULL) {std::cout << "Source file not exist!" << endl;continue;}    // file not exist
            if(dst_file == NULL) {std::cout << "Destination file not exist!" << endl;continue;}    // file not exist

            fseek(src_file,0,SEEK_END);
            char * cur = (char*)malloc(sizeof(char)*ftell(src_file));
            fseek(src_file,0,SEEK_SET);
            fread(&cur, sizeof(cur), 1, src_file);

            fwrite(&cur, sizeof(cur), 1, dst_file);

            
            // close the files 
            fclose(src_file);
            fclose(dst_file);
            free(cur);
            

        }
        
        else{
            // implement rest of the commands using the system function
            system(command_c);

            // // fork, exec and wait implementation
            // pid_t pid = fork(); // create a new process
            // if (pid < 0)
            // {
            //     cout << "fork failed" << endl;
            // }
            
            // if(pid == 0){
            //     // child process 
            //     char cmd[command.size() + 1];
            //     strcpy(cmd, command_c);
            //     char * argv[] = {cmd, NULL};
            //     // run the command
            //     execvp(argv[0], argv);
            //     exit(127);
                
            // }
            // else{
            //     //parent process
            //     int status;
            //     while (waitpid(pid, &status, 0) == -1)
            //     {
            //         if (errno != EINTR)
            //         {
            //             status = -1;
            //             break;
            //         }   
            //     }   
            // }
        }


    }

    std::cout << "Thank you for using Shell" << endl;
    
    








    return 0;
}