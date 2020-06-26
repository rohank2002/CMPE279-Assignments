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

int main(int argc, char  *args[]) 
{   
    printf("Executing execv created process.....\n");
    int server_fd, new_socket, valread;
    server_fd=atoi(args[0]);
    int PORT = atoi(args[1]);
    int file_desc = atoi(args[2]);

    char* hello;
	struct passwd *pwd = getpwnam("nobody");
    uid_t UserId = pwd->pw_uid;



    // //chroot
    // chdir("/home/ubuntu/Assignment3/lockdown");
    // chroot("/home/ubuntu/Assignment3/lockdown");
    if (chdir("/home/ubuntu/Assignment4/lockdown")==-1){
        printf("Could'nt change directory\n");
    }
    
    if(chroot("/home/ubuntu/Assignment4/lockdown")==-1){
        printf("Couldn't chroot\n");
    }

        if (setuid(UserId) < 0) {                      //setuid returns 0 on success and -1 on failure
            printf("Failed to set ID \n");
            exit(EXIT_FAILURE);
        }
        else {

            printf("Process associated with id  %ld\n", (long)UserId);
        }
	

    read(file_desc,hello, 1024); 
    printf("%s\n",hello );

    struct sockaddr_in address; 
    // int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    // char *hello = "Hello from server"; 
       
    
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
       
    
    

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                               (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
           
        valread = read( new_socket , buffer, 1024);

        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
    return 0; 
}

