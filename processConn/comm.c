#include "comm.h"

static int CommSemid(int nums,int flag){
    key_t key = ftok(PATHNAME,PROJID);
    if(key>0){
        //key:sem标识符;nums:信号量个数，一般是1（互斥量）;flag:创建标志，IPC_CREAT
        return semget(key,nums,flag);
    }else{
        perror("fail to CommSemid");
        return -1;
    }
}

int CreateSemid(int nums){
    return CommSemid(nums,IPC_CREAT|IPC_EXCL|0666);
}

int GetSemid(int nums){
    return CommSemid(nums,IPC_CREAT);
}

int InitSemid(int semid,int which,int val){
    union semnum _semnum;
    _semnum.val = 1;
    int ret = semctl(semid,which,SETVAL,_semnum);
    if(ret!=0){
        perror("fail to init sem");
        return -1;
    }
    return 0;
}

static int SemPV(int semid,int which,int op){
    struct sembuf _sf;
    _sf.sem_num = which;
    _sf.sem_op = op;
    _sf.sem_flg = SEM_UNDO; //若没有释放信号量而进程终止，则操作系统释放该信号量
    return semop(semid,&_sf,1);
}

int P(int semid,int which,int op){
    return SemPV(semid,which,op);
}

int V(int semid,int which,int op){
    return SemPV(semid,which,op);
}

int Destory(int semid){
    return semctl(semid,0,IPC_RMID);
}


static int CommShm(int size,int flag){
    key_t key = ftok("..",1);
    if(key<0){
        //error
        return -1;
    }else{
        return shmget(key,size,flag);
    }
}

int CreatShm(int size){
    return CommShm(size,IPC_CREAT|IPC_EXCL|0666);
}
int GetShm(int size){
    return CommShm(size,IPC_CREAT);
}
int DestoryShm(int shmid){
    return shmctl(shmid,0,IPC_RMID);
}




