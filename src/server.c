#define _GNU_SOURCE

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 8181

int main()
{
	int server_fd;
    int new_socket;
    ssize_t valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };

	if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
    {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
    {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    
    int maxSockets = 10;
    int numSockets = 0;
    int sockets[maxSockets];

    while(1)
    {
        new_socket = accept4(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen, SOCK_NONBLOCK);
        if ((new_socket < 0) && (errno != EAGAIN) && (errno != EWOULDBLOCK))
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            if (new_socket > 0)
            {
                sockets[numSockets++] = new_socket;
                printf("Connection from %s:%d\n", inet_ntoa(address.sin_addr), (int) ntohs(address.sin_port) );
            }
        }
        
        for (int i = 0; i < numSockets; i++)
        {
            memset(buffer, 0, 1024);
            valread = read(sockets[i], buffer, sizeof(buffer));
            if (valread < 0)
            {
                if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
                {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if (valread > 0)
                {
                    printf("#%d: %s", i, buffer);
                }
            }
        }
    }

	close(new_socket);
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}
