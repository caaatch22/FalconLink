#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>

#include "../../falconlink/include/util.hpp"
#include "../../falconlink/include/poller.hpp"
#include "../../falconlink/include/inet_addr.hpp"
#include "../../falconlink/include/socket.hpp"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
void handleReadEvent(int);
using namespace falconlink;

int main() {
    Socket *serv_sock = new Socket();
    InetAddr *serv_addr = new InetAddr("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Poller *ep = new Poller();
    serv_sock->setNonBlock();
    ep->addFd(serv_sock->fd(), EPOLLIN | EPOLLET);
    while(true){
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for(int i = 0; i < nfds; ++i){
            if(events[i].data.fd == serv_sock->fd()){        //新客户端连接
                InetAddr *clnt_addr = new InetAddr();      //会发生内存泄露！没有delete
                Socket *conn_sock = new Socket(serv_sock->accept(clnt_addr));       //会发生内存泄露！没有delete
                printf("new client fd %d! IP: %s Port: %d\n", conn_sock->fd(),
                       inet_ntoa(clnt_addr->addr_.sin_addr),
                       ntohs(clnt_addr->addr_.sin_port));
                conn_sock->setNonBlock();
                ep->addFd(conn_sock->fd(), EPOLLIN | EPOLLET);
            } else if(events[i].events & EPOLLIN){      //可读事件
                handleReadEvent(events[i].data.fd);
            } else{         //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}
