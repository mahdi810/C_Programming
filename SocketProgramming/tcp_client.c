#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT_NUMBER 50012

int main(){
    //creating the socket 
    int network_socket; 
    network_socket = socket(AF_INET, SOCK_STREAM, 0);  //socket(domain_of_socket, tcp_socket(sock_stream), protocol)
    
    //specify an address for the socket 
    struct sockaddr_in server_address; 
    server_address.sin_family = AF_INET;  //ineternet 
    server_address.sin_port = htons(PORT_NUMBER); //specify the port 
    server_address.sin_addr.s_addr = INADDR_ANY; //connect to localhost 

    //connect 
    if (connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        printf("the connection was not done properly \n"); 
    }

    //receive 
    char server_response[256]; 
    recv(network_socket, &server_response, sizeof(server_response), 0);
    
    //printing the received data; 
    printf("the server sent: %s \n", server_response); 

    //send 

    //close the socket 
    close(network_socket); 

    return 0; 
}