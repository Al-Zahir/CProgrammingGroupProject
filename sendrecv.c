#include "proj.h"

void recieve_msg(void *recv_struct, int size, int sock){

    int rval;
    if( (rval = recv(sock, recv_struct, size, 0)) < 0 ){
        perror("reading stream message error");
        close(sock);
        exit(1);
    }
    else if(rval == 0){
        printf("Ending connection.\n");
        close(sock);
        exit(1);
    }

}

void send_msg(int struct_msg, int msg_type, int sock, int size, char filename[], int status){

    if(struct_msg == CMD_SEND){

        struct send_msg sendmsg;
        sendmsg.msg_type = msg_type;
        sendmsg.file_size = size;
        strcpy(sendmsg.filename, filename);

        if(send(sock, &sendmsg, sizeof(sendmsg), 0) < 0){

            perror("send failed");
            close(sock);
            exit(1);

        }

    }else if(struct_msg == CMD_RESP){

        struct resp_msg respmsg;
        respmsg.msg_type = msg_type;
        respmsg.status = status;
        respmsg.file_size = size;

        if(send(sock, &respmsg, sizeof(respmsg), 0) < 0){

            perror("send failed");
            close(sock);
            exit(1);

        }

    }

}

void send_data(int sock, char filename[], int size){

    int totalSize = 0;

    int fd = open(filename, O_RDONLY);

    while(totalSize < size)
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

    close(fd);

}

void recv_data(int sock, char filename[], int size){

    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

    int totalSize = 0;

    while(totalSize < size)
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


}