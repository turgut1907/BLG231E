// Turgut Can Aydinalev - 150120021 - OS - HW3
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#define SIZE 300
#define STAMEM 77
#define QUEUEMEM 155
#define QCOUNT 133
#define SEM_QCOUNT 112
#define SEM_STAMEM 1900
#define SEM_QUEUEMEM 1907
#define SEM_SYNC 1111;

FILE *ptr;
int sem_id,sem_idq, sem_idqc,sem_sync, shm_id, shm_idq,shm_idqc,exited;
int app_counter;
void sem_up(int semid, int val);
void sem_down(int semid,int val);
void *trfunc(void *id);

int main(int argc, char *argv[]) {
	sleep(5); // for sync with hr.c

	ptr = fopen(argv[1], "r");
	if (ptr == NULL) printf("dosyada hata\n");
	int temp,t;
	app_counter = 0;
	fscanf(ptr, "%d\n", &temp);
	while(!feof(ptr)) {
		fscanf(ptr,"%d %d %d %d\n",&temp,&temp,&t,&temp);
		if (t==1) app_counter++; // how many applicants will be here?
		}

	key_t keysem1 = SEM_QCOUNT;
	key_t keysem2 = SEM_QUEUEMEM;
	key_t keysem3 = SEM_STAMEM;
	key_t keysem4 = SEM_SYNC;
	key_t keyshm1 = STAMEM;
	key_t keyshm2 = QUEUEMEM;

	sem_sync = semget(keysem4,1,0);// getting ids
	sem_id = semget(keysem3,1,0); 
	sem_idq = semget(keysem2,1,0);
	sem_idqc = semget(keysem1,1,0);
	shm_id = shmget(keyshm1,sizeof(int),0);
	shm_idq = shmget(keyshm2,sizeof(int),0);

	pthread_t tr[4];
	int i,rc;
	exited=0;

	for (i=0; i<4;i++) { // trainer threads
		rc = pthread_create(&tr[i], NULL,trfunc,(void *) i);
		if (rc)  {
		printf("Error at thread creation!!");
		exit(1);
		}
	}

	while (exited < 4) {} // wait
	sem_down(sem_sync,1); // wait for hr.c end
	printf("Training process is done!\n");
	fclose(ptr);
	pthread_exit(NULL);
	return 0;
}

void *trfunc(void *id) {
		int tid = (int) id;
		while (app_counter > 0) {
		sleep(1);
		sem_down(sem_idqc,1); // waiting the queue counter
		if (app_counter == 0) break;
		sem_down(sem_idq,1); // waiting for accessing shared queue
		
		int *pt = (int *) shmat(shm_idq,0,0);
		app_counter--;
		int count = pt[0];
		pt[0]--; // decrement counter
		int app = pt[1]; // get the first element

		int k;
			for(k=1;k < count+1; k++) {
				pt[k] = pt[k+1]; // queue shift
			}

		shmdt(pt);
		sem_up(sem_idq,1);

		sem_down(sem_id,1); //
		int *shptr;
		shptr =  (int *) shmat(shm_id,0,0);
		printf("Trainer %d started training with Appliciant %d (Status = %d)\n",tid+1,app+1,shptr[app]);
		shmdt(shptr);
		sem_up(sem_id,1); //

		int time,temp,i,flag;
		fseek(ptr, 0, SEEK_SET);
		fscanf(ptr, "%d\n", &temp);
		if (app > 0) {
			for (i=0; i < app; i++) {
				fscanf(ptr,"%d %d %d %d\n",&temp,&temp,&temp,&temp);
			}
		}
		fscanf(ptr, "%d %d %d %d\n",&temp,&time,&temp,&flag);

		if (flag == 0) temp = 3; // status
		else temp = 4;
		sleep(time);
		sem_down(sem_id,1); // status update
		shptr =  (int *) shmat(shm_id,0,0);
		shptr[app] = temp;
		printf("Trainer %d finished training with Appliciant %d (Status = %d)\n",tid+1,app+1,shptr[app]);
		shmdt(shptr);
		if (app_counter == 0) sem_up(sem_idqc,3);
		sem_up(sem_id,1);
	}
	exited++;
	pthread_exit(NULL);
};

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