#include "proj.h"

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

    printf("Sending SENDMSG\n");

    struct send_msg sendmsg;
    sendmsg.msg_type = CMD_SEND;
    sendmsg.file_size = 100;
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

    close(sock);

}