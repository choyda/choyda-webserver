
### 用C打造自己的WEB服务器  
>  从最简单最基础的代码开始，到最后实现一个高性能的WEB服务器，主要用于学习交流、对知识点的总结。 
> 
>  每个版本迭代更新，会增加对应的扩展功能，每个版本都是独立的，配有Makefile，可以单独进行版本的接编和译运行，方便理解和调试，README.md会记录扩展功能所需要的知识要点，注意事项，和一些基础理论。
>  
>  会有一些概念性的总结文档，比如TCP协议概述、TCP状态转换、滑动窗口、超时重传，心跳检测，还有链路层，网络层，应用层对数据的处理等等。
>
>  会结合代码、Wireshark抓包工具还有大量的图解，当然会借鉴一些比较成功的，健壮的代码，如《深入理解计算机系统》中 csapp.c 文件的代码，还会阅读一些开源的项目如Nginx，libevent库等等，同时学习前辈们的编程思想。  
>
> 还会有进程相关、内存、线程相关、IO相关等等，系统编程知识。
>
> 因本人所学有限，代码或解释可能会有偏差和误解，希望大家秉承交流，指导的心态能给出宝贵的意见、建议。同时也希望能够帮助正在学习这方面内容的同学，大家共同进步。    
>
> PS：添加扩展功能可能会随着开发和迭代有所调整。目前先给出一个大纲。
>
>![](https://raw.githubusercontent.com/choyda/picture/master/choyda-webserver-picture/other/xiao.png) 写文档比写代码难！！


一些概念的文档：  

* [TCP/IP协议总述]

版本大纲：

* [version1]：Linux命令行模式下Client和Server通信，单进程。

* [version2]：实现多进程，同时可以处理多个客户端同时请求。
* [version3]：对一些库函数出错处理进行封装，自定义各种wrap.c文件。
* [version4]：实现多线程处理通信。
* [version5]：改变Server服务器为守护进程，不依赖于终端。
* [version6]：浏览器和Server交互，添加处理HTTP解析的自定义函数，csapp.c文件。
* [version7]：支持GET、POST方式，处理静态文件(包括HTML、CSS、JS、各种图片等)。
* [version8]：浏览器和Server交互，支持PHP文件访问，用CGI的方式处理PHP。
* [version9]：HTTP长连接处理，心跳包监测机制。
* [version10]：添加配置文件，主要实现配置PHP-FPM处理，和一些其他的参数配置。
* [version11]：支持PHP-FPM处理。
* [version12]：支持日志访问日志存储。
* [version13]：多路IO复用(select、poll、epoll)，用epoll实现，性能最高。
* [version14]：引入线程池。

[TCP/IP协议总述]:https://github.com/choyda/choyda-webserver/blob/master/document/TCP%E5%8D%8F%E8%AE%AE%E6%80%BB%E8%BF%B0.md

[version1]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version1"
[version2]: https://github.com/choyda/choyda-webserver/tree/master/version2  "version2"
[version3]: https://github.com/choyda/choyda-webserver/tree/master/version3  "version3"
[version4]: https://github.com/choyda/choyda-webserver/tree/master/version4  "version4"
[version5]: https://github.com/choyda/choyda-webserver/tree/master/version5  "version5"
[version6]: https://github.com/choyda/choyda-webserver/tree/master/version6  "version6"
[version7]: https://github.com/choyda/choyda-webserver/tree/master/version7  "version7"
[version8]: https://github.com/choyda/choyda-webserver/tree/master/version8  "version8"
[version9]: https://github.com/choyda/choyda-webserver/tree/master/version9  "version9"
[version10]: https://github.com/choyda/choyda-webserver/tree/master/version10  "version10"
[version11]: https://github.com/choyda/choyda-webserver/tree/master/version11  "version11"
[version12]: https://github.com/choyda/choyda-webserver/tree/master/version12  "version12"
[version13]: https://github.com/choyda/choyda-webserver/tree/master/version13  "version13"
[version14]: https://github.com/choyda/choyda-webserver/tree/master/version14  "version14"
