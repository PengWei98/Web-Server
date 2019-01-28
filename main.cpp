#include "stdio.h"
//socket相关函数需要
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <thread>
#include <pthread.h>
#include <iostream>
#include <vector>
#include "socketThread.h"
using namespace std;

int SocketThread::id = 0;
bool close_server = false;

int connfd;
vector<int> sockets;
vector<SocketThread*> SocketThreadPointers;

void* acceptConnections(void *data) {
    int sock = *(int*)data;
    
    //监听的端口
    unsigned short bindPort=2409;
    printf("* * * Server with sock %d started! * * *\n", sock);
    
    struct sockaddr_in sockaddr;
    memset(&sockaddr,0,sizeof(sockaddr));
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    sockaddr.sin_port=htons(bindPort);
    
    //开始绑定
    //这里要注意要使用::  这样就调用的全局的bind函数，而不是std中的bind
    ::bind(sock,(struct sockaddr *)&sockaddr,sizeof(sockaddr));
    //监听，最多支持10个连接
    listen(sock, MAXCONNECT);
    
    char clientIp[20];
    printf("Waiting for connections...\n" );
    
    while (true) {
        struct sockaddr_in sockaddrClient;
        int clientl=sizeof(sockaddrClient);
        
        //接受客户端连接的同时，获取客户端的信息
        if((connfd = accept(sock,(struct sockaddr*)&sockaddrClient,(socklen_t *)&clientl))==-1) {
            printf("accpet socket error: %s errno :%d\n",strerror(errno),errno);
        }
        
        //获取端口
        unsigned short port= ntohs(sockaddrClient.sin_port);
        
        //网络Ip=>主机Ip
        inet_ntop(AF_INET,(void *)&sockaddrClient.sin_addr,clientIp,16);
        printf("New client from %s:%d\n", clientIp, port);
        
        //使用线程
        sockets.push_back(connfd);
        SocketThread* st=new SocketThread(connfd, (string)clientIp, (int)port);
        thread t(&SocketThread::run,st);
        SocketThreadPointers.push_back(st);
        t.detach();
    }
}

int main(int argc,char ** argv){
    
    //创建一个socket
    int sock=socket(AF_INET,SOCK_STREAM,0);
    pthread_t serverThread;
    
    // start a server thread for connections.
    if(sock==-1) {
        printf("create socket failed.\n" );
        return 0;
    } else {
        pthread_create(&serverThread, NULL, acceptConnections, (void *)&sock);
    }
    
    // Deal with keyboard events: when 'close' is typed in, the server will shut down.
    string input;
    while(true) {
        cin>>input;
        if(input.compare("close") == 0)
            break;
    }
    
    pthread_cancel(serverThread);
    
    // Deal with sockets and threads...
    for(int i=0;i<SocketThreadPointers.size();i++) {
        delete SocketThreadPointers[i];
    }
    sockets.push_back(sock);
    for(int i=0;i<sockets.size();i++) {
        close(sockets[i]);
    }
    cout<<"Server closed."<<endl;

    return 0;
}
