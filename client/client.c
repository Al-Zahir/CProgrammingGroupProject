#include "../proj.h"

int main(int argc, char* argv[]){

    if(argc != 2){

        perror("Please enter exactly 2 arguments. \
            \nExample ./client cs for client to server. \
            \nExample ./client sc for server to client.");
        exit(1);

    }

    char address[100];

    printf("Enter IP address of destination => ");
    fgets(address, 100, stdin);

    char port[100];

    printf("Enter port of destination => ");
    fgets(port, 100, stdin);

    if(strcmp(argv[1], "cs") == 0){

        char filename[100];

        printf("Enter name of file to send => ");
        fgets(filename, 100, stdin);

        filename[strlen(filename) - 1] = '\0';

        FILE *fp = fopen(filename, "r");
        if( fp == NULL )  {
            perror ("Error opening file");
            exit(1);
        }

        fseek(fp, 0, SEEK_END);

        int size = ftell(fp);

        fclose(fp);

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

        send_msg(CMD_SEND, CMD_SEND, sock, size, filename, 0);

        printf("Sent file name: %s\n", filename);
        printf("Sent file length = %d\n", size);

        struct resp_msg respmsg;
        recieve_msg(&respmsg, sizeof(respmsg), sock);

        send_data(sock, filename, size);
        printf("Wrote %d bytes to remote server\n", size);

        close(sock);

    }else{

        char filename[100];

        printf("Enter name of file to receive => ");
        fgets(filename, 100, stdin);

        filename[strlen(filename) - 1] = '\0';

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

        send_msg(CMD_SEND, CMD_RECV, sock, 0, filename, 0);

        printf("Sent file name: %s\n", filename);

        struct resp_msg respmsg;
        recieve_msg(&respmsg, sizeof(respmsg), sock);

        if(respmsg.status != 0){

            errno = respmsg.status;
            perror("Error opening file on server");
            close(sock);
            exit(1);

        }

        printf("Received file size: %d\n", respmsg.file_size);

        recv_data(sock, filename, respmsg.file_size);

        close(sock);

    }


}