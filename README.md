## 用c打造自己的web服务器。

* 终端client, server通信， 单进程。
* 终端client, server通信， 多进程处理。添加warp.c，容错处理文件。
* 终端client，server通信，多线程。
* 改变server服务器为守护进程，不依赖于终端。
* 浏览器和server交互，http解析，支持get、post方式，处理静态文件(包括html, css, js, 图片等)。
* 浏览器和server交互，支持php文件，用cgi的方式处理php。
* http长连接处理，心跳包监测机制。
* 支持php-fpm
* 添加配置文件
* 支持日志存储，查询。
* 多路io复用epoll (select, poll, epoll )，实现方式差不多，直接用epoll，性能最高。
* 引入线程池。