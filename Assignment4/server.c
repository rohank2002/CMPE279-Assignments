// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <seccomp.h>
#include <sys/prctl.h> /* prctl */

// #define PORT 8080

 void setsyscalls(){
    prctl(PR_SET_NO_NEW_PRIVS, 1);
    // ensure no escape is possible via ptrace
    prctl(PR_SET_DUMPABLE, 0);

    // Init the filter
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_KILL);

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigreturn), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(chdir), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(chroot), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(send), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(lseek), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(access), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(setsockopt), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(bind), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(prctl), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(accept), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fork), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(setuid), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(listen), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(connect), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clone), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
    

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup2), 2,
                         SCMP_A0(SCMP_CMP_EQ, 1),
                         SCMP_A1(SCMP_CMP_EQ, 2));    


    seccomp_load(ctx);
}
int main(int argc, char const *argv[]) 
{ 
    setsyscalls();
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int PORT = atoi(argv[1]);   //Port from argument
    char* inputfile = argv[2];  // Input file for message to be sent

    int input_file_desc = open(inputfile, O_RDONLY);
    // int addrlen = sizeof(address); 
    // char buffer[1024] = {0}; 
    // char *hello = "Hello from server"; 
       
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

    

    pid_t ProcessID = fork(), id;               
       
    /*
    When the fork() function is called, it returns the process id to the parent and 0 value to child, 
    and if it cannot spawn a new process a -1 value is returned.

    When the fork function is called both the child and parent start executing parallely from the next instruction.
    */
    if(ProcessID==0) {                                        
    
    

        char server_fd_send[5]; // File descriptor to be shared
        char port_send[5]; // Port number to be shared
        sprintf(server_fd_send,"%d",server_fd); // copying values
        sprintf(port_send,"%d",PORT); //copying values
        char input_file_desc_str[5];
        sprintf(input_file_desc_str,"%d",input_file_desc);



        char *args[]={server_fd_send,port_send,input_file_desc_str, NULL}; // NULL terminated array of character pointers referenced from  https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
        execvp("./ServerExec",args);  // Exec function to create a fresh process
        printf("Exec failed!\n");   // Just in case exec fails
    }

    
    
    return 0; 
} 
/*
Instructions for executing the server code:

compile using command 
gcc server.c -o server

run as super user

sudo ./server 8080 hello.txt

This will execute the new process called serverexec.c 

same for the client

The code for run on AWS Ec2 instance, as I have a windows laptop.
*/
