> #### 说明:
> 这篇文章主要是从比较简单易懂的角度描述一下TCP协议和通信的一般过程。
>
> 当然，在通信过程中还有很多比较复杂和容易出错的细节，比如TCP连接三次握手第三次服务器没有收到ACK应答怎么办？这个又是另外一个点，会涉及到 SYN FLOOD攻击、内核调优、状态转换等，再比如TIME_WIAT的回收与重用，TCP超时重传，流量控制，滑动窗口等等疑难杂症。  
>
> 还有TCP传输需要的上下文协议，以太网帧，IP, ARP，路由，HTTP协议等相关知识。 
>
>以后会在相应的版本和文档中，针对每一点给出一个详细的梳理，由浅入深。

### 一. TCP/IP协议族(簇)
1. TCP/IP协议簇：是Internet的基础，TCP/IP是一组协议的代名词，包括许多别的协议，组成了TCP/IP协议簇。其中比较重要的有SLIP协议、PPP协议、IP协议、ICMP协议、ARP协议、TCP协议、UDP协议、FTP协议、DNS协议、SMTP协议等。
  
2. TCP/IP协议模型： OSI的七层参考模型也叫做传统的开放式系统互连参考模型，其中每一层执行某一特定任务。该模型的目的是使各种硬件在相同的层次上相互通信。而TCP/IP通讯协议采用了4层的层级结构，每一层都呼叫它的下一层所提供的网络来完成自己的需求。


3. 下图给了一个TCP/IP四层和OSI七层的对比模型：具体的每层主要用途，请自行百度或Google，由于篇幅过长这里就不阐述了。


<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview15-5.jpg" /> 
</div>


### 二. TCP协议  
#### TCP协议简介
TCP是一种面向连接（连接导向）的、可靠的基于字节流的传输层通信协议。TCP将用户数据打包成报文段，它发送后启动一个定时器，另一端收到的数据进行确认、对失序的数据重新排序、丢弃重复数据。


#### TCP的特点有：

1. TCP是面向连接的运输层协议。  
2. 每一条TCP连接只能有两个端点，每一条TCP连接只能是点对点的。  
3. TCP提供可靠交付的服务。  
4. TCP提供全双工通信。数据在两个方向上独立的进行传输。因此，连接的每一端必须保持每个方向上的传输数据序号。
5. 面向字节流。面向字节流的含义：虽然应用程序和TCP交互是一次一个数据块，但TCP应用程序交下来的数据仅仅是一连串的无结构的字节流。

#### TCP包 首部

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview3.jpg" /> 
</div>

* 下面解释了TCP首部每个段的作用，其中包括了6个标记位的含义，还有比较重要的11种状态变迁会在TCP状态转换的时候讲。

1. 源端口号：数据发起者的端口号，16bit。  
2. 目的端口号：数据接收者的端口号，16bit。    
3. 序号：32bit的序列号，由发送方使用。    
4. 确认序号：32bit的确认号，是接收数据方期望收到发送方的下一个报文段的序号，因此确认序号应当是上次已成功收到数据字节序号加1。  
5. 首部长度：首部中32bit字的数目，可表示15*32bit=60字节的首部。一般首部长度为20字节，如果有选项长度会增加。  
6. 保留：6bit, 均为0。  
7. 紧急URG：当URG=1时，表示报文段中有紧急数据，应尽快传送。  
8. 确认比特ACK：ACK = 1时代表这是一个确认的TCP包，取值0则不是确认包。  
9. 推送比特PSH：当发送端PSH=1时，接收端尽快的交付给应用进程。  
10. 复位比特（RST）：当RST=1时，表明TCP连接中出现严重差错，必须释放连接，再重新建立连接。  
11. 同步比特SYN：在建立连接是用来同步序号。SYN=1， ACK=0表示一个连接请求报文段。SYN=1，ACK=1表示同意建立连接。  
12. 终止比特FIN：FIN=1时，表明此报文段的发送端的数据已经发送完毕，并要求释放传输连接。  
13. 窗口：用来控制对方发送的数据量，通知发放已确定的发送窗口上限。  
14. 检验和：该字段检验的范围包括首部和数据这两部分。由发端计算和存储，并由收端进行验证。  
15. 紧急指针：紧急指针在URG=1时才有效，它指出本报文段中的紧急数据的字节数。  
16. 选项：长度可变，最长可达40字节。  
  

