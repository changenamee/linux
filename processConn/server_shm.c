#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>
#include<errno.h>
#include<assert.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<signal.h>

#define MAXBUF 1024
int main(void){
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t cli_addr_len;
    char buf[1024];
    int cfd;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(9527);
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    int ret = bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    //if(ret!=)
    ret = listen(listenfd,8);
    cli_addr_len = sizeof(cli_addr);
    cfd = accept(listenfd,(struct sockaddr*)&cli_addr,&cli_addr_len);
    ret = read(cfd,buf,sizeof(buf));
    while(ret>0){
        for(int i=0;i<strlen(buf);++i)
            buf[i] = toupper(buf[i]);
        write(cfd,buf,strlen(buf));
        ret = read(cfd,buf,sizeof(buf));
    }
    close(cfd);
    return 0;
}
