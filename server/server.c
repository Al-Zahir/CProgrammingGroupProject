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

    int rval;
    if( (rval = recv(mysock, &sendmsg, sizeof(sendmsg), 0)) < 0 ){
        perror("reading stream message error");
        close(mysock);
        exit(1);
    }
    else if(rval == 0){
        printf("Ending connection.\n");
        close(mysock);
        exit(1);
    }
    
    printf("Receive file: %s\n", sendmsg.filename);
    printf("Receive file length: %d\n", sendmsg.file_size);

    struct resp_msg respmsg;
    respmsg.msg_type = CMD_SEND;
    respmsg.status = 0;
    respmsg.file_size = sendmsg.file_size;

    if(send(mysock, &respmsg, sizeof(respmsg), 0) < 0){

        perror("send failed");
        close(mysock);
        exit(1);

    }

    int fd = open(sendmsg.filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

    int totalSize = 0;

    while(totalSize < sendmsg.file_size)
    {

        struct data_msg datamsg;
        int rval;
        if( (rval = recv(mysock, &datamsg, sizeof(datamsg), 0)) < 0 ){
            perror("reading stream message error");
            close(fd);
            close(mysock);
            exit(1);
        }
        else if(rval == 0){
            printf("Ending connection.\n");
            close(fd);
            close(mysock);
            exit(1);
        }

        write(fd, &datamsg.buffer, datamsg.data_length);

        printf("Read %d bytes\n", datamsg.data_length);

        totalSize += datamsg.data_length;
    }

    printf("Wrote %d bytes to output file\n", totalSize);

    close(fd);
    close(mysock);

}