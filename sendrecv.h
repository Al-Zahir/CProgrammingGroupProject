void send_msg(int struct_msg, int msg_type, int sock, int size, char filename[], int status);
void recieve_msg(void *recv_struct, int size, int sock);
void send_data(int sock, char filename[], int size);
void recv_data(int sock, char filename[], int size);