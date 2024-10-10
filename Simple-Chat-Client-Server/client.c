
/*    THE CLIENT PROCESS */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


char *receve_str(int sockfd, char buff[])
{
    int n = 0;
    for (int i = 0; i < 50; i++)
        buff[i] = '\0';
    n = recv(sockfd, buff, 50, 0);
    if (strcmp(buff, "NULL") != 0)
    {
        // printf("%d\n", n);
        printf("%s", buff);
        while (buff[48] != '\0')
        {
            if (strcmp(buff, "NULL") == 0)
                break;
            for (int i = 0; i < 50; i++)
                buff[i] = '\0';
            recv(sockfd, buff, sizeof(buff)+1, 0);
            printf("%s", buff);
        }
        printf("\n");
    }
    return buff;
}

int main()
{
	int sockfd;
	struct sockaddr_in serv_addr;

	int i;
	char buf[100];
	char buff[100];

	
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("Unable to create socket\n");
			exit(0);
		}

		serv_addr.sin_family = AF_INET;
		inet_aton("127.0.0.1", &serv_addr.sin_addr);
		serv_addr.sin_port = htons(2002);

		if ((connect(sockfd, (struct sockaddr *)&serv_addr,
					 sizeof(serv_addr))) < 0)
		{
			perror("Unable to connect to server\n");
			exit(0);
		}

		// strcpy(buff, "Message from client");
		// send(sockfd, buff, sizeof(buff) + 1, 0);

		for (i = 0; i < 100; i++)
			buf[i] = '\0';
		receve_str(sockfd, buf);
		
	close(sockfd);
	return 0;
}
