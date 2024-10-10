/*
			NETWORK PROGRAMMING WITH SOCKETS

In this program we illustrate the use of Berkeley sockets for interprocess
communication across the network. We show the communication between a server
process and a client process.


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
/* THE SERVER PROCESS */

char *receve_str(int sockfd, char buff[])
{
    int n = 0;
    for (int i = 0; i < 100; i++)
        buff[i] = '\0';
    n = recv(sockfd, buff, 100, 0);
    if (strcmp(buff, "NULL") != 0)
    {
        // printf("%d\n", n);
        printf("%s", buff);
        while (buff[48] != '\0')
        {
            if (strcmp(buff, "NULL") == 0)
                break;
            for (int i = 0; i < 100; i++)
                buff[i] = '\0';
            recv(sockfd, buff, sizeof(buff)+1, 0);
            printf("%s", buff);
        }
        printf("\n");
    }
    return buff;
}


void send_str(char str1[], char buff[], int newsockfd)
{
    for (int j = 0; j < 50; j++)
        buff[j] = '\0';
    int i = 0;
    int n;
    int k = 0;
    while (str1[k] != '\0')
    {
        buff[i] = str1[k];
        if (buff[i] == '\0')
        {
            buff[i] = '\0';
            break;
        }

        i++;
        k++;
        if (i == 49)
        {
            i = 0;
            n = 0;
            // printf("%s\n", buff);
            n = send(newsockfd, buff, strlen(buff) + 1, 0);
            // printf("%d\n", n);
            for (int j = 0; j < 50; j++)
                buff[j] = '\0';
        }
    }
    // for (int j = 0; j < 50; j++)
    // buff[j] = '\0';
    // printf("%s\n", buff);

    n = send(newsockfd, buff, strlen(buff) + 1, 0);
    // printf("%d\n", n);
    for (int j = 0; j < 50; j++)
        buff[j] = '\0';
}

