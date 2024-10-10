

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

/* THE SERVER PROCESS */



int main(int argc, char **argv)
{
	srand(time(0));

	int sockfd, newsockfd; /* Socket descriptors */
	int clilen;
	struct sockaddr_in cli_addr, serv_addr;

	int i;
	char buf[100]; /* We will use this buffer for communication */
	// char buff[100];
	struct tm *ptr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(sockfd, (struct sockaddr *)&serv_addr,
			 sizeof(serv_addr)) < 0)
	{
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5);

	while (1)
	{

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
						   &clilen);

		if (newsockfd < 0)
		{
			perror("Accept error\n");
			exit(0);
		}

		// strcpy(buf, "Message from server_time");
		// send(newsockfd, buf, strlen(buf) + 1, 0);

		for (i = 0; i < 100; i++)
			buf[i] = '\0';
		recv(newsockfd, buf, 100, 0);
		printf("%s\n", buf);

		if (strcmp(buf, "Send Load") == 0)
		{
			int rand_val = rand() % 100;

			for (i = 0; i < 100; i++)
				buf[i] = '\0';
			sprintf(buf, "%d", rand_val);

			int n = send(newsockfd, buf, strlen(buf) + 1, 0);
			printf("Load sent: %d\n", rand_val);
		}
		else if (strcmp(buf, "Send Time") == 0)
		{
			time_t t;
			t = time(NULL);
			ptr = localtime(&t);
			strcpy(buf, asctime(ptr));
			send(newsockfd, buf, strlen(buf) + 1, 0);

			// for (i = 0; i < 100; i++)
			// 	buf[i] = '\0';
			// recv(newsockfd, buf, 100, 0);
			// printf("%s\n", buf);
		}

		close(newsockfd);
	}
	return 0;
}
