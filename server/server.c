#include "../proj.h"

int main(int argc, char* argv[]){

    char address[100];

    printf("Enter address to bind: ");
    fgets(address, 100, stdin);

    char port[100];

    printf("Enter port to listen => ");
    fgets(port, 100, stdin);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(address);
    serverAddr.sin_port = htons(atoi(port));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Socket create error");
        exit(1);
    }

    if(bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0){
        perror("Bind error");
        exit(1);
    }

    listen(sock, 5);

    struct sockaddr_in addr;
    socklen_t fromlen = sizeof(addr);
    int mysock = accept(sock, (struct sockaddr *)&addr, &fromlen);

    if(mysock == -1){

        perror("accept failed");
        close(mysock);
        exit(1);

    }

    printf("Connection accepted from /%s", address);

    struct send_msg sendmsg;
    recieve_msg(&sendmsg, sizeof(sendmsg), mysock);

    
    if(sendmsg.msg_type == CMD_SEND)
    {
        printf("Receive file: %s\n", sendmsg.filename);
        printf("Receive file length: %d\n", sendmsg.file_size);

        send_msg(CMD_RESP, CMD_SEND, mysock, sendmsg.file_size, NULL, 0);

        recv_data(mysock, sendmsg.filename, sendmsg.file_size);
        
        close(mysock);
    } else {
        int fileExists = 1;

        FILE *fp = fopen(sendmsg.filename, "r");
        if( fp == NULL )  {
            perror ("Error opening file");
            fileExists = 0;
        }

        int size = 0;
        if(fileExists){

            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fclose(fp);
            printf("Sending file: %s\n", sendmsg.filename);
            printf("Sending file length: %d\n", size);

        }

        send_msg(CMD_RESP, CMD_RECV, mysock, size, NULL, errno);

        if(!fileExists){
            close(mysock);
            exit(1);
        }

        send_data(mysock, sendmsg.filename, size);
        printf("Wrote %d bytes to remote client\n", size);

        close(mysock);
    }

}