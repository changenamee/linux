#ifndef COMM_H
#define COMM_H
#include<stdio.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<errno.h>
#define PATHNAME "."
#define PROJID 0

union semnum{
    int val;
    struct semid_ds *buf;
    unsigned short  *arr;
    struct seminfo*_buf;
};
//创建/获取信号量
static int CommSemid(int nums,int flag);
//创建信号量
int CreateSemid(int nums);
//获取信号量
int GetSemid(int nums);
//初始化信号量
int InitSemid(int semid,int which_,int val);
//操作信号量
static int SemPV(int semid,int which,int op);
//P
int P(int semid,int which,int op);
//V
int V(int semid,int which,int op);
//信号量生命周期随内核，所以要销毁
int Destory(int semid);

//创建共享内存
int CreatShm(int size);
//获得贡献内存
int GetShm(int size);
//销毁
int DestoryShm(int shmid);
#endif
