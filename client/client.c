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

        struct send_msg sendmsg;
        sendmsg.msg_type = CMD_SEND;
        sendmsg.file_size = size;
        sprintf(sendmsg.filename, filename);

        if(send(sock, &sendmsg, sizeof(sendmsg), 0) < 0){

            perror("send failed");
            close(sock);
            exit(1);

        }

        printf("Sent file name: %s\n", filename);
        printf("Sent file length = %d\n", size);

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

        int totalSize = 0;

        int fd = open(filename, O_RDONLY);

        while(totalSize < sendmsg.file_size)
        {
            struct data_msg datamsg;

            datamsg.msg_type = CMD_SEND;
            datamsg.data_length = read(fd, &datamsg.buffer, MAX_DATA_SIZE);

            if(send(sock, &datamsg, sizeof(datamsg), 0) < 0){

                perror("send failed");
                close(sock);
                exit(1);

            }

            printf("Sent %d bytes from file\n", datamsg.data_length);

            totalSize += datamsg.data_length;
        }

        printf("Wrote %d bytes to remote server\n", totalSize);

        close(fd);

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

        struct send_msg sendmsg;
        sendmsg.msg_type = CMD_RECV;
        sendmsg.file_size = 0;
        sprintf(sendmsg.filename, filename);

        if(send(sock, &sendmsg, sizeof(sendmsg), 0) < 0){

            perror("send failed");
            close(sock);
            exit(1);

        }

        printf("Sent file name: %s\n", filename);

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

        if(respmsg.status != 0){

            errno = respmsg.status;
            perror("Error opening file on server");
            close(sock);
            exit(1);

        }

        printf("Received file size: %d\n", respmsg.file_size);

        int fd = open(sendmsg.filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

        int totalSize = 0;

        while(totalSize < respmsg.file_size)
        {

            struct data_msg datamsg;
            int rval;
            if( (rval = recv(sock, &datamsg, sizeof(datamsg), 0)) < 0 ){
                perror("reading stream message error");
                close(fd);
                close(sock);
                exit(1);
            }
            else if(rval == 0){
                printf("Ending connection.\n");
                close(fd);
                close(sock);
                exit(1);
            }

            write(fd, &datamsg.buffer, datamsg.data_length);

            printf("Read %d bytes\n", datamsg.data_length);

            totalSize += datamsg.data_length;
        }

        printf("Wrote %d bytes to output file\n", totalSize);

        close(fd);
        close(sock);

    }


}