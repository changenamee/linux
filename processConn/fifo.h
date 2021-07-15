#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<limits.h>
#include<sys/types.h>
#include<sys/stat.h>

#define SERVER_FIFO_NAME "/tmp/server_fifo"
#define CLIENT_FIFO_NAME "/tmp/client_%d_fifo"

#define BUFFER_SIZE 20
typedef struct data{
    pid_t child_pid;
    char some_data[BUFFER_SIZE+1];
}data_to_pass;