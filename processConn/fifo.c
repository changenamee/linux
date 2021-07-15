#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<limits.h>

#define FIFO_NAME "my_fifo"
#define BUFFER_SIZE PIPE_BUF

int main(void){
    int pipe_fd;
    int res;
    int open_mode = O_RDONLY;
    int bytes_read = 0;
    char message[BUFFER_SIZE+1];
    memset(message,'\0',sizeof(message));
    printf("process %d opening fifo O_RDONLY\n",getpid());
    pipe_fd = open(FIFO_NAME,open_mode);
    printf("process %d result %d\n",getpid(),pipe_fd);
    if(pipe_fd!=-1){
        do{
            res = read(pipe_fd,message,BUFFER_SIZE);
            bytes_read+=res;
        }while(res>0);
        (void)close(pipe_fd);
    }else exit(EXIT_FAILURE);
    printf("process %d read %d bytes.",getpid(),bytes_read);
    exit(EXIT_SUCCESS);
}