int main(int argc, char **argv)
{
	int server1, server2, server3, sockfd, newsockfd; /* Socket descriptors */
	int clilen, rc, nfds;
	struct sockaddr_in cli_addr, lb_addr, lb_addr_cli[2];
	time_t time_before_poll = 0, time_after_poll =0;
	// struct pollfd fds;
	int i;
	int val1, val2;
	char buf[100]; /* We will use this buffer for communication */

	/* The following system call opens a socket. The first parameter

	   indicates the family of the protocol to be followed. For internet
	   protocols we use AF_INET. For TCP sockets the second parameter
	   is SOCK_STREAM. The third parameter is set to 0 for user
	   applications.
	*/

	// creating socket for
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Cannot create socket\n");
		exit(0);
	}

	if ((server1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Cannot create socket\n");
		exit(0);
	}

	if ((server2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Cannot create socket\n");
		exit(0);
	}

	// if ((server3 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	// {
	// 	printf("Cannot create socket\n");
	// 	exit(0);
	// }

	// printf("%d %d %d %d\n", sockfd, server1, server2, server3);

	/* The structure "sockaddr_in" is defined in <netinet/in.h> for the
	   internet family of protocols. This has three main fields. The
	   field "sin_family" specifies the family and is therefore AF_INET
	   for the internet family. The field "sin_addr" specifies the
	   internet address of the server. This field is set to INADDR_ANY
	   for machines having a single IP address. The field "sin_port"
	   specifies the port number of the server.
	*/

	// for clinet
	lb_addr.sin_family = AF_INET;
	lb_addr.sin_addr.s_addr = INADDR_ANY;
	lb_addr.sin_port = htons(atoi(argv[1]));

	// for server
	lb_addr_cli[0].sin_family = AF_INET;
	inet_aton("127.0.0.1.", &lb_addr_cli[0].sin_addr);
	lb_addr_cli[0].sin_port = htons(atoi(argv[2]));

	lb_addr_cli[1].sin_family = AF_INET;
	inet_aton("127.0.0.1.", &lb_addr_cli[1].sin_addr);
	lb_addr_cli[1].sin_port = htons(atoi(argv[3]));

	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call.
	*/
	if (bind(sockfd, (struct sockaddr *)&lb_addr,
			 sizeof(lb_addr)) < 0)
	{
		printf("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5); /* This specifies that up to 5 concurrent client
				  requests will be queued up while the system is
				  executing the "accept" system call below.
			   */

	/* In this program we are illustrating a concurrent server -- one

	   which forks to accept multiple client connections concurrently.
	   As soon as the server accepts a connection from a client, it
	   forks a child which communicates with the client, while the
	   parent becomes free to accept a new connection. To facilitate
	   this, the accept() system call returns a new socket descriptor
	   which can be used by the child. The parent continues with the
	   original socket descriptor.
	*/

	if (connect(server1, (struct sockaddr *)&lb_addr_cli[0], sizeof(lb_addr_cli[0])) < 0)
	{
		perror("Unable to connect to server\n");
		exit(0);
	}

	// for (i = 0; i < 100; i++)
	// 	buf[i] = '\0';
	// recv(server1, buf, 100, 0);
	// printf("%s\n", buf);

	for (i = 0; i < 100; i++)
		buf[i] = '\0';
	strcpy(buf, "Send Load");
	send(server1, buf, 100, 0);

	for (i = 0; i < 100; i++)
		buf[i] = '\0';
	recv(server1, buf, 100, 0);
	printf("Load received from server1 %s\n", buf);
	val1 = atoi(buf);
	close(server1);

	if (connect(server2, (struct sockaddr *)&lb_addr_cli[1], sizeof(lb_addr_cli[1])) < 0)
	{
		perror("Unable to connect to server\n");
		exit(0);
	}

	// for (i = 0; i < 100; i++)
	// 	buf[i] = '\0';
	// recv(server2, buf, 100, 0);
	// printf("%s\n", buf);

	for (i = 0; i < 100; i++)
		buf[i] = '\0';
	strcpy(buf, "Send Load");
	send(server2, buf, 100, 0);

	for (i = 0; i < 100; i++)
		buf[i] = '\0';
	recv(server2, buf, 100, 0);
	printf("Load received from server2 %s\n", buf);
	val2 = atoi(buf);
	close(server2);

	struct pollfd fds;
	fds.fd = sockfd;
	fds.events = POLLIN;
	long int timeout = (5 * 1000);
	while (1)
	{

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/

		// printf("Waiting on poll()...\n");
		// printf("%ld\n", timeout);
		// timeout = timeout - ((time_after_poll - time_before_poll)*1000);
		// time_before_poll = time(NULL);
		rc = poll(&fds, 1, timeout);
		// printf("rc=%d\n", rc);
		// time_after_poll = time(NULL);
		// printf("%ld", ((time_after_poll - time_before_poll)*1000));
		
		// printf("%d\n", timeout);
		// if(timeout < 0)timeout = (5*1000);
		if (rc < 0)
		{
			perror("  poll() failed");
			break;
		}
		if (rc == 0)
		{

			if ((server1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				printf("Cannot create socket\n");
				exit(0);
			}

			if ((server2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				printf("Cannot create socket\n");
				exit(0);
			}

			if (connect(server1, (struct sockaddr *)&lb_addr_cli[0], sizeof(lb_addr_cli[0])) < 0)
			{
				perror("Unable to connect to server\n");
				exit(0);
			}

			// printf("%s\n", buf);

			// for (i = 0; i < 100; i++)
			// 	buf[i] = '\0';
			// recv(server1, buf, 100, 0);
			// printf("%s\n", buf);

			for (i = 0; i < 100; i++)
				buf[i] = '\0';
			strcpy(buf, "Send Load");
			send(server1, buf, 100, 0);

			for (i = 0; i < 100; i++)
				buf[i] = '\0';
			recv(server1, buf, 100, 0);
			printf("Load received from server1 %s\n", buf);
			val1 = atoi(buf);
			close(server1);

			if (connect(server2, (struct sockaddr *)&lb_addr_cli[1], sizeof(lb_addr_cli[1])) < 0)
			{
				perror("Unable to connect to server\n");
				exit(0);
			}

			// for (i = 0; i < 100; i++)
			// 	buf[i] = '\0';
			// recv(server2, buf, 100, 0);
			// printf("%s\n", buf);

			for (i = 0; i < 100; i++)
				buf[i] = '\0';
			strcpy(buf, "Send Load");
			send(server2, buf, 100, 0);

			for (i = 0; i < 100; i++)
				buf[i] = '\0';
			recv(server2, buf, 100, 0);
			printf("Load received from server2 %s\n", buf);
			val2 = atoi(buf);
			close(server2);
		}

		else
		{
			if ((server3 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				printf("Cannot create socket\n");
				exit(0);
			}
			clilen = sizeof(cli_addr);
			newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
							   &clilen);

			if (newsockfd < 0)
			{
				printf("Accept error\n");
				exit(0);
			}

			if (fork() == 0)
			{

				/* This child process will now communicate with the
				   client through the send() and recv() system calls.
				*/
				close(sockfd); /* Close the old socket since all
						  communications will be through
						  the new socket.
					   */

				/* We initialize the buffer, copy the message to it,
				   and send the message to the client.
				*/
				// for (i = 0; i < 100; i++)
				// 	buf[i] = '\0';
				// recv(newsockfd, buf, 100, 0);
				// printf("%s\n", buf);

				
				if (val1 > val2)
				{
					if (connect(server3, (struct sockaddr *)&lb_addr_cli[1], sizeof(lb_addr_cli[1])) < 0)
					{
						perror("Unable to connect to server\n");
						exit(0);
					}
					printf("Sending client request to server_2\n");
				}

				else
				{
					if (connect(server3, (struct sockaddr *)&lb_addr_cli[0], sizeof(lb_addr_cli[0])) < 0)
					{
						perror("Unable to connect to server\n");
						exit(0);
					}
					printf("Sending client request to server_1\n");
				}


				for (i = 0; i < 100; i++)
					buf[i] = '\0';
				strcpy(buf, "Send Time");
				send(server3, buf, strlen(buf) + 1, 0);

				/* We again initialize the buffer, and receive a
				   message from the client.
				*/

				for (i = 0; i < 100; i++)
					buf[i] = '\0';

				int n = recv(server3, buf, 100, 0);
				printf("%s", buf);
				send(newsockfd, buf, strlen(buf) + 1, 0);

				close(newsockfd);
				exit(0);
			}
		}
		// close(newsockfd);
	}
	return 0;
}
