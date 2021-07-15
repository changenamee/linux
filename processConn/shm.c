#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/shm.h>
#include<fcntl.h>
#include<signal.h>
#include<memory.h>
#include<assert.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<sys/stat.h>

#define BUFFER_SIZE 1024
#define FD_LIMIT 65535
#define USERS_LIMIT 5
#define EVENTS_MAX 65535
struct client_data{
    struct sockaddr_in addr;
    int sockfd;
    pid_t pid;  //
    int pipefd[2];
};
static const char* shm = "/my_shm";
int sig_pipefd[2];
int listened;
int epollfd;
int shmfd;
char *shm = 0;
client_data *users=0;
int* sub_process=0;
int user_count=0;
bool stop_child=false;
int sig_pipefd[2];

void del_resource(){
    close(listened);
    close(sig_pipefd[0]);
    close(sig_pipefd[1]);
    close(epollfd);
    shm_unlink(shm);
    delete [] users;
    delete [] sub_process;
}
//设置fd为非阻塞
int setnoblock(int fd){
    int oldop = fcntl(fd,F_GETFL);
    int newop = oldop|O_NONBLOCK;
    fcntl(fd,F_SETFL,newop);
    return oldop;
}
void addfd(int epollfd,int fd){
    struct epoll_event event;
    event.data.fd=fd;
    event.events = EPOLLIN|EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnoblock(fd);
}
//信号处理函数
void sig_handler(int sig){
    int old_errno = errno;
    int msg=sig;
    send(sig_pipefd[1],(char*)&msg,1,0);
    errno = old_errno;
}
//处理信号
void addsig(int sig,void (*sig_handler)(int),bool restart=true){
    struct sigaction sa;
    memset(sa,'\0',sizeof(sa));
    sa.sa_handler = sig_handler;
    if(restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL)!=-1);
}
//停止一个子进程
void child_handler(int sig){

    stop_child = true;
}

int run_child(int idx,client_data* users,char* share_mem){
    struct epoll_event events[EVENTS_MAX];
    int connfd = users[idx].sockfd;
    int child_epollfd = epoll_create(5);
    addfd(child_epollfd,connfd);
    int pipefd = users[idx].pipefd[1];
    addfd(child_epollfd,pipefd); 
    int ret;
    addsig(SIGTERM,&child_handler,false);
    while(!stop_child){
        int num = epoll_wait(epollfd,events,EVENTS_MAX-1,0);
        if((num<0) && (errno!=EINTR)){
            printf("epoll failed\n");
            break;
        }
        for(int i=0;i<num;++i){
            int sockfd = events[i].data.fd;
            if(sockfd==connfd && (events[i].events&EPOLLIN)){
                memset(share_mem+idx*BUFFER_SIZE,'\0',BUFFER_SIZE);
                ret = recv(connfd,share_mem+idx*BUFFER_SIZE,BUFFER_SIZE-1,0);
                if(ret<0){
                    if(errno!=EAGAIN){
                        stop_child = true;
                    }
                }else if(ret==0){
                    stop_child=true;
                }else{
                    send(pipefd,(char*)&idx,sizeof(idx),0);
                }
            }else if(sockfd == pipefd && (events[idx].events&EPOLLIN)){
                int client;
                ret = recv(pipefd,(char*)&client,sizeof(client),0);
                if(ret<0){
                    if(errno!=EAGAIN){
                        stop_child = true;
                    }
                }else if(ret==0){
                    stop_child = true;
                }else{
                    send(connfd,share_mem+client*BUFFER_SIZE,BUFFER_SIZE,0);
                }
            }else   continue;
        }
    }
}