// Thanks to https://beej.us/guide/bgipc/html/multi/semaphores.html

#include <iostream>
#include <stdexcept>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include "user_driver.h"

namespace user_driver
{

const int MAX_RETRIES = 10;
const uint64_t FAKE_PID = 3544952156018063160ull;

class ScopedSemaphore
{
private:
	union semun {
    	int val;
	    struct semid_ds *buf;
	    ushort *array;
	};

public:
	ScopedSemaphore()
	{
		sembuf sem_buf;
		semid_ds buf;
		semun arg;
		int ret;

		key_t semkey = ftok("/dev/simple_driver", 0xff);
		semid = semget(semkey, 1, IPC_CREAT | IPC_EXCL | 0666);
		if(semid >= 0)
		{
			sem_buf.sem_num = 0;
			sem_buf.sem_flg = 0;
			sem_buf.sem_op = 1;
			arg.val = 1;

			ret = semop(semid, &sem_buf, 1);
			if(ret == -1)
			{
				semctl(semid, 0, IPC_RMID); /* clean up */
				throw std::runtime_error("Failed semop with semid >= 0\n");
			}
		}
		else if(errno == EEXIST)
		{
			bool ready = false;

			semid = semget(semkey, 1, 0);
			if(semid < 0)
				throw std::runtime_error("semid < 0 again\n");

			arg.buf = &buf;
			for(int i = 0; i < MAX_RETRIES && !ready; ++i)
			{
				semctl(semid, 0, IPC_STAT, arg);
				if(arg.buf->sem_otime != 0)
					ready = true;
				else
					sleep(1);
			}
			if(!ready)
				throw std::runtime_error("Max retries reached\n");
		}
		else
		{
			throw std::runtime_error("Failed semget\n");
		}

		sem_buf.sem_num = 0;
		sem_buf.sem_flg = SEM_UNDO;
		sem_buf.sem_op = -1; /* Allocating the resources */
		ret = semop(semid, &sem_buf, 1);
		if(ret == -1)
			throw std::runtime_error("Failed lock operation\n");

//		std::cout << "LOCKED" << std::endl;
	}
	~ScopedSemaphore()
	{
		sembuf sem_buf;
		sem_buf.sem_num = 0;
		sem_buf.sem_flg = SEM_UNDO;
		sem_buf.sem_op = 1; /* Releasing the resource */
		semop(semid, &sem_buf, 1);

//		std::cout << "UNLOCKED" << std::endl;
	}

private:
	int semid;
};

void reserve(accid_t acc_id)
{
	uint64_t stored_pid;
	uint64_t pid = getpid();

	ScopedSemaphore sem;
	FILE* fp = fopen("/dev/simple_driver", "rb+");
	fread(&stored_pid, sizeof(uint64_t), 1, fp);
	if(stored_pid == FAKE_PID) // can reserve
	{
		fwrite(&pid, sizeof(uint64_t), 1, fp);
	}
	fclose(fp);
}

bool check_reserve(accid_t acc_id)
{
	uint64_t stored_pid;
	uint64_t pid = getpid();

	ScopedSemaphore sem;
	FILE* fp = fopen("/dev/simple_driver", "rb+");
	fread(&stored_pid, sizeof(uint64_t), 1, fp);
	fclose(fp);

	return (pid == stored_pid);
	return false;
}

void release(accid_t acc_id)
{
	uint64_t stored_pid;
	uint64_t pid = getpid();

	ScopedSemaphore sem;
	FILE* fp = fopen("/dev/simple_driver", "rb+");
	fread(&stored_pid, sizeof(uint64_t), 1, fp);
	if(stored_pid == pid) // can release
	{
		fwrite(&FAKE_PID, sizeof(uint64_t), 1, fp);
	}
	fclose(fp);
}

}
