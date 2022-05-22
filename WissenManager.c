#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>

#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 512


static int tcpSend(const char* sendbuf)
{
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        return -1;
    }
    send(sock_cli, sendbuf, strlen(sendbuf),0); ///发送
    close(sock_cli);

    return 0;

}

static void* handleRecvCmd(void* buf)
{

    const char* cmd = (char*) buf;
    if(strncmp("wissen_se", cmd, 9) == 0){
        tcpSend(cmd+10);
    } else {
        system(cmd);
    }

    return NULL;
}


int main()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    printf("监听%d端口\n",MYPORT);
    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    while(1)
    {

        ///成功返回非负描述字，出错返回-1
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        if(conn<0)
        {
            perror("connect");
            exit(1);
        }
        printf("客户端成功连接\n");

        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);
        if (len <= 0) {
            close(conn);
            continue;
        }
        printf("来自客户端数据：%s\n",buffer);

        pthread_t tId;
        if (pthread_create(&tId, NULL, handleRecvCmd, (void*)buffer) < 0)
        {
            system("aplay /wissen/ws_ld.wav");
        }
        close(conn);
    }
    close(server_sockfd);
    return 0;
}

