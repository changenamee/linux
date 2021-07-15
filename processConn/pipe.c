#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main(void){
    int pipefd[2];
    pid_t pid;  
    int status;
    int ret = pipe(pipefd);
    if(ret==-1){
        perror("fail to pipe");
    }

    pid = fork();
    if(pid==0){
        //child
        close(pipefd[0]);
        char  *buf = "child..";
        int i=10;
        while(i>=0){
            write(pipefd[1],buf,strlen(buf)+1);
            sleep(2);
            --i;
        }
    }else if(pid>0){
        //parent
        close(pipefd[1]);
        char buf[1024];
        int i=5;
        memset(buf,'\0',sizeof(buf));
        while(i>=0){
            int len = read(pipefd[0],buf,sizeof(buf));
            if(len>0){
                buf[len-1] = '\0';
                printf("read:%s\n",buf);
            }
            --i;
        }
        close(pipefd[0]);
        waitpid(pid,&status,0);
        printf("pid status:%d",status);
    }else{
        perror("fail to fork");
    }
    return 0;
}