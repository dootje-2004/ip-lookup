#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8181

int main()
{
    int client_fd;
    ssize_t valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		perror("Could not create socket");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
		perror("Invalid or unsupported address");
		return -1;
	}

	if ((connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
		perror("Connection failed");
		return -1;
	}
    
    int running = 1;

    while(running)
    {
        memset(buffer, 0, 1024);
        valread = read(STDIN_FILENO, buffer, 1024);
        printf("%ld characters read\n", valread);
	    send(client_fd, buffer, valread, 0);
	    printf("%s", buffer);
        if (memcmp(buffer, "quit\n", 5) == 0) running = 0;
    }

	close(client_fd);
	return 0;
}
