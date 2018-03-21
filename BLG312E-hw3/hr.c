// Turgut Can Aydinalev - 150120021 - OS - HW3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>

#define SIZE 300
#define STAMEM 77
#define QUEUEMEM 155
#define QCOUNT 133
#define SEM_QCOUNT 112
#define SEM_STAMEM 1900
#define SEM_QUEUEMEM 1907
#define SEM_SYNC 1111;

int rtime, app_count=0, shm_id, sem_id,sem_sync,sem_idq, shm_idq, sem_idqc,shm_idqc; // receptionist time and applicant counter
int q[SIZE]; // queue
int tail=0; // queue tail
int remaining=0;
int texitcount =0;
FILE *ptr;
pthread_mutex_t lockIt,countIt; // mutex definitions

void *iwfunc(void *id);
void *recfunc(void *id);
int fileops(char *str1);
void push(int in);
int pop();
void print_all();
void sem_up(int semid, int val);
void sem_down(int semid,int val);


int main(int argc, char *argv[]) {

	sleep(3); // wait for applicant.c to create semaphores

	key_t keysem2 = SEM_QUEUEMEM; 
	sem_idq = semget(keysem2,1,0);

	keysem2 = SEM_SYNC;
	sem_sync = semget(keysem2,1,0);

	key_t keysem3 = SEM_QCOUNT;
	sem_idqc = semget(keysem3,1,0); 

	key_t keyshm = QUEUEMEM; // queue shared memory
	shm_idq = shmget(keyshm, sizeof(int),0);

	remaining = atoi(argv[2]);
	app_count = remaining;

	if (fileops(argv[1]) == 0) return 0; // file operations
	key_t keysem = SEM_STAMEM; // get key of shared memory semaphore
	sem_id = semget(keysem,1,0);
	keyshm = STAMEM;
	shm_id = shmget(keyshm,sizeof(int),0); // get key of shared memory

	pthread_t rec; // thread definitions
	pthread_t iw[3];
	
	int rc,i;
	
	rc = pthread_mutex_init(&lockIt,NULL); // first mutex init
	if (rc) {
		printf("Error at mutex init!!");
		exit(1);
	}
	
	rc = pthread_mutex_init(&countIt,NULL); // second mutex init
	if (rc) {
		printf("Error at mutex init!!");
		exit(1);
	}

	rc = pthread_create(&rec, NULL,recfunc,NULL); // receptionist thread
	if (rc)  {
		printf("Error at thread creation!!");
		exit(1);
	}

	for (i=0; i<3;i++) { // interviewer threads
		rc = pthread_create(&iw[i], NULL,iwfunc,(void *) i);
		if (rc)  {
		printf("Error at thread creation!!");
		exit(1);
	}
	}
	while (texitcount < 3) {} // wait for all interviewer finishes their jobs
	printf("Interviewing process is done!\n");
	sleep(1);
	sem_up(sem_sync,1); // telling to trial that interviews are done
	pthread_mutex_destroy(&lockIt);
	pthread_mutex_destroy(&countIt);
	pthread_exit(NULL);
	fclose(ptr);
	return 0;
}

//queue functions
void print_all() {
	int i;
	printf("queue: ");
	for (i=0; i <tail; i ++) printf("%d ", q[i]);

	printf("\n");
}

void push(int in) {
	q[tail++] = in;
};

int pop(void) {
	int temp = q[0];
	int i;
	for(i=0; i <tail-1; i++) { q[i] = q[i+1];}
	tail--;
	return temp;
}; // queue functions end

int fileops(char *str1) { //file operations

	ptr = fopen(str1, "r");

	if (!ptr) {
		printf("File cannot be opened! \n");
		return 0;
	}
	else {
		fscanf(ptr, "%d\n",&rtime);
		return 1;
	}
}

void *recfunc(void *id) { // receptionist function
	int t;
	printf("Registeration time: %d seconds\n", rtime);
	for(t=0; t<app_count;t++) printf("Applicant %d applied to the Receptionist (Status = 0)\n", t+1); // all applicants are applied
	int i=0;
	while (i < app_count) { //adding queue. no need to mutex because there is only 1 thread using this loop
		sleep(rtime);
		push(i);
		printf("Applicant %d's registeration is done (Status = 0)\n", i+1);
		i++;
	}
	printf("Registeration process is done!\n");
	pthread_exit(NULL);
}

void *iwfunc(void *id) {
	int tid = (int) id;
	while (remaining > 0) {
		if(tail > 0) {

			pthread_mutex_lock(&lockIt); // mutex start
			int app = pop();
			--remaining;
			pthread_mutex_unlock(&lockIt); // mutex end

			int time,temp,i,flag;
			fseek(ptr, 0, SEEK_SET);
			fscanf(ptr, "%d\n", &temp);

			if (app > 0) {
				for (i=0; i < app; i++) {
					fscanf(ptr,"%d %d %d %d\n",&temp,&temp,&temp,&temp);
				}
			}
			fscanf(ptr, "%d %d %d %d\n",&time,&temp,&flag,&temp);

			printf("Interviewer %d started interview with Appliciant %d (Status = 0)\n",tid+1,app+1);
			sleep(time);
			sem_down(sem_id,1);
			if (flag == 0) temp = 1;
			else temp = 2;
			int *shptr;
			shptr =  (int *) shmat(shm_id,0,0);
			shptr[app] = temp;
			printf("Interviewer %d finished interview with Appliciant %d (Status = %d)\n",tid+1,app+1,shptr[app]);
			shmdt(shptr);
			sem_up(sem_id,1);

			if (flag == 1) {
				sem_down(sem_idq,1);
				int *ptr;
				ptr = (int *) shmat(shm_idq,0,0);
				
				ptr[0]++; // increment shared queue counter
				ptr[ptr[0]] = app; // add the element to the shared queue

				shmdt(ptr);
				sem_up(sem_idq,1);
				sem_up(sem_idqc,1);
			}
		}
	}
	pthread_mutex_lock(&countIt);
	texitcount++;
	pthread_mutex_unlock(&countIt);
	pthread_exit(NULL);
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