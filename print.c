/*
 *说明：使用信号量实现进程互斥示例；
 *算法思路：1,设置信号量计数值为1；
 *	    2,父进程使用P操作获取信号量，打印字符‘O’
 *	    进入睡眠，再打印字符'O',最后使用V操作释放信号量。
 *	    3,子进程和父进程进行同样的操作打印字符'X';
 *	    4,程序运行时我们可以观察到字符'O','X'是成对打印输出的。
 */
#include<stdio.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>


union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

//semget()创建或者打开一个信号量的函数
//semctl()控制信号量集

int sem_create(key_t key)
{
	int semid;
	semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if(semid == -1)
		ERR_EXIT("semget");
	return semid;
}

int sem_open(key_t key)
{
	int semid;
	semid = semget(key, 0, 0);
	if(semid == -1)
		ERR_EXIT("semget");
	return semid;
}

//设置信号量集计数值
int sem_setval(int semid, int val)
{
	union semun su;
	su.val = val;
	int ret;
	ret = semctl(semid, 0, SETVAL, su);
	if(ret == -1)
		ERR_EXIT("sem_setval");
	return ret;
}

//获取信号量集计数值
int sem_getval(int semid)
{
	int ret;
	ret =semctl(semid, 0, GETVAL, 0);
	if(ret == -1)
		ERR_EXIT("sem_getval");
	return ret;
}
//删除
int sem_d(int semid)
{
	int ret;
	ret = semctl(semid, 0, IPC_RMID, 0);
	if(ret == -1)
		ERR_EXIT("semctl");
	return ret;
}

int sem_p(int semid)
{
	struct sembuf sb = {0, -1, 0};
	int ret;
	ret = semop(semid, &sb, 1);
	if(ret == -1)
		ERR_EXIT("semop");
	return ret;
}

int sem_v(int semid)
{
        struct sembuf sb = {0,1,0};
        int ret;
        ret = semop(semid, &sb, 1);
        if(ret == 1)
                ERR_EXIT("semop");
        return ret;
}

int semid;

void print(char op_char)
{
	int pause_time;
	srand(getpid());
	int i;
	for(i=0; i<10; i++)
	{
		sem_p(semid);
		printf("%c",op_char);
		fflush(stdout);
		pause_time = rand() % 3;
		sleep(pause_time);
		printf("%c",op_char);
		fflush(stdout);
		sem_v(semid);
		pause_time = rand() % 2;
		sleep(pause_time);
	}
}



int main()
{
	semid = sem_create(IPC_PRIVATE);
	sem_setval(semid,0);
	pid_t pid;
	pid = fork();
	if(pid == -1)
		ERR_EXIT("fork");
	if(pid > 0)
	{
		sem_setval(semid,1);
		print('O');
		wait(NULL);
		sem_d(semid);
	}
	else
	{
		print('X');
	}
	return 0;
}
