# CProgrammingGroupProject

Group Project: Networking with TCP. Create a client application and a server application to copy a file on the client computer to a file on the server computer and vice versa. This exercise mimics the UNIX/Linux rcp – remote copy – command that allows users to copy files from and to remote servers. Your programs will also bypass all the security constraints built into the rcp command. You WILL be strictly graded on HOW WELL YOU FOLLOW DIRECTIONS! Do not take any shortcuts. This project does NOT utilize threading.

All reads and writes of file contents will be done in blocks. Define the block size as 1024 bytes. (Be sure to test your program with files larger than the block size.)

Create a server that: 1) prompts the user for an address and a port on which to listen (or accepts these as command line parameters); 2) enters an infinite loop that 3) accepts incoming connections; 4) reads the input stream for the type of copy (client-to-server or server-to-client) and sends the proper response to the client, 5) if the copy type is from client to server: enters a loop that reads the network input for the file contents, and writes the bytes read to the output file; if the copy type is from server to client: enters a loop that writes the file contents to the network output,  and 6) when done closes the file and the remote sd. You MUST catch all possible exceptions at EVERY STEP, and if an exception occurs you must terminate the TCP connection (using close(2) on the remote socket returned by the accept(2) call) AND close the file before awaiting the next incoming request. Sample server dialog for the successful copy from the client to the server:

Enter address to bind: 127.0.0.1
Enter port to listen => 5000
Connection accepted from /127.0.0.1
Receive file: test.txt
Receive file length: 7414
Read 1024 bytes
Read 1024 bytes
Read 1024 bytes
Read 1024 bytes
Read 1024 bytes
Read 1024 bytes
Read 1024 bytes
Read 246 bytes
Wrote 7414 bytes to output file

Create a client that: 1) prompts the user for the remote server IP address, the remote server port, the type of copy to do (client-to-server or server-to-client) and the name of the file to copy (or accepts these as command line parameters); 2) if the copy is from the client to the server: sends the server the copy message type (i.e., send), and the name and size of the file of the file to be sent; if the copy is from the server to the client sends the copy message type (i.e., receive) and the name of the file, 3) awaits the servers response, 4) if the response has status = OK, enters a loop that either writes blocks to the remote server from the file, or writes from the remote server to the file, and 5) when done closes the file and socket before exiting. Sample client dialog for a successful copy from the client to the server:

Enter IP address of destination => 127.0.0.1
Enter port of destination => 5000
Enter name of file to send => test.txt
Connection accepted from localhost
Sent file name: test.txt
Sent file length = 7414
Sent 1024 bytes from file
Sent 1024 bytes from file
Sent 1024 bytes from file
Sent 1024 bytes from file
Sent 1024 bytes from file
Sent 1024 bytes from file
Sent 1024 bytes from file
Sent 246 bytes from file
Wrote 7414 bytes to remote server

If you want to use the loopback address make sure that your client and server are executing in different current working directories!

You will need to define several message buffers and message types:

MAX_DATA_SIZE – 1024

CMD_SEND – message type for sending a file from the client to the server
CMD_RECV – message type for receiving a file from the server to the client

struct send_msg {
   int msg_type;               // CMD_SEND or CMD_RECV
   int file_size;                  // size of file to be sent (if applicable; if not set it to 0)
   char filename[128];  // name of file
};

CMD_RESP – message type for command responses; sent by server to client after receiving a CMD_SEND or CMD_RECV message

struct resp_msg {           // handshake message
   int msg_type;               // either CMS_SEND or CMD_RECV
   int status;                     // 0 = OK, otherwise put the errno here
   int file_size;                 // used only by server when transferring a file from the
                                           // server to the client and status is OK
}

CMD_DATA – message type for data transfer (in either direction)

struct data_msg {
   int msg_type;              // CMD_DATA
   int data_leng;              // length of data in buffer
   char buffer[1024];
}

Design: you are expected to turn in a design for this project.

Manual page: you are expected to turn in a man page for this project that explains how to invoke the server and the client (it does not have to look like a standard C language man page, but it should cover the NAME, SYNOPSIS, and DESCRIPTION sections of a C language man page).

Design recommendations: put all of the includes in a single  .h file (say proj.h) and also include all the #define constants and the structure definitions (see above). Then, in every .c file just #include “proj.h” (and yes, the “s are required).

There are several routines that are shared by both the client and the server:
receive_msg() – receives a single message from the network
send_mesg() – sends a single message to the network
send_data() – loops through reads on the file and sends 1024 blocks to the network
recv_data() – loops through reads on the network and writes the blocks received to the file
Put all of these routines in a separate .c file (say sendrecv.c); compile this file to a .o (object) file:
$ cc –c sendrecv.c
Then compile your server and client with this file:
$ cc server.c sendrecv.o –o server
$ cc client.c sendrecv.o –o client
