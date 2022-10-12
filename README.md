# threadpool
# 一、程序框架设计
## 1.1 基本概念
### 1.1.1 线程的定义

线程，有时被称为轻量级进程(Lightweight Process，LWP），是程序执行流的最小单元。一个标准的线程由线程ID，当前指令指针(PC），寄存器集合和堆栈组 成。另外，线程是进程中的一个实体，是被系统独立调度和分派的基本单位，线程自己不拥有系统资源，只拥有一点儿在运行中必不可少的资源，但它可与同属一个进程的其它线程共享进程所拥有的全部资源。一个线程可以创建和撤消另一个线程，同一进程中的多个线程之间可以并发执行。由于线程之间的相互制约，致使线程 在运行中呈现出间断性。线程也有就绪、阻塞和运行三种基本状态。每一个程序都至少有一个线程，若程序只有一个线程，那就是程序本身。

线程是程序中一个单一的顺序控制流程。在单个程序中同时运行多个线程完成不同的工作，称为多线程。

### 1.1.2 线程的特点

 

在多线程OS中，通常是在一个进程中包括多个线程，每个线程都是作为利用CPU的基本单位，是花费最小开销的实体。线程具有以下属性。

1）轻型实体

线程中的实体基本上不拥有系统资源，只是有一点必不可少的、能保证独立运行的资源，比如，在每个线程中都应具有一个用于控制线程运行的线程控制块TCB，用于指示被执行指令序列的程序计数器、保留局部变量、少数状态参数和返回地址等的一组寄存器和堆栈。

2）独立调度和分派的基本单位。

在多线程OS中，线程是能独立运行的基本单位，因而也是独立调度和分派的基本单位。由于线程很“轻”，故线程的切换非常迅速且开销小（在同一进程中的）。

3）可并发执行。

在一个进程中的多个线程之间，可以并发执行，甚至允许在一个进程中所有线程都能并发执行；同样，不同进程中的线程也能并发执行，充分利用和发挥了处理机与外围设备并行工作的能力。

4）共享进程资源。

在同一进程中的各个线程，都可以共享该进程所拥有的资源，这首先表现在：所有线程都具有相同的地址空间（进程的地址空间），这意味着，线程可以访问该地址空间的每一个虚地址；此外，还可以访问进程所拥有的已打开文件、定时器、信号量机构等。由于同一个进程内的线程共享内存和文件，所以线程之间互相通信不必调用内核。

 

### 1.1.3 线程与进程的比较

进程是资源分配的基本单位。所有与该进程有关的资源，都被记录在进程控制块PCB中。以表示该进程拥有这些资源或正在使用它们。

另外，进程也是抢占处理机的调度单位，它拥有一个完整的虚拟地址空间。当进程发生调度时，不同的进程拥有不同的虚拟地址空间，而同一进程内的不同线程共享同一地址空间。

与进程相对应，线程与资源分配无关，它属于某一个进程，并与进程内的其他线程一起共享进程的资源。

线程只由相关堆栈（系统栈或用户栈）寄存器和线程控制表TCB组成。寄存器可被用来存储线程内的局部变量，但不能存储其他线程的相关变量。

通常在一个进程中可以包含若干个线程，它们可以利用进程所拥有的资源。在引入线程的操作系统中，通常都是把进程作为分配资源的基本单位，而把线程作为独立运行和独立调度的基本单位。由于线程比进程更小，基本上不拥有系统资源，故对它的调度所付出的开销就会小得多，能更 高效的提高系统内多个程序间并发执行的程度，从而显著提高系统资源的利用率和吞吐量。因而近年来推出的通用操作系统都引入了线程，以便进一步提高系统的并发性，并把它视为现代操作系统的一个重要指标。

线程与进程的区别可以归纳为以下4点：

1）地址空间和其它资源（如打开文件）：进程间相互独立，同一进程的各线程间共享。某进程内的线程在其它进程不可见。

2）通信：进程间通信IPC，线程间可以直接读写进程数据段（如全局变量）来进行通信——需要进程同步和互斥手段的辅助，以保证数据的一致性。

3）调度和切换：线程上下文切换比进程上下文切换要快得多。

4）在多线程OS中，进程不是一个可执行的实体。

 

### 1.1.4 互斥锁

如果不需要信号量的计数能力，有时可以使用信号量的一个简化版本，称为互斥量（mutex）。互斥量仅仅适用于管理共享资源或一小段代码。由于互斥量在实现时既容易又有效，这使得互斥量在实现用户空间线程包时非常有用。

互斥量是一个可以处于两态之一的变量：解锁和加锁。这样，只需要一个二进制位表示它，不过实际上，常常使用一个整型量，0表示解锁，而其他所有的值则表示加锁。互斥量使用两个过程。当一个线程（或进程）需要访问临界区时，它调用mutex_lock。如果该互斥量当前是解锁的（即临界区可用），此调用成功，调用线程可以自由进入该临界区。

另一方面，如果该互斥量已经加锁，调用线程被阻塞，直到在临界区中的线程完成并调用mutex_unlock。如果多个线程被阻塞在该互斥量上，将随机选择一个线程并允许它获得锁。

由于互斥量非常简单，所以如果有可用的TSL或XCHG指令，就可以很容易地在用户空间中实现它们。

 

### 1.1.5 条件变量

条件变量是利用线程间共享的全局变量进行同步的一种机制，主要包括两个动作：一个线程等待"条件变量的条件成立"而挂起；另一个线程使"条件成立"（给出条件成立信号）。为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。

条件变量和互斥锁一样，都有静态和动态两种创建方式，静态方式使用PTHREAD_COND_INITIALIZER常量进行初始化，如下：

pthread_cond_tcond = PTHREAD_COND_INITIALIZER;

动态方式调用pthread_cond_init()函数，API定义如下：

int  pthread_cond_init(pthread_cond_t*cond, pthread_condattr_t *cond_attr)

尽管POSIX标准中为条件变量定义了属性，但在LinuxThreads中没有实现，因此cond_attr值通常为NULL，注销一个条件变量需要调用pthread_cond_destroy()，只有在没有线程在该条件变量上等待的时候，才能注销这个条件变量，否则返回EBUSY。API定义如下：

int  pthread_cond_destroy(pthread_cond_t *cond)

且被忽略。

### 1.1.6线程中需要用的函数

POSIX线程（Pthreads）的头文件是<pthread.h>，适用于类Unix操作系统。

1.创建线程

int  pthread_create(pthread_t*thread,constpthread_attr_t*attr,void*(*start_routine)(void*),void*arg);

2.等待线程

int  pthread_join(pthread_tthread,void**retval);

3.退出线程

void  pthread_exit(void*retval);

4.返回当前线程的线程标识符

pthread_t  pthread_self(void);

5.线程取消

int  pthread_cancel(pthread_tthread);

## 1.2 程序设计框图
![未命名文件](https://user-images.githubusercontent.com/114986300/195299847-f0101a87-c367-48c6-b8a9-bf7118be41cf.png)

在主控制线程中创建了三个请求线程，三个计算线程。使用了两个队列，一个存放计算请求，另一个存放计算结果。请求线程对请求队列进行写入，因此这里引入一把队列互斥锁实现对请求队列的保护。计算线程从请求队列中读取计算请求，并且放入每个线程各自的请求队列，然后对其进行计算后放入计算结果队列中，这里引入第二把锁来对计算结果队列进行保护。
这里要特别说明线程之间共享着文件表项及文件描述符，又因为read操作是原子操作，从而在多线程读文件时不会出现不正常读取现象。但是需要注意的是在各个请求放入总请求队列时会发生竞争导致错乱，在这里需要加入请求队列互斥量来避免多线程竞争。M个计算线程需要考虑当总请求队列中有任务时要去读取请求，将它们放入自己的队列中，注意如果没有任务时，这些线程也会不断的去尝试读取队列。
# 二、 核心数据结构
## 2.1 互斥锁以及信号量
```
pthread_mutex_t lock =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready =PTHREAD_COND_INITIALIZER;

```
为了防止由于各个线程之间产生竞争关系必须适当加上一些锁。在程序设计架构里可以看到，多线程会对总的请求队列和总的结构队列产生竞争操作，所以我们需要定义两个互斥量，分别为lock与lock2,这两个互斥量分别对于请求队列的锁与结构队列的锁。程序运行过程中，每个线程会不断地去争夺请求队列锁，从而需要判断队列是否为空。在请求队列为空的情况下，如果每个线程仍然去不断地争夺互斥量这必将导致程序效果低下，同时产生很多不必要的阻塞。因此定义一个条件变量ready，当线程得知当前请求队列为空时就进入睡眠状态，等待请求队列不为空的条件变量去激活，这样就能有效的避免大量的线程阻塞。
## 2.2  队列结构
```
typedef struct Object object;
typedef struct Queue queue;

struct Object{
	int a; 
	int b;                  //Operands
	char ope[MaxChar];	//operater
	double res;		//result
 object* next;
};

struct Queue{
	object* head;
	object* tail;
};
```
定义了队列结构如上，使用了链表式队列，队列里有object类型的头尾指针，object里有计算请求的相关信息：元素的值，运算符，运算结果和下一个object的地址。object结构既能表示每个任务请求，又能表示每个计算结果。 

# 三、 异常情况设计
## 3.1 产生多个请求队列出错
产生多个请求队列出错时，退出线程,并输出提示信息，部分代码如下。
```
for (int i = 1; i <= 3; i++){						//crerate three requie thread
	if (pthread_create(&tid, NULL, pthr_req, &i) != 0){
		printf("create the %dth require thread error\n", i);
		exit(0);
	}
 ```
## 3.2 产生多个计算队列出错
产生多个计算队列出错时，退出线程,并输出提示信息，部分代码如下。
```
	for (int i = 1; i <= 3; i++){						//cerate three calculate thread
	if (pthread_create(&tid, NULL, pthr_cal, &i) != 0){
		printf("create the %dth calculate thread error\n", i);
		exit(0);
	}
 ````
## 3.3 输入的参数有误
当输入信息有误，即输入的参数中没有指定计算文件时，退出线程,并输出提示信息，部分代码如下。
```
	if (argc != 2){
        	printf("input error\n");
        	exit(0);
        }
```
## 3.4 打不开指定的计算文件
当打不开指定的计算文件时，退出线程，并输出提示信息，部分代码如下。
```
if ((fd = open(argv[1], O_RDONLY)) < 0){
        	printf("open file error\n");
      	  	exit(0);
        }
```
## 3.5 堆中申请结构体不成功
当在堆中申请定义结构体的空间不成功时，退出线程，并输出提示信息，部分代码如下。
```
 if ((jp = (object *)malloc(sizeof(object))) == NULL){
				printf("thread malloc error\n");
        			exit(0);
```
## 3.6 请求错误

当程序在运行的过程中，如果请求中有错误请求时则会产生错误报告，不退出继续执行下一个计算：如输入 4 / 0时，因为除数不能为0，所以会产生"4 / 0, b cannot be zero"的报告。并输出提示信息，部分代码如下。

```
if (jp->b == 0){
			printf("%d / %d, b cannotbe zero\n", jp->a, jp->b);
	       		jp->res = -1;
```

# 四、流程图
## 4.1 主控线程流程图
程序开始运行时，首先进行初始化，完成准备工作，定义结构体、队列、变量等，然后创建请求线程，创建计算线程，在睡眠一秒钟的情况下，等待计算结果的输出，计算结果输出后再将计算结果打印输出，程序的主控线程如图所示。

![未命名文件 (1)](https://user-images.githubusercontent.com/114986300/195335855-c7cd1274-7c4a-4220-ae35-4a6e36dfb712.png)
## 4.2 请求线程流程图
![未命名文件 (2)](https://user-images.githubusercontent.com/114986300/195337998-c10a170e-9e23-4088-a1e6-cdc753efbab3.png)
## 4.3 计算线程流程图
![未命名文件 (3)](https://user-images.githubusercontent.com/114986300/195339043-214d87d3-c6e2-4682-8c92-671980500188.png)

# 五、性能分析
## 5.1 时间复杂度
   时间复杂度是指执行算法所需要的计算工作量，它定量描述了该算法的运行时间。

## 5.2 空间复杂度
空间复杂度(Space Complexity)是对一个算法在运行过程中临时占用存储空间大小的量度。一个算法在计算机存储器上所占用的存储空间，包括存储算法本身所占用的存储空间，算法的输入输出数据所占用的存储空间和算法在运行过程中临时占用的存储空间这三个方面。

对于存储算法本身所占用的存储空间，从pthreadpool.c这个文件来说，大小为4.31KB，文件很小。

对于算法的输入输出数据所占用的存储空间，算法本身输入的数据为存储在文件中的单词数据；输出数据为输出到文件中的数据，从上面文件大小的分析中可知，数据占用的数据量并不大。

对于算法在运行过程中临时占用的存储空间，程序编译后运行的过程中是一定会占用内存的，程序在运行过程中所占用的临时空间主要为定义变量、结构体、互斥锁进程、并行进程的运行、读写操作、程序本身运行等操作所需要的内存空间，可以看出，在保证程序完整执行的情况下，运行过程并不会占用太大的内存空间。

