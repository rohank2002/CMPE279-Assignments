// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#define PORT 8080
int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    struct passwd *pwd = getpwnam("nobody");
    uid_t UserId = pwd->pw_uid;

    pid_t ProcessID = fork(), id;               
    int flag = 0;    
    /*
    When the fork() function is called, it returns the process id to the parent and 0 value to child, 
    and if it cannot spawn a new process a -1 value is returned.

    When the fork function is called both the child and parent start executing parallely from the next instruction.
    */
    if(ProcessID==0) {                                        


        if (setuid(UserId) < 0) {                      //setuid returns 0 on success and -1 on failure
            printf("Failed to set ID \n");
            exit(EXIT_FAILURE);
        }
        else {

            printf("Process associated with id  %ld\n", (long)UserId);
        }

        char server_fd_send[5]; // File descriptor to be shared
        char port_send[5]; // Port number to be shared
        sprintf(server_fd_send,"%d",server_fd); // copying values
        sprintf(port_send,"%d",PORT); //copying values


        char *args[]={server_fd_send,port_send, NULL}; // NULL terminated array of character pointers referenced from  https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
        execv("./ServerExec",args);  // Exec function to create a fresh process
        printf("Exec failed!\n");   // Just in case exec fails
    }

    
    
    return 0; 
} 
/*
Instructions for executing the server code:

compile using command 
gcc server.c -o server

run as super user

sudo ./server

This will execute the new process called serverexec.c 

same for the client

The code for run on AWS Ec2 instance, as I have a windows laptop.
*/
