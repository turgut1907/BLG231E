// Turgut Can Aydinalev - 150120021 - OS - HW3
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <errno.h>
#define SIZE 300
#define STAMEM 77
#define QUEUEMEM 155
#define QCOUNT 133
#define SEM_QCOUNT 112
#define SEM_STAMEM 1900
#define SEM_QUEUEMEM 1907
#define SEM_SYNC 1111;

void sem_up(int semid, int val);
void sem_down(int semid,int val);
int sem_id,sem_idq,sem_sync, sem_idqc, shm_id, shm_idq;

int main(int argc, char *argv[]) {
	fflush(stdout);
	int childs[SIZE]; // childs ids
	int app_count = atoi(argv[1]);	// applicant count from user
	int f,i;

	//semaphore for status mem
	key_t keysem = SEM_STAMEM; // key create for sem
	int sem_id = semget(keysem,1,0700|IPC_CREAT); // Status memory control semaphore
	semctl(sem_id,0,SETVAL,1); // set value to 1

	keysem = SEM_SYNC; // sync with trial and hr
	int sem_sync = semget(keysem,1,0700|IPC_CREAT);
	semctl(sem_sync,0,SETVAL,0);

	//shared memory for status
	key_t keyshm = STAMEM;
	long int segsize = sizeof(int) * app_count;
	int shm_id = shmget(keyshm, segsize, 0700|IPC_CREAT);
	shm_id = shmget(keyshm,sizeof(int),0);
	int *shm_ptr;
	shm_ptr = (int *) shmat(shm_id,0,0);

	key_t keysem2 = SEM_QUEUEMEM; // key create for sem shared queue
	sem_idq = semget(keysem2,1,0700|IPC_CREAT); // Status memory control semaphore
	semctl(sem_idq,0,SETVAL,1); // set value to 1

	key_t keysem3 = SEM_QCOUNT; // key create for sem queue count
	sem_idqc = semget(keysem3,1,0700|IPC_CREAT); // Status memory control semaphore
	semctl(sem_idqc,0,SETVAL,0); // set value to 0. trial.h will wait the flag

	keyshm = QUEUEMEM; // queue shared memory
	segsize = sizeof(int) * SIZE; // fixed size for queue
	shm_idq = shmget(keyshm, segsize, 0700|IPC_CREAT);
	shm_idq = shmget(keyshm,sizeof(int),0);
	int *p = (int *) shmat(shm_idq,0,0);
	p[0] = 0; // first element is the counter of the queue
	shmdt(p);

	for (i=0;i<app_count; i++) { // applicant process create
		f = fork();
		if (f > 0) {
			childs[i] = f;
			sem_down(sem_id,1);
			shm_ptr[i] = 0; // set status
			sem_up(sem_id,1);
		}
		else if (f==0) { // applicant processes
			sleep(1);
			break;
		}
		else {
			printf("hata\n");
			break;
		}
	} // end for
	if (f == 0) { // child process
		while(1) {
			if (shm_ptr[i] == 1 || shm_ptr[i] == 3 || shm_ptr[i] == 4) break;
			else sleep(1);
		}
		exit(1); // applicant is leaving the system
	}
	else { // main process
		int pid;
		while(pid = waitpid(-1,NULL,0)) {
			if (errno == ECHILD) break;
		}
		shmdt(shm_ptr);
		// killing all semaphores and shared memories
		key_t key = SEM_STAMEM;
		sem_id = semget(key,1,0);
		if (sem_id != -1)semctl(sem_id,0,IPC_RMID,0);

		key = SEM_QUEUEMEM;
		sem_id = semget(key,1,0);
		if (sem_id != -1)semctl(sem_id,0,IPC_RMID,0);

		key = SEM_SYNC;
		sem_id = semget(key,1,0);
		if (sem_id != -1)semctl(sem_id,0,IPC_RMID,0);

		key = SEM_QCOUNT;
		sem_id = semget(key,1,0);
		if (sem_id != -1)semctl(sem_id,0,IPC_RMID,0);

		key = STAMEM;
		shm_id = shmget(key,sizeof(int),0);
		if (shm_id != -1)shmctl(shm_id,IPC_RMID,0);

		key = QUEUEMEM;
		shm_id = shmget(key,sizeof(int),0);
		if (shm_id != -1)shmctl(shm_id,IPC_RMID,0);

		return 0;
	}
}

void sem_up(int semid, int val) { // ++ sem
	struct sembuf ss;
	ss.sem_num = 0;
	ss.sem_op = val;
	ss.sem_flg = 1;
	semop(semid, &ss, 1);

};

void sem_down(int semid,int val) { // -- sem
	struct sembuf ss;
	ss.sem_num = 0;
	ss.sem_op = val * (-1);
	ss.sem_flg = 1;
	semop(semid, &ss, 1);
};