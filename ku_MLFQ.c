#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

typedef struct NODE {
	struct NODE* next;
	pid_t data;
	int timeAllot;
}Node;

typedef struct QUEUE {
	Node* front;
	Node* end;
	int count;
}Queue;

Queue queue1;
Queue queue2;
Queue queue3;
int count = 0;
int ts;

void initQueue(Queue* queue);
int IsEmpty(Queue* queue);
void enqueue(Queue* queue, pid_t pid, int ta);
pid_t dequeue(Queue* queue);
void moveAllQueue1();
void RR(Queue* currq, Queue* nextq);

void alarm_handler(int signo) {

	if (!count) {
		kill((&queue1)->front->data, SIGCONT);
	}
	else {
		if (!IsEmpty(&queue1)) {
			RR(&queue1, &queue2);
		}
		else if (!IsEmpty(&queue2)) {
			RR(&queue2, &queue3);
		}
		else {
			RR(&queue3, &queue3);
		}

		if (!IsEmpty(&queue1)) {
			kill(*(&queue1)->front->data, SIGCONT);
		}
		else if (!IsEmpty(&queue2)) {
			kill(*(&queue2)->front->data, SIGCONT);
		}
		else {
			kill(*(&queue3)->front->data, SIGCONT);
		}
	}
	count++;
}

int main(int argc, char* argv[]) {
	
	int n = atoi(argv[1]);
	ts = atoi(argv[2]);
	pid_t pid;
	char* str[26] = { "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z" };

	struct itimerval timer;
	int ret;

	initQueue(&queue1);
	initQueue(&queue2);
	initQueue(&queue3);

	signal(SIGALRM, alarm_handler);

	timer.it_value.tv_sec = 5;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 1;
	timer.it_interval.tv_usec = 0;

	for (int i = 0; i < n; i++) {
		sleep(5);
		pid = fork();
		if (pid == 0) {
			execl("./ku_app", "ku_app", str[i], NULL);
		}
		else {
			enqueue(&queue1, pid, 0);
		}
	}

	ret = setitimer(ITIMER_REAL, &timer, NULL);
	if (ret) {
		perror("setitimer");
		return 0;
	}

	while (1)
		pause(1);

}

void initQueue(Queue* queue)
{
	queue->front = queue->end = NULL;
	queue->count = 0;
}

int IsEmpty(Queue* queue)
{
	return queue->count == 0;
}

void enqueue(Queue* queue, pid_t pid, int ta)
{
	Node* now = (Node*)malloc(sizeof(Node));
	now->data = pid;
	now->next = NULL;
	now->timeAllot = ta;

	if (IsEmpty(queue)) {
		queue->front = now;
	}
	else {
		queue->end->next = now;
	}
	queue->end = now;
	queue->count++;
}

pid_t dequeue(Queue* queue)
{
	pid_t ret;
	Node* curr;
	if (IsEmpty(queue)) {
		return 0;
	}
	curr = queue->front;
	ret = curr->data;
	queue->front = curr->next;
	free(curr);
	queue->count--;
	return ret;
}

void moveAllQueue1()
{
	Node* curr;
	pid_t p;
	int n = (&queue1)->count;

	while (!IsEmpty(&queue2)) {
		p = dequeue(&queue2);
		enqueue(&queue1, p, 0);
	}
	while (!IsEmpty(&queue3)) {
		p = dequeue(&queue3);
		enqueue(&queue1, p, 0);
	}

	for (int i = 0; i < n; i++) {
		curr = (&queue1)->front;
		if (curr->timeAllot >= 2) {
			if (IsEmpty(&queue2)) {
				(&queue2)->front = curr;
			}
			else {
				(&queue2)->end->next = curr;
			}
			(&queue2)->end = curr;
			(&queue2)->count++;
			(&queue1)->count--;
			curr->timeAllot = 0;
		}
	}

}

void RR(Queue* currq, Queue* nextq)
{
	pid_t p;
	int ta;

	ta = currq->front->timeAllot++;
	p = dequeue(currq);
	kill(p, SIGSTOP);
	enqueue(currq, p, ta);

	if (count >= ts) {
		exit(0);
	}
	if (count % 10 == 0) {
		moveAllQueue1();
	}
	else {
		while (currq->front->timeAllot >= 2) {
			p = dequeue(currq);
			enqueue(nextq, p, 0);
			currq->count--;
			nextq->count++;
		}
	}

}


