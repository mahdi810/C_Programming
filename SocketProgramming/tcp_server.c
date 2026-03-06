#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT_NUMBER 50012

int main(){
    /*
    socket creation
    binding 
    listening 
    accepting 
    */

    //socket creation 
    int server_socket; 
    server_socket = socket(AF_INET, SOCK_STREAM, 0); 

    //define the server address 
    struct sockaddr_in server_address; 
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(PORT_NUMBER); 
    server_address.sin_addr.s_addr = INADDR_ANY; 

    //bind 
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)); 

    //listen to connections 
    listen(server_socket, 1); //1 means only for one connection is listned. 

    int client_socket; 
    client_socket = accept(server_socket, NULL, NULL); //NULLs gives you the address of the client. 

    //send 
    char server_message[256] = "hello and how are, this is the server talking"; 
    send(client_socket, server_message, sizeof(server_message),0);
    
    //close the socket 
    close(server_socket); 

    return 0; 
}