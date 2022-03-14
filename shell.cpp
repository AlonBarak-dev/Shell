#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
using namespace std;



int main(){

    string command; // a variable to save the user command
    char buff[256];

    while (true)
    {
        // ask the user for the next command 
        cout << "Please enter your next Command" << endl;
        // save the input as the next command
        getline(cin, command);

        cout << command << endl;
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
        
        //else if ()


    }

    cout << "Thank you for using Alon Barak Shell" << endl;
    








    return 0;
}