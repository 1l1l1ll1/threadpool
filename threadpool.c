#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#define MaxChar 20


typedef struct Object object;
typedef struct Queue queue;

struct Object{                              //用于存储文件信息：操作数，操作符和结果
	int a; 
	int b;                  //Operands
	char ope[MaxChar];	//operater
	double res;		//result
  object* next;
};

struct Queue{                               //object的链式队列
	object* head;
	object* tail;
};

pthread_mutex_t lock =PTHREAD_MUTEX_INITIALIZER;    //对锁和信号量进行初始化
pthread_mutex_t lock2 =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready =PTHREAD_COND_INITIALIZER;

queue* qp;                                    //定义请求队列和存储结果的队列
queue* qp_ans;

void queue_init(queue *qp){
	qp->head = NULL;
	qp->tail = NULL;
}

void print_ans(queue *qp){                    //打印队列中的信息
	object* jp;
	printf("print the answer\n");
	for (jp = qp->head; jp != NULL; jp = jp->next){
		printf("%d %s %d = %lf\n", jp->a, jp->ope, jp->b,jp->res);
	}
}

void object_insert(queue *qp, object *jp){        //append a job on the tail of the queue

	jp -> next = NULL;
	if (qp -> tail != NULL){
		qp -> tail->next = jp;
	}else {
		qp -> head = jp;
	}
	qp -> tail = jp;
}

void object_remove(queue *qp){                   //移除队列中头部结构
	object *jp;
	if (qp->head->next == NULL){
		qp->tail = NULL;
	}
	jp = qp->head->next;
	qp->head->next = NULL;
	qp->head = jp;
}

void work_qp(queue *sqp){                        //对传入队列进行计算并存储在结果队列
	object *jp;
	for (jp = sqp->head; jp != NULL; jp = sqp->head){
		object_remove(sqp);
		if (strcmp(jp->ope, "+") == 0){
	  		jp->res = jp->a + jp->b;
		} else if (strcmp(jp->ope, "-") == 0) {
	    	jp->res = jp->a -jp->b;    
		} else if (strcmp(jp->ope, "*") == 0) {
	   		jp->res = jp->a * jp->b;    
		} else {
	   	  if (jp->b == 0){
			      printf("%d / %d, b cannotbe zero\n", jp->a, jp->b);
	       		jp->res = -1;
	   	} else {
	   		jp->res = (double)jp->a / jp->b;
	   		}
		}
    pthread_mutex_lock(&lock2);
    object_insert(qp_ans, jp);                   //将计算结果放入结果队列
    pthread_mutex_unlock(&lock2);
	}
} 

void *pthr_req(void *argc){                           //请求线程
	int a , b ;
	char ope[MaxChar] = {0};
	object *jp;
	
	
	while (scanf("%d%s%d", &a, ope, &b) != EOF){                  //对文件内容进行读取，一直读到文件结束符
      if ((jp = (object *)malloc(sizeof(object))) == NULL){
				      printf("thread malloc error\n");
        			exit(0);
        }    
                        
			jp->a = a;
			jp->b = b;
			strcpy(jp->ope, ope);
			printf("a=%d str=%s b=%d\n",a, ope, b);
			
			pthread_mutex_lock(&lock);                       
			object_insert(qp, jp);                           //将文件计算请求放入队列
			pthread_mutex_unlock(&lock);
			pthread_cond_broadcast(&ready);                  //信号量广播
	}
	pthread_exit((void *)0);	
}

void *pthr_cal(void *argc){                                 //计算线程
	int num = (int)(*((int*)argc));
	queue *qp_self;
  if ((qp_self = (queue *)malloc(sizeof(queue))) == NULL){
        printf("the %dth calculte thread malloc error\n", num);
        exit(0);
	}
	queue_init(qp_self);
	object *jp;
  while (1){
        pthread_mutex_lock(&lock);
        while (qp->head == NULL)
          pthread_cond_wait(&ready, &lock);
        for (jp = qp->head; jp != NULL; jp = qp->head){
          object_remove(qp);                               
          object_insert(qp_self, jp);                    //从请求队列取出请求放入自己的队列
        }
  pthread_mutex_unlock(&lock);
  work_qp(qp_self);                                       //计算自己队列的请求
  }
}

int main(int argc, char* argv[]){
	if (argc != 2){                                     //检测输入是否正确
        	printf("input error\n");
        	exit(0);
        }
        
        int fd = -1;
        if ((fd = open(argv[1], O_RDONLY)) < 0){       //检测文件打开是否正确
        	printf("open file error\n");
      	  	exit(0);
        }
        
	dup2(fd, STDIN_FILENO); 		                          //将标准输入文件换为输入文件
  qp = (queue *)malloc(sizeof(queue));
  qp_ans = (queue *)malloc(sizeof(queue));
  queue_init(qp);
  queue_init(qp_ans);
  pthread_t tid = -1;
 
	for (int i = 1; i <= 3; i++){						                //创建三个请求线程
	if (pthread_create(&tid, NULL, pthr_req, &i) != 0){
		printf("create the %dth require thread error\n", i);
		exit(0);
	}
	}

	for (int i = 1; i <= 3; i++){						                 //创建三个计算线程
	if (pthread_create(&tid, NULL, pthr_cal, &i) != 0){
		printf("create the %dth calculate thread error\n", i);
		exit(0);
	}
	}

	sleep(1);
	print_ans(qp_ans);
	exit(0);    
}
