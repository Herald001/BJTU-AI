/*
* 	д������
*/

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/types.h>
# include <pthread.h>
# include <semaphore.h>
# include <string.h>
# include <unistd.h>

//semaphores
sem_t RWMutex, mutex1, mutex2, mutex3, wrt;
int writeCount, readCount;


struct data {
	int id;
	int opTime;
	int lastTime;
};

//����
void* Reader(void* param) {
	int id = ((struct data*)param)->id;
	int lastTime = ((struct data*)param)->lastTime;
	int opTime = ((struct data*)param)->opTime;

	sleep(opTime);
	printf("�����ߡ��߳�%d�ȴ����Ķ���\n", id);	

	sem_wait(&mutex3);
	sem_wait(&RWMutex);
	sem_wait(&mutex2);
	readCount++;
	if(readCount == 1)
		sem_wait(&wrt);
	sem_post(&mutex2);
	sem_post(&RWMutex);
	sem_post(&mutex3);

	printf("�����ߡ��߳�%d��ʼ���Ķ���\n", id);
	/* reading is performed */
	sleep(lastTime);
	printf("�����ߡ��߳�%d�������Ķ���\n", id);

	sem_wait(&mutex2);
	readCount--;
	if(readCount == 0)
		sem_post(&wrt);
	sem_post(&mutex2);

	pthread_exit(0);
}

//д��
void* Writer(void* param) {
	int id = ((struct data*)param)->id;
	int lastTime = ((struct data*)param)->lastTime;
	int opTime = ((struct data*)param)->opTime;

	sleep(opTime);
	printf("��д�ߡ��߳�%d�ȴ���д�롿\n", id);
	
	sem_wait(&mutex1);
	writeCount++;
	if(writeCount == 1){
		sem_wait(&RWMutex);
	}
	sem_post(&mutex1);
	
	sem_wait(&wrt);
	printf("��д�ߡ��߳�%d���ڡ�д�롿\n", id);
	/* writing is performed */
	sleep(lastTime);
	printf("��д�ߡ��߳�%d������д�롿\n", id);
	sem_post(&wrt);

	sem_wait(&mutex1);
	writeCount--;
	if(writeCount == 0) {
		sem_post(&RWMutex);
	}
	sem_post(&mutex1);
	
	pthread_exit(0);
}

int main() {
	printf("==================================================\n"); 
    printf("���� ������Ashyfox \n");
    printf("��ѧ �š���22231044\n");
    printf("��OSLAB����д�����ȵĶ���-д������ģ�⴦�������\n");
    printf("==================================================\n"); 
    printf("**�������ݣ�ʾ���̣߳���\n"); 
	printf("1 R 3 5\n" );
	printf("2 W 4 5\n" );
	printf("3 R 5 2\n" );
	printf("4 R 6 5\n" );
	printf("5 W 7 3\n" );

	pthread_t tid; 
	pthread_attr_t attr; //set of thread attributes
	pthread_attr_init(&attr);

	sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&mutex3, 0, 1);
    sem_init(&wrt, 0, 1);
    sem_init(&RWMutex, 0, 1);

    readCount = writeCount = 0;

	int id = 0;
	printf("**�봴���̣߳�\n"); 
	while(scanf("%d", &id) != EOF) {
		
		char role;		
		int opTime;		
		int lastTime;	
		
		scanf("%c%d%d", &role, &opTime, &lastTime);
		struct data* d = (struct data*)malloc(sizeof(struct data));

		d->id = id;
		d->opTime = opTime;
		d->lastTime = lastTime;

		if(role == 'R') {
			printf("�����ߡ��߳�%d��������\n", id);
			pthread_create(&tid, &attr, Reader, d);

		}
		else if(role == 'W') {
			printf("��д�ߡ�����%d��������\n", id);
			pthread_create(&tid, &attr, Writer, d);
		}
	}

	sem_destroy(&mutex1);
	sem_destroy(&mutex2);
	sem_destroy(&mutex3);
	sem_destroy(&RWMutex);
	sem_destroy(&wrt);

	return 0;
}

