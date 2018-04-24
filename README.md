
### 用c打造自己的web服务器  
> 主要用于学习交流、总结。  
> 每个版本迭代更新，会增加对应的扩展功能。  
> 每个版本的README.md会记录扩展功能所需要的知识要点，注意事项，和一些理论思想。  
> 因本人所学有限，代码或解释可能会有偏差和误解，也希望大牛们能给出宝贵的意见和建议。  
> 也希望能帮助正在学习这方面内容的同学，大家共同进步。    
>PS：添加扩展功能可能会随着开发和迭代有所调整。目前先给出一个大纲。



* [version1] 终端client, server通信， 单进程。
* [version2] 终端client, server通信， 多进程处理。添加warp.c，容错处理文件。
* [version3] 终端client，server通信，多线程。
* [version4] 改变server服务器为守护进程，不依赖于终端。
* [version5] 浏览器和server交互，http解析，支持get、post方式，处理静态文件(包括html, css, js, 图片等)。
* [version6] 浏览器和server交互，支持php文件，用cgi的方式处理php。
* [version7] http长连接处理，心跳包监测机制。
* [version8] 支持php-fpm
* [version9] 添加配置文件
* [version10] 支持日志存储，查询。
* [version11] 多路io复用epoll (select, poll, epoll )，实现方式差不多，直接用epoll，性能最高。
* [version12] 引入线程池。


[version1]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version1"
[version2]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version2"
[version3]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version3"
[version4]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version4"
[version5]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version5"
[version6]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version6"
[version7]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version7"
[version8]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version8"
[version9]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version9"
[version10]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version10"
[version11]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version11"
[version12]: https://github.com/choyda/choyda-webserver/tree/master/version1  "version12"