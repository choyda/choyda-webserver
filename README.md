
#### 用C打造自己的WEB服务器  
> 从最简单最基础的代码开始，到最后实现一个高性能的WEB服务器。  
> 主要用于学习交流、对知识点的总结。  
> 每个版本迭代更新，会增加对应的扩展功能。  
> 每个版本的README.md会记录扩展功能所需要的知识要点，注意事项，和一些基础理论。  
> 因本人所学有限，代码或解释可能会有偏差和误解，也希望大牛们能给出宝贵的意见和建议。  
> 也希望能帮助正在学习这方面内容的同学，大家共同进步。    
>PS：添加扩展功能可能会随着开发和迭代有所调整。目前先给出一个大纲。  


* [version1]：Linux命令行模式下Client和Server通信，单进程。
* [version2]：实现多进程，同时可以处理多个客户端同时请求。
* [version3]：对一些库函数出错处理进行封装，自定义各种wrap.c文件。
* [version4]：实现多线程处理通信。
* [version5]：改变Server服务器为守护进程，不依赖于终端。
* [version6]：浏览器和Server交互，添加处理HTTP解析的自定义函数，借鉴于《深入理解计算机系统》中的csapp.c文件。
* [version7]：支持GET、POST方式，处理静态文件(包括HTML、CSS、JS、各种图片等)。
* [version8]：浏览器和Server交互，支持PHP文件访问，用CGI的方式处理PHP。
* [version9]：HTTP长连接处理，心跳包监测机制。
* [version10]：添加配置文件，主要实现配置PHP-FPM处理，和一些其他的参数配置。
* [version11]：支持PHP-FPM处理。
* [version12]：支持日志访问日志存储。
* [version13]：多路IO复用(select、poll、epoll)，用epoll实现，性能最高，其他的方式实现可以参考其他资料。
* [version14]：引入线程池。


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