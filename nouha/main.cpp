#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

int main()
{
    int server_fd;
    int new_socket;
    //create a socket AF_INET for IP domaine 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("cannot create socket");
        return (0);
    }
    // identify a socket mean assigning port IP to the socket
    struct sockaddr_in address; // this sturtc is defined s now we need to fil it with some info
    const int PORT = 8080;

    memset((char *)&address, 0, sizeof(address));
    address.sin_addr.s_addr = htonl(INADDR_ANY);  // you ip address . the address for this socket
    address.sin_family = AF_INET; //The address family we used 
    address.sin_port = htons(PORT);   // the port 

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        return (0);
    }

    // wait for an incoming connection
    // 3 is the number of pending connection before refuse
    if (listen(server_fd, 3) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    int addrlen = sizeof(address);
    char hello[] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 21\n\n<h1>Hello world!</h1>";
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("IN accpet");
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        long valread = read(new_socket, buffer, 30000);
        printf("%s\n",buffer );
        write(new_socket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }
    close(server_fd);
}