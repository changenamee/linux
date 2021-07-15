#ifndef LST_TIMER
#define LST_TIMER
#include<iostream>
#include<time.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define BUFSIZ 1024
using namespace std;
class util_timer;

struct client_data{
    sockaddr_in clientaddr;
    int sockfd;
    char buf[BUFSIZ];
    util_timer* timer;  //定时器    
};
//定时器类
class util_timer{
public:
    util_timer():prev(nullptr),next(nullptr){}
public:
    time_t expire; //超时时间，绝对时间
    //双向链表s`    
    util_timer* prev;
    util_timer* next;
    client_data *data;
    void (*cb_func)(client_data* data);
};

//定时器类链表
class sort_timer_list{
public:
    sort_timer_list():head(nullptr),tail(nullptr){}
    ~sort_timer_list(){
        util_timer* tmp = head;
        while(tmp){
            head = tmp->next;
            head->prev = nullptr;
            tmp->next = nullptr;
            delete tmp;
            tmp = head;
        }
    }
    void add_timer(util_timer* timer){
        if(!timer)  return;

        if(!head){
            head = tail = timer;
            return;
        }

        if(timer->expire<head->expire){
            timer->next = head;
            head->prev = timer;
            head = timer;
            return;
        }
        add_timer(timer,head);
    }
    //超时时间增加后调整timer在链表中的位置
    void adjust_timer(util_timer* timer){
        if(!timer)  return;

        if(timer->next==nullptr || (timer->expire<=timer->next->expire))
            return;
        
        if(timer==head){
            head=head->next;
            head->prev = nullptr;
            timer->next = nullptr;
            add_timer(timer,head);
        }else{
            timer->next->prev = timer->prev;
            timer->prev->next = timer->next;
            add_timer(timer,timer->next);
        }
    }
    void del_timer(util_timer* timer){
        if(!timer) return;
        if(timer==head && timer==tail){
            delete timer;
            head=nullptr;
            tail = nullptr;
            return;
        }
        if(timer==head){
            head=head->next;
            head->prev=nullptr;
            timer->next=nullptr;
            delete timer;
            return;
        }
        if(timer==tail){
            tail=tail->prev;
            tail->next=nullptr;
            timer->prev = nullptr;
            delete timer;
            return;
        }
        timer->next->prev=nullptr;
        timer->prev->next=nullptr;
        delete timer;
    }
    void tick(){
        if(!head)   return;
        util_timer* tmp = head;
        time_t cur = time(NULL);
        printf("tick once\n");
        while(tmp){
            if(cur<tmp->expire) break;
            tmp->cb_func(tmp->data);
            head = tmp->next;
            if(head){
                head->prev=nullptr;
            }
            delete tmp;
            tmp=head;
        }
    }
private:
    util_timer* head;
    util_timer* tail;
    void add_timer(util_timer* timer,util_timer* after){
        util_timer* cur=after->next;
        util_timer* prev=after;
        while(cur){
            if(timer->expire<=cur->expire){
                prev->next=timer;
                timer->prev=prev;
                timer->next=cur;
                cur->prev=timer;
                break;
            }
            prev=cur;
            cur=cur->next;
        }
        if(!cur){
            prev->next=timer;
            timer->prev=prev;
            timer->next=nullptr;
            tail=timer;
        }
    }
};

#endif