### 三.Wireshark抓包与协议层对应关系

wireshark抓到的包与对应的协议层如下图所示：  


<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview2.jpg" /> 
</div>


1. Frame:   物理层的数据帧概况  
2. Ethernet II: 数据链路层以太网帧头部信息  
3. Internet Protocol Version 4: 互联网层IP包头部信息  
4. Transmission Control Protocol:  传输层的数据段头部信息，此处是TCP  
5. Hypertext Transfer Protocol:  应用层的信息，此处是HTTP协议。  


 
wireshark捕获到的TCP包中的每个字段如下图所示：  

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview4.jpg" width="720" height="420" /> 
</div>

### 四.TCP建立连接三次握手
TCP建立连接时，会有三次握手过程，如下图所示，wireshark截获到了三次握手的三个数据包。第四个包才是http的，说明http的确是使用TCP建立连接的。  


 <div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview5.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview14.jpg" /> 
</div>


第一次握手:  
客户端向服务器发送连接请求包，标志位SYN（同步序号）置为1，序号为X=0。  

 <div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview6.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview26.jpg" /> 
</div>
 

第二次握手:    
服务器收到客户端发过来报文，由SYN=1知道客户端要求建立联机。向客户端发送一个SYN和ACK都置为1的TCP报文，设置初始序号Y=0，将确认序号(Acknowledgement Number)设置为客户的序列号加1，即X+1 = 0+1=1, 如下图。  

 <div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview7.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview27.jpg" /> 
</div>


第三次握手:  
客户端收到服务器发来的包后检查确认序号(Acknowledgement Number)是否正确，即第一次发送的序号加1（X+1=1）。以及标志位ACK是否为1。若正确，服务器再次发送确认包，ACK标志位为1，SYN标志位为0。确认序号(Acknowledgement Number)=Y+1=0+1=1，发送序号为X+1=1。客户端收到后确认序号值与ACK=1则连接建立成功，可以传送数据了。  

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview8.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview28.jpg" /> 
</div>


### 五.TCP断开连接四次挥手
 
TCP断开连接时，会有四次挥手过程，如下图所示，wireshark截获到了四次挥手的四个数据包。    

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview9.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview18.jpg" /> 
</div>

第一次挥手:  
客户端给服务器发送TCP包，用来关闭客户端到服务器的数据传送。将标志位FIN和ACK置为1，序号为X=1，确认序号为Z=1。  

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview10.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview20.jpg" /> 
</div>


第二次挥手:  
服务器收到FIN后，发回一个ACK(标志位ACK=1),确认序号为收到的序号加1，即X=X+1=2。序号为收到的确认序号=Z。  

 <div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview11.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview21.jpg" /> 
</div>

第三次挥手:
服务器关闭与客户端的连接，发送一个FIN。标志位FIN和ACK置为1，序号为Y=1，确认序号为X=2。 

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview12.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview22.jpg" /> 
</div>


第四次挥手:
客户端收到服务器发送的FIN之后，发回ACK确认(标志位ACK=1),确认序号为收到的序号加1，即Y+1=2。序号为收到的确认序号X=2。  

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview13.jpg" /> 
</div>
<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview23.jpg" /> 
</div>

### 六.最后在给两个整体的流程图
#### TCP连接断开：  

<div>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview29.jpg" width="520" height="620" /> 
</div>

#### TCP协议传输示意图（逐层封装，逐层解封）：  

<div  align=center>
<img src="https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/tcp-overview/tcp-overview15-22.jpeg" width="720" height="420" /> 
</div>


说不如写，写不如画。  
图片大部分来自百度搜索：主要是用简单的图表达复杂的协议。    
部分配图借鉴：http://www.360doc.com/content/14/1201/16/7669533_429603672.shtml