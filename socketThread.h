#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>

#define MAXCONNECT  10
#define BUF_SIZE    1024
#define LOGIN_LENGTH 10
#define PASS_LENGTH 4
#define POST_LENGTH 4
#define DEPOST_LENGTH 6

using namespace std;

class SocketThread{
public:
    static int id;
private:
    int sock;
    string clientIp;
    int port;
    int myId;
public:
    SocketThread(int sock, string ip, int port){
        this->sock = sock;
        this->clientIp = ip;
        this->port = port;
        this->myId = id++;
    }
    
    ~SocketThread(){cout<<"Client "<<myId<<" closed."<<endl;}
    
    inline bool is_get(char* recvBuff) {return !strncmp(recvBuff, "GET", 3);}
    
    inline bool is_post(char* recvBuff) {return !strncmp(recvBuff, "POST", 4);}
    
    // parse GET and write replies in sendBuff.
    void deal_with_get(char *recvBuff, char *sendBuff) {
        char *name = recvBuff + 5;
        char *space = strchr(name, ' ');
        *space = '\0';
        string path;
        string header = "HTTP/1.1 ";
        bool isImage = false;
        
        if(strcmp(name, "test.html") == 0) {
            path = "./files/html/test.html";
            header += "200 OK\r\nContent-type: text/html\t\nContent-length: ";
        } else if(strcmp(name, "noimg.html") == 0) {
            path = "./files/html/noimg.html";
            header += "200 OK\r\nContent-type: text/html\t\nContent-length: ";
        } else if(strcmp(name, "test.txt") == 0) {
            path = "./files/txt/test.txt";
            header += "200 OK\r\nContent-type: text/html\t\nContent-length: ";
        } else if(strcmp(name, "logo.png") == 0 || strcmp(name, "img/logo.png") == 0) {
            path = "./files/img/logo.png";
            header += "200 OK\r\nContent-type: image/png\t\nContent-length: ";
            isImage = true;
        } else {
            path = "";
            header += "404 Not Found\r\nContent-type: text/html\r\nContent-length: 0\r\n\r\n";
        }

        if(path.length()) {
            ifstream t(path);
            stringstream buffer;
            buffer << t.rdbuf();
            std::string contents(buffer.str());
            header += to_string(contents.length()) + "\r\n\r\n";
            header += contents;
        }
        
        if(isImage) {
            // Send image via write, because it's tooooooo big...
            write(sock, header.c_str(), header.length());
            cout<<"Reply is sent.\n"<<endl;
        } else {
            strcpy(sendBuff, header.c_str());
            // Send messages.
            if(send(sock, sendBuff, strlen(sendBuff), 0) < 0)
                cout<<"Your reply is not sent.\n"<<endl;
            else
                cout<<"Reply is sent.\n"<<endl;
        }
    }
    
    void sendResponse(char messege[], int state, char *sendBuff){
        string len(to_string((int)strlen(messege)));
        string msg;
        memset(sendBuff, 0, strlen(sendBuff));

        if (state==404) {
            msg = "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\nContent-Length: " + len + "\r\n\r\n" + messege;
            strcpy(sendBuff, msg.c_str());
        }
        if(state==200){
            msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + len + "\r\n\r\n" + messege;
            strcpy(sendBuff, msg.c_str());
        }
        
        // Send messages.
        if(send(sock, sendBuff, strlen(sendBuff), 0) < 0)
            cout<<"Your reply is not sent.\n"<<endl;
        else
            cout<<"Reply is sent.\n"<<endl;
    }
    
    
    int getLine(char buf[], char line[]){
        int i = 0;
        while(i < BUF_SIZE){
            if(buf[i] == '\r'){
                if(buf[i + 1] == '\n'){
                    memcpy(line, buf, i);
                    break;
                }
            }
            i++;
        }
        line[i] = '\0';
        return i; //'\0' is not included!
    }
    
    void readMessege(char body[], char *sendBuff){
        int j= 0;
        //to get rid of the space
        for(int i=0;i<strlen(body);i++)
            if(body[i] != ' ') body[j++]=body[i];
        body[j]='\0';
        char login[LOGIN_LENGTH + 1];
        char pass[PASS_LENGTH + 1];
        memcpy(login, body + 6, LOGIN_LENGTH);
        login[LOGIN_LENGTH] = '\0';
        memcpy(pass, body + 22, PASS_LENGTH);
        pass[PASS_LENGTH] = '\0';
        printf("%s\n", login);
        printf("%s\n",pass);
        
        if(strcmp(login, "3160104618") == 0 && strcmp(pass, "4618") == 0) {
            //login success
            printf("login success!\n");
            char msg[BUF_SIZE] = "<html><body>login success!</body></html>";
            sendResponse(msg, 200, sendBuff);
        } else{
            //login failure
            char msg[BUF_SIZE] = "<html><body>login failure!</body></html>";
            sendResponse(msg, 200, sendBuff);
        }
    }
    
    // parse POST and write replies in sendBuff.
    void deal_with_post(char *recvBuff, char *sendBuff) {
        char line[BUF_SIZE];
        //    char response[BUF_SIZE] = "<html><body>";
        int lineLength = getLine(recvBuff, line);
        int i = POST_LENGTH;
        while(line[i] != '\0'){
            if(line[i] == ' '){
                i++;
            }
            else{
                char path[DEPOST_LENGTH + 1];
                memcpy(path, line+i+1, DEPOST_LENGTH);
                //to detect whether the file path is "depost"
                if(strcmp(path, "dopost") != 0){
                    //                    printf("%s", path);
                    //to do:error control
                    char msg[BUF_SIZE] = "0";
                    sendResponse(msg, 404, sendBuff);
                    //                    printf("error\n");
                    return;
                }
                else{
                    
                    //to get rid of the request head
                    int sumLength=lineLength + 2;
                    do{
                        lineLength = getLine(recvBuff + sumLength, line);
                        if(lineLength==0){
                            sumLength += 2;
                            break;
                        }
                        sumLength+=(lineLength + 2);
                        
                    }while(1);
                    //to do read the messege
                    char body[BUF_SIZE];//store the messege
                    strcpy(body, recvBuff + sumLength);
                    readMessege(body, sendBuff);
                }
                break;
            }
            
        }
    }
    
    // Handle other requests...
    void deal_with_others(char *recvBuff, char *sendBuff) {}

    void run(){
        int n=0;
        char recvBuff[BUF_SIZE];
        char sendBuff[BUF_SIZE];
        
        for(;;) {
            n = (int)recv(this->sock,recvBuff,1024,0);
            // 在非阻塞模式下不代表关闭连接，在阻塞模式下表示关闭
            if(n<=0) {
                break;
            }
            recvBuff[n] = '\0';
            printf("Client %d >> %s", this->myId, recvBuff);
            
            // Parse get / post requests, and write replies in sendBuff.
            if(is_get(recvBuff)) {
                deal_with_get(recvBuff, sendBuff);
            } else if(is_post(recvBuff)) {
                deal_with_post(recvBuff, sendBuff);
            } else {
                deal_with_others(recvBuff, sendBuff);
            }
        }
        close(this->sock);
    }
};

#endif
