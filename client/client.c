#include "../proj.h"

int main(int argc, char* argv[]){

    char address[100];

    printf("Enter IP address of destination => ");
    fgets(address, 100, stdin);

    char port[100];

    printf("Enter port of destination => ");
    fgets(port, 100, stdin);

    char filename[100];

    printf("Enter filename to send => ");
    fgets(filename, 100, stdin);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(address);
    serverAddr.sin_port = htons(atoi(port));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Socket create error");
        exit(1);
    }

    if(connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0 ){

        perror("connect failed");
        close(sock);
        exit(1);

    }

    printf("Connection accepted from localhost\n");

    printf("Sending this code compiled SENDMSG\n");

    FILE *fp = fopen("test.txt", "r");
    if( fp == NULL )  {
        perror ("Error opening file");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);

    int size = ftell(fp);

    printf("Size: %d\n", size);

    fclose(fp);

    struct send_msg sendmsg;
    sendmsg.msg_type = CMD_SEND;
    sendmsg.file_size = size;
    sprintf(sendmsg.filename, filename);

    if(send(sock, &sendmsg, sizeof(sendmsg), 0) < 0){

        perror("send failed");
        close(sock);
        exit(1);

    }

    printf("Waiting for RECVMSG\n");

    struct resp_msg respmsg;
    int rval;
    if( (rval = recv(sock, &respmsg, sizeof(respmsg), 0)) < 0 ){
        perror("reading stream message error");
        close(sock);
        exit(1);
    }
    else if(rval == 0){
        printf("Ending connection.\n");
        close(sock);
        exit(1);
    }

    printf("RECV: %d, %d, %d\n", respmsg.msg_type, respmsg.status, respmsg.file_size);

    int totalSize = 0;

    int fd = open("test.txt", O_RDONLY);

    printf("Sent file length: %d\n", sendmsg.file_size);

    while(totalSize < sendmsg.file_size)
    {
        struct data_msg datamsg;

        datamsg.msg_type = CMD_SEND;
        datamsg.data_length = read(fd, &datamsg.buffer, MAX_DATA_SIZE);

        printf("Sending %d bytes from file\n", datamsg.data_length);

        if(send(sock, &datamsg, sizeof(datamsg), 0) < 0){

            perror("send failed");
            close(sock);
            exit(1);

        }

        totalSize += datamsg.data_length;
    }

    close(fd);


    close(sock);

}