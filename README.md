# A C++ High Performance Web Server

## Introduction 

本项目使用C++11编写的Web服务器，解析了get、head请求，可处理静态资源，支持HTTP长连接，支持管线化请求，并实现了异步日志，记录服务器运行状态。

## 涵盖内容 
* TCP,HTTP协议
* 多进程多线程
* IO
* 锁
* 网络通信
* C++语法
* 编码规范
* Linux环境下各种工具的使用
* 版本控制Git

## Envoriment
* OS: WSL2 Ubuntu20.04LTS 
* Compiler: g++ 9.3.0 

## Usage
    ./WebServer [-t thread_numbers] [-p port] [-l log_file_path(should begin with '/')]

## Technical points
* 使用Epoll边沿触发的IO多路复用技术，非阻塞IO，使用Reactor模式
* 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
* 使用基于小根堆的定时器关闭超时请求
* 主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一特定线程中
* 使用eventfd实现了线程的异步唤醒
* 使用双缓冲区技术实现了简单的异步日志系统
* 为减少内存泄漏的可能，使用智能指针等RAII机制
* 使用状态机解析了HTTP请求,支持管线化
* 支持优雅关闭连接

## Model
并发模型为Reactor+非阻塞IO+线程池，新连接Round Robin分配。

