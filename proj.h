#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

extern int errno;

#define MAX_DATA_SIZE 1024
#define CMD_SEND 0
#define CMD_RECV 1
#define CMD_RESP 2
#define CMD_DATA 3

struct send_msg{

    int msg_type;
    int file_size;
    char filename[128];

};

struct resp_msg{

    int msg_type;
    int status;
    int file_size;

};

struct data_msg{

    int msg_type;
    int data_length;
    char buffer[MAX_DATA_SIZE];

};

void send_msg(int struct_msg, int msg_type, int sock, int size, char filename[], int status);
void recieve_msg(void *recv_struct, int size, int sock);
void send_data(int sock, char filename[], int size);
void recv_data(int sock, char filename[], int size);