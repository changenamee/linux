#include "comm.h"
#include<unistd.h>

int main(void){
    int semid = CreateSemid(1);
    printf("semid:%d\n",semid);
    InitSemid(semid,0,1);
    pid_t pid = fork();
    if(pid==0){
        //child
        int semid = GetSemid(0);
        while(1){
            P(semid,0,-1);
            printf("A");
            fflush(stdout);
            usleep(10000);
            printf("A");
            fflush(stdout);
            usleep(20000);
            V(semid,0,1);
        }
    }else if(pid>0){
        //parent
        while(1){
            P(semid,0,-1);
            usleep(30000);
            printf("B");
            fflush(stdout);
            usleep(8000);
            printf("B");
            fflush(stdout);
            usleep(20000);
            V(semid,0,1);
        }
        waitpid(pid,0,0);
    }else{
        perror("fork");
        return -1;
    }
    Destory(semid);
    return 0;
}