#### 本节重要提示： 
万事开头难，socket编程开始，需要介绍一些基本概念和操作原理，本章节描述的比较多，
基本概念和原理会用代码图片等简单的方式描述出来，主要考虑一些新手朋友，不会牵扯太深层的东西，更底层的操作如socket_create在内核中的创建过程等，有需要我会点出来，由于底层代码较多，思想比较复杂，故不予列出，以后会单独针对每个知识点整理出详细解析的文章，如想彻底了解，请读者借阅其他资料(man文档，APUE等，深入理解计算机操作系统、TCP/IP协议详解等) 感谢！  

* 一些用到的主要函数：   
socket、bind、listen、accept、setsockopt、memset、inet\_ntop、inet\_pton

-----------
socket函数：  
socket 这个函数建立一个协议族为domain、协议类型为type、协议编号为protocol的套接字文件描述符。如果函数调用成功，会返回一个标识这个套接字的文件描述符，失败的时候返回-1，并设置errno。  
函数的原型如下:

``` 
#include <sys/types.h>  宏定义头文件  
#include <sys/socket.h> 函数头文件
int socket(int domain, int type, int protocol)
```
domain：参数domain用于设置网络通信的域，socket()根据这个参数选择通信协议的族。通信协议族在文件sys/socket.h中定义，一般常用的协议 TCP、UDP。


type：参数type用于设置套接字通信的类型，主要有SOCKET_STREAM（TCP流式套接字）、SOCK\_DGRAM（UDP数据包套接字）等。  


protocol：参数protocol用于制定某个协议的特定类型，通常TCP,UDP这样的通信协议 protocol参数设置为0，系统能自动推演出协议类型；有些协议有多种特定的类型，就需要设置这个参数来选择特定的类型。

示例: ```int sock = socket(AF_INET, SOCK_STREAM, 0)```

理解：  
Linux或者说Unix中一切皆文件，程序在执行任何形式的 I/O 的时候，程序是在读或者写一个文件描述符。  
内核为每个进程维护了一个结构体struct task_struct，可称为进程表项、进程控制块（PCB: Process Control Block）或者进程描述符，如下：

``` 
struct task_struct {
	volatile long state;  /* -1 unrunnable, 0 runnable,>0 stopped 
	…
	pid_t pid;
	…
	struct files_struct *files; //进程的文件描述符表
	…
};
```
![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/file_struct.gif)  
其中files_struct结构体的成员files为打开文件描述符表，定义如下：

```
struct files_struct {
	…
	struct fdtable fdtab;	//管理文件描述符
	…
	struct file __rcu * fd_array[NR_OPEN_DEFAULT]; //文件描述符和文件的对应关系
};
```
![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/file_struct1.gif)    
其成员fdtab为关键数据成员，定义如下：

```
struct fdtable {
 unsigned int max_fds;               //可以代开的最大文件数
 int max_fdset;                      //位图的最大长度
 int next_fd;                        //下一个可用的fd
 struct file ** fd;      				//指向files_struct的fd_array
 fd_set *close_on_exec;
 fd_set *open_fds;                   //打开的文件标记，比如第2位为0，则打开了2号文件
 struct rcu_head rcu;
 struct files_struct *free_files;
 struct fdtable *next;
};
```


![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/file_struct2.gif)   
配图来自：http://www.51testing.com/html/00/n-832100-2.html

-----------
bind函数：  
bind通过给一个未命名套接口分配一个本地名字来为套接口建立本地捆绑(主机地址/端口号)。从上面的socket函数我们了解到，socket创建返回来套接字(文件描述符)和FIFO，管道，终端，磁盘上的文件或者什么其他的东西等创建文件描述符没有什么区别，但是调用bind()函数之后，为这个套接字关联一个相应地址，发送到这个地址的数据可以通过该套接字读取与使用。  

``` 
#include<sys/types.h>  宏定义头文件  
#include<sys/socket.h> 函数头文件
int bind(int sockfd, const sockaddr *addr, socklen_t addrlen)
```
sockfd：socket函数创建出来的套接字(文件描述符)。  
addr：关联套接字的详细结构体信息。
addrlen：结构体的长度。


备注：bind()函数并不是总是需要调用的，只有用户进程想与一个具体的地址或端口相关联的时候才需要调用这个函数。如果用户进程没有这个需要，那么程序可以依赖内核的自动的选址机制来完成自动地址选择，而不需要调用bind()函数，同时也避免不必要的复杂度。在一般情况下，对于服务器进程问题需要调用bind()函数，对于客户进程则不需要调用bind()函数。