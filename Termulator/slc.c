#include <sys/types>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char* argv[]){
    int fd_a, fd_b, fd_c;

    int nfd;
    fd_set read_fds;
    struct timeval tv;

    signal(SIGINT,trataCC);
    
    mkfifo("pipe_a", 00777);
    mkfifo("pipe_b", 00777);

    fd_a=open("pipe_a", O_RDWR | O_NONBLOCK);
    if(fd_a==-1)
        printf("ERRO OPEN PIPE A");
    fd_b=open("pipe_b", O_RDWR | O_NONBLOCK);
    if(fd_b==-1)
        printf("ERRO OPEN PIPE B");

    while(1){
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        
        FD_ZERO( & read_fds);
        FD_SET(0, & read_fds); //stdin
        FD_SET(fd_a, & read_fds); //stdin
        FD_SET(fd_b, & read_fds); //stdin
        
        nfd=select(
            max(fd_a, fd_b)+1, &read_fds, NULL, NULL, &tv);
        if(nfd==0){
            printf("\nwaiting...");
            fflush(stfout);
            continue;
        }
        if(nfd==-1){
            printf("\nERRO...");
            fflush(stfout);
            continue;
        }
        
        if(FD_ISSET(0, &read_fds)){
            KB
        }
        if(FD_ISSET)

    }
            
}   