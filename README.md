# http_server

一个简单的http服务器，采用libevent网络库

## 开发流程

使用libevent利器bufferevent

1.struct bufferevent内置了两个event（读/写）和对应的缓冲区。当有数据被读入(input)的时候，readcb被调用，当output被输出完成的时候，writecb被调用，当网络I/O出现错误，如链接中断，超时或其他错误时，errorcb被调用。

 2.使用bufferevent_socket_new创建一个structbufferevent *bev，关联该sockfd，托管给event_base

3.设置读写对应的回调函数

4.启用读写事件,其实是调用了event_add将相应读写事件加入事件监听队列poll。正如文档所说，如果相应事件不置为true，bufferevent是不会读写数据的

5. 进入bufferevent_setcb回调函数：
   在readcb里面从input中读取数据，处理完毕后填充到output中；
writecb对于服务端程序，只需要readcb就可以了，可以置为NULL；
errorcb用于处理一些错误信息。
关于bufferevent很多API都还不是很熟悉，还有libevent 添加事件event_add是非线程安全的，如果使用多线程，需要保证event_add不能出现在多个线程中，以后有时间慢慢研究。

体会：关于源码，还需要好好研究，其实最近挺郁闷的，弄了半天，没有什么进展，现在自己还有很多疑问，主要是自己太急了，不过3天时间做到基本了解，自己还算满意，下一步有时间多研究下吧。

## 使用函数API

| 函数声明 | 使用实例 | 函数功能 |
|-|-|-|
| struct event_base *base |  |  |
| event_base_new(); | | |
| listen signal.. | | |
| event_base_dipatch(base) | | |
| event_base_free(base) | | |
|int bufferevent_enable(**struct bufferevent \*bufev, short event)**|bufferevent_enable(bev, EV_READ\|EV_WRITE);|启用读写事件,其实是调用了event_add将相应读写事件加入事件监听队列poll。正如文档所说，如果相应事件不置为true，bufferevent是不会读写数据的|

libevent库的一般使用方法

1. struct event_base *base;
2. base = event_base_new();  --- epoll_create()   //创建一颗监听红黑树
3. 向base安插事件  listen signal ---epoll_ctl()
4. event_base_dispatch(base);    ----epoll_wait()
5. event_base_free(base)



epoll反应堆--------->libevent库设计核心思想

- 普通epoll : epol1. .create 创建-颗监听红黑树。
  - socket、 bind、 listen -- lfd --- 组织1fd对应结构体(EPOLLIN、 void * (回调函数
    epo1l ct1() 将1fd及对应结构体添加到红黑树进行监听。---- epoll. wait(监听。--- 返回满足条件的个数。----_ 传出参数(结构体数组) ---- 回调。cfd = accpet(满足EPOLLIN 对应的客户端---将cfd及对应结构体(回调函数)
    ----挂到监听红黑树--- 启动epol1, waitO监听--- 返回满足条件的个数。---- 传出参数(结构体数组)
    -----回调: recv客户端发送的小写字符串----转大写--- send回给客户。
- epoll反应堆: epoll. .create 创建-颗监听红黑树。--- socket、 bind、 listen -- 1fd ---组织1fd对应结构体(EPOLLIN、 void * (回调函数)
  epo1l _ct1()将1fd及对应结构体添加到红黑树进行监听。---- epoll. wait()监听。--- 返回满足条件的个数。
  ----传出参数(结构体数组) ---- 回调，cfd = accpetO满足EPOLLIN对应的客户端
  --将cfd及对应结构体(EPOLLIN、void * (回调函数--read) ) ---- 挂到监听红黑树--- 启动epoll mait()监听
  ---返回满足条件的个数。---传出参数(结构体数组) ----回调: recy 客户端发送的小写字符串
  --- cfd从监听红黑树上摘下。---修改它的EPOLLIN --> EPOLLOUT 1修改它的回调函数---write
  ---再次将cid及对应结构体(EPOLLOUT、 void * (回调图数--write) ) ---- 挂到监听红黑树---启动epo11, wait()监听
  ---返回满足条件的个数。---传出参数(结构体数组) ----回调: send ---转大写--- send回给客户.