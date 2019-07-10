#include <netinet/in.h>  //sockaddr_in
#include <arpa/inet.h>

//libevent
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include "log.h"
#include "http.h"

using namespace std;


void accept_conn_cb(evconnlistener *listener, evutil_socket_t fd, 
		sockaddr *address, int socklen, void *ctx)
{
	event_base *base = evconnlistener_get_base(listener);
	Http* http = Http::create(base, fd);
	DEBUG_LOG("http: %p", http);
	http->run(http);

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(((sockaddr_in*)address)->sin_addr), 
			ip, sizeof(ip));
	DEBUG_LOG("accept from:%s", ip);
}

void accept_error_cb(evconnlistener *listener, void *ctx)
{
	event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();

	if (err)
	{
		DEBUG_LOG("Got an error %d (%s) on the listener. Shutting down.\n", err, evutil_socket_error_to_string(err));
	}
	event_base_loopexit(base, NULL);
}

void clean_up_child_process(int signal_num)
{
	int stat;
	while (waitpid(-1, &stat, WNOHANG) > 0);
}

int main(int argc, char* argv[])
{
	INIT_LOG("server");

	struct sigaction sigchild_action;
    memset(&sigchild_action, 0, sizeof(sigchild_action));
    sigchild_action.sa_handler = &clean_up_child_process;
    sigaction(SIGCHLD, &sigchild_action, NULL);

	event_base* base = event_base_new();
	if(base == NULL)
	{
		DEBUG_LOG("get event_base error!");
		return 0;	
	}

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sockaddr_in));	
	sin.sin_family = AF_INET;
	if(argc == 2)
	{
		sin.sin_port = htons(atoi(argv[1]));
	}
	else
	{
		sin.sin_port = htons(8081);
	}

	sin.sin_addr.s_addr = htonl(INADDR_ANY);

<<<<<<< HEAD
=======
  //evconnlistener其实是对even_base和event的封装而已。
  
  /*
   *一系列的工作函数，因为listener可以用于不同的协议。
   struct eventlistener_ops{
     int (*enable)(stuct evconnlistener*);
     int (*disable)(stuct evconnlistener*);
     void (*destroy)(struct evconnlistener*);
     void (*shutdown)(stuct evconnlistener*);
     evutil_socket_t (*getfd)(struct evconnlistener*);
     stuct event_base *(*getbase)(sturct evconnlistener*);
   };

   struct evconnlistener{
     const struct evconnlistener_ops *ops;  //操作函数
     void *lock;                            //锁变量，用于线程安全
     evconnlistener_cb cb;                  //用户的回调函数
     evconnlistener_errorcb errorcv;        //发生错误时的回调函数
     void *user_data;                       //回调函数的参数。当回调函数执行的时候，通过形参传入回调函数内部
     unsigned flags;                        //属性标志；例如socket套接字属性，可以是阻塞，非阻塞，reuse等。
     short refcnt;                          //引用计数
     unsigned enabled :1;                   //位域为1，即只需一个比特位来存储这个成员
   };

   struct evconlistener_event{
      struct evconnlistener base;
      stuct event listener;    //内部event插入到event_base,完成监听
   };
   */

>>>>>>> temp
	evconnlistener* listener = evconnlistener_new_bind(base, accept_conn_cb,
		   	0, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE /*| LEV_OPT_THREADSAFE*/, 
			-1, (sockaddr*)&sin, sizeof(sin));
	
	if(listener == NULL)
	{
    ERROR_LOG("listen error!");
		return 0;
	}

	evconnlistener_set_error_cb(listener, accept_error_cb);

	DEBUG_LOG("listening");
<<<<<<< HEAD
	event_base_dispatch(base);

	evconnlistener_free(listener);
=======
  //循环监听红黑树base
	event_base_dispatch(base);
  //释放监听
	evconnlistener_free(listener);
  //释放事件指针
>>>>>>> temp
	event_base_free(base);
	return 0;
}
