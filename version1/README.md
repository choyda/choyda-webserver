#### 本节重要提示： 
>万事开头难，SOCKET编程从本章开始，需要介绍一些基本概念和实现原理。
>  
>TCP三次握手建立连接，四次握手断开连接，基本SOCKET系列函数背后的原理及细节，TCP状态转换，套接字文件等等概念，会以代码配合图解的形式讲解。 
>
>主要考虑一些新手朋友，不会牵扯太深层的东西，用到的地方我会点出来，以后会单独针对每个知识点整理出详细解析的文章，或者请读者借阅其他资料(man文档，APUE，UNP，深入理解计算机操作系统、TCP/IP协议详解等) 感谢！  

#### 一些重要概念：
按照惯例，先从三次握手四次断开开始，整体上有一个了解。  
请移步：[TCP/IP协议总述]    

SOCKET模型创建流程图：  
![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/socket3.png)


[TCP/IP协议总述]:https://github.com/choyda/choyda-webserver/blob/master/document/TCP%E5%8D%8F%E8%AE%AE%E6%80%BB%E8%BF%B0.md


Linux文件类型：  

<table>
	<th width='400px'>文件类型标识</th>
	<th width='400px'>文件类型</th>
	<tr>
		<td align="center"> - </td>
		<td align="center">普通文件</td>
	</tr>
	<tr>
		<td align="center">d</td>
		<td align="center">目录</td>
	</tr>
	<tr>
		<td align="center">|</td>
		<td align="center">符号链接</td>
	</tr>
	<tr>
		<td align="center">s（伪文件）</td>
		<td align="center">套接字</td>
	</tr>
	<tr>
		<td align="center">b（伪文件）</td>
		<td align="center">块设备</td>
	</tr>
	<tr>
		<td align="center">c（伪文件）</td>
		<td align="center">字符设备</td>
	</tr>
	<tr>
		<td align="center">p（伪文件）</td>
		<td align="center">管道</td>
	</tr>
</table>

* 占用存储空间：文件、目录、符号链接。慢速符号链接存储在磁盘上、快速符号连接存储在inode结构中。
* 不占用存储空间：套接字、块设备、字符设备、管道。

  
“一切皆是文件” 是Unix/Linux的基本哲学之一，屏蔽了硬件的区别，所有设备都抽象成文件，提供统一的接口给用户。  
  
文件描述符：内核为每一个进程维护一张该进程打开的文件的记录表，文件描述符实际上是一个索引值，指向的就是这张表中的一条记录。
     
既然是文件，那么我们可以使用文件描述符引用套接字。与管道类似的，Linux系统将其封装成文件的目的是为了统一接口，使得读写套接字和读写文件的操作一致。管道用于本地进程间通信，而套接字多应用于网络进程间数据的传递。  

socket究其本质是内核借助缓冲区形成的伪文件。(套接字的内核实现较为复杂，不宜在学习初期深入学习)。  
套接字出现必然是一对(插头和插座)，套接字是全双工(电话)，每一个套接字(sfd或者cfd)对应发送和接受两个缓冲区。


![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/socket1.png)


socket在通信接口中的位置：

![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/socket2.png)


#### 一些用到的主要函数：   
socket、bind、listen、accept、connect、setsockopt、memset、inet\_ntop、inet\_pton

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
![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/file_struct.gif)  
其中files_struct结构体的成员files为打开文件描述符表，定义如下：

```
struct files_struct {
	…
	struct fdtable fdtab;	//管理文件描述符
	…
	struct file __rcu * fd_array[NR_OPEN_DEFAULT]; //文件描述符和文件的对应关系
};
```

![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/file_struct1.gif)    
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


![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/file_struct2.gif)   

-----------
bind函数：  
从上面的socket函数我们了解到，socket创建返回来套接字(文件描述符)和FIFO，管道，终端，磁盘上的文件或者什么其他的东西等创建文件描述符没有什么区别，服务器程序所监听的网络地址和端口号通常是固定不变的，客户端程序得知服务器程序的地址和端口号后就可以向服务器发起连接，因此服务器需要调用bind绑定一个固定的网络地址和端口号。

``` 
#include<sys/types.h>  宏定义头文件  
#include<sys/socket.h> 函数头文件
int bind(int sockfd, const sockaddr *addr, socklen_t addrlen)
```

sockfd：socket函数创建出来的套接字(文件描述符)。  
addr：构造出IP地址加端口号。  
addrlen：结构体的长度。  
返回值：成功返回0，失败返回-1, 设置errno。  

重要结构：sockaddr  
strcut sockaddr 很多网络编程函数诞生早于IPv4协议，那时候都使用的是sockaddr结构体,为了向前兼容，现在sockaddr退化成了（void *）的作用，传递一个地址给函数，至于这个函数是sockaddr\_in还是sockaddr\_in6，由地址族确定，然后函数内部再强制类型转化为所需的地址类型。

![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/version1/bind-1.png)

bind()的作用是将参数sockfd和addr绑定在一起，使sockfd这个用于网络通讯的文件描述符监听addr所描述的地址和端口号。前面讲过，struct sockaddr *是一个通用指针类型，addr参数实际上可以接受多种协议的sockaddr结构体，而它们的长度各不相同，所以需要第三个参数addrlen指定结构体的长度。如：



```
/*绑定服务器地址结构*/
struct sockaddr_in serv_addr;               //定义结构体
socklen_t serv_len;                         //结构体长度
serv_len = sizeof(serv_addr);               //获取结构体长度
memset(&serv_addr, 0, serv_len);            //清空结构体
serv_addr.sin_family = AF_INET;             //使用的协议族
serv_addr.sin_addr.s_addr = INADDR_ANY;     //本机的任何网卡
serv_addr.sin_port = htons(SERV_PORT);      //程序端口号，本地转网络字节序【为0，则系统自动分配，使用getsockname函数配合】

/*初始化一个地址结构*/
if(bind(sfd, (struct sockaddr *)&serv_addr, serv_len) == -1){
	perror("fail to bind socket");
	exit(1);
}
```

首先将整个结构体清零，然后设置地址类型为AF\_INET，网络地址为INADDR_ANY，这个宏表示本地的任意IP地址，因为服务器可能有多个网卡，每个网卡也可能绑定多个IP地址，这样设置可以在所有的IP地址上监听，直到与某个客户端建立了连接时才确定下来到底用哪个IP地址，端口号为SERV\_PORT。

-----------  

