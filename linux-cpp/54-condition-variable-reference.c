// 条件变量实现生产者消费者模型
#include  <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct Node {
	int num;
	struct Node* next;
};

struct Node* head = NULL;
struct Node* mp = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* producter(void* arg)
{
	while (1) {
		mp = (struct Node *)malloc(sizeof(struct Node));
		mp->num = rand() % 400 + 1;
		printf("---producted---%d\n", mp->num);

		pthread_mutex_lock(&mutex);
		mp->next = head;
		head = mp;
		pthread_mutex_unlock(&mutex);

		pthread_cond_signal(&cond);          /* 唤醒等待在该条件变量上的线程 */

		sleep(rand() % 3);
	}

	return NULL;
}

void* consumer(void* arg)
{
	while (1) {
		pthread_mutex_lock(&mutex);
		while (head == NULL) {
			pthread_cond_wait(&cond, &mutex);   // broadcast 会唤醒所有等待在条件变量上的线程，该函数返回并不一定条件成立
		}
		mp = head;
		head = mp->next;
		pthread_mutex_unlock(&mutex);

		printf("------------------consumer--%d\n", mp->num);
		free(mp);
		mp = NULL;
		sleep(rand() % 3);
	}

	return NULL;
}

int main(void)
{
	pthread_t ptid, ctid;


	pthread_create(&ptid, NULL, producter, NULL);
	pthread_create(&ctid, NULL, consumer, NULL);

	pthread_join(ptid, NULL);
	pthread_join(ctid, NULL);

	return 0;
}
