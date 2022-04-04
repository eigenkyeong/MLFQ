#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

typedef struct NODE {
	struct NODE* next;
	pid_t data;
	int timeAllot;
	int c;
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
//int n;
int ts;

void initQueue(Queue* queue);
void initNode(Queue* queue, pid_t pid, int ta, int ch);
int IsEmpty(Queue* queue);
void enqueue(Queue* queue, pid_t pid, int ta);
pid_t dequeue(Queue* queue);
void moveAllQueue1();
void RR(Queue* currq, Queue* nextq);

void alarm_handler(int signo) {
	pid_t p;
	int retValue;
	int ta;

	if (!count) {
		kill(queue1->front->data, SIGCONT);
	}
	else {
		if (!IsEmpty(queue1)) {
			RR(queue1, queue2);
		}
		else if (!IsEmpty(queue2)) {
			RR(queue2, queue3);
		}
		else {
			RR(queue3, queue3);
		}

		if (!IsEmpty(queue1)) {
			kill(queue1->front->data, SIGCONT);
		}
		else if (!IsEmpty(queue2)) {
			kill(queue2->front->data, SIGCONT);
		}
		else {
			kill(queue3->front->data, SIGCONT);
		}
	}

}

int main(int argc, char* argv[]) {
	
	int n = atoi(argv[1]);
	ts = atoi(argv[2]);
	char c = 'A';
	
	struct itimerval timer;
	int ret;

	initQueue(queue1);
	initQueue(queue2);
	initQueue(queue3);

	signal(SIGALRM, alarm_handler);

	timer.it_value.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = 10;
	timer.it_interval.tv_usec = 0;

	for (int i = 0; i < n; i++) {
		if (fork() == 0) {
			//initNode(queue1, getpid(), 0, c+i);
			enqueue(queue1, getpid(), 0);
			execl("./ku_app", "ku_app", "c+i", NULL);
		}
	}
	sleep(5);

	ret = setitimer(ITIMER_REAL, &timer, NULL);
	if (ret) {
		perror("setitimer");
		return 0;
	}

}

void initQueue(Queue* queue)
{
	queue->front = queue->end = NULL;
	queue->count = 0;
}

void initNode(Queue* queue, pid_t pid, int ta, int ch)
{
	Node* now = (Node*)malloc(sizeof(Node));
	now->data = pid;
	now->next = NULL;
	now->timeAllot = ta;
	now->c = ch;

	if (IsEmpty(queue)) {
		queue->front = now;
	}
	else {
		queue->end->next = now;
	}
	queue->end = now;
	queue->count++;
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
	Node* curr = queue1->front;
	Node* node = NULL;
	pid_t p;
	int n = queue1->count;

	queue1->end->next = queue2->front;
	queue2->end->next = queue3->front;
	queue1->end = queue3->end;
	queue1->count += queue2->count;
	queue1->count += queue3->count;
	initQueue(queue2);
	initQueue(queue3);

	for (int i = 0; i < n; i++) {
		if (curr->timeAllot >= 2) {
			if (IsEmpty(queue2)) {
				queue2->front = curr;
			}
			else {
				queue2->end->next = curr;
			}
			queue2->end = curr;
			queue2->count++;
			queue1->count--;
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
		free(queue1);
		free(queue2);
		free(queue3);
		exit(0);
	}
	if (count % 10 == 0) {
		moveAllQueue1();
	}
	while (currq->front->timeAllot >= 2) {
		p = dequeue(currq);
		enqueue(nextq, p, 0);
	}

	//kill(queue1->front->data, SIGCONT);
}


