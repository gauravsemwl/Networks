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
#include <time.h>
#include <poll.h>
#include<sys/stat.h>
/* THE SERVER PROCESS */
struct tm *modified;
char modified_date[100];
char *date()
{
    char *Date = (char *)malloc(80 * sizeof(char));
    struct tm *ptr;
    time_t t, t_modified;
    t = time(NULL);
    ptr = gmtime(&t);

    strftime(Date, 80, "%a, %d %b %Y %H:%M:%S GMT", ptr);
    // printf("%s\n", Date);
    t_modified = t + (3 * 24 * 60 * 60);
    modified = gmtime(&t_modified);
    strftime(modified_date, 80, "%a, %d %b %Y %H:%M:%S GMT\n", modified);

    return Date;
}

int main()
{
    int sockfd, newsockfd; /* Socket descriptors */
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;

    int i;
    char buf[100]; /* We will use this buffer for communication */
    char header_url[1024];
    char header_ip[1024];
    char port[100] = "80";
    char type[10];
    char file_name[1000];
    char protocol_version[50];
    char host[100];
    char connection[100];
    char Date[100];
    char modi_time[100];
    char Accept[100];
    char content_type[100];
    char content_len[100];
    char cache_control[100];
    char folder[100];

    /* The following system call opens a socket. The first parameter
       indicates the family of the protocol to be followed. For internet
       protocols we use AF_INET. For TCP sockets the second parameter
       is SOCK_STREAM. The third parameter is set to 0 for user
       applications.
    */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    /* The structure "sockaddr_in" is defined in <netinet/in.h> for the
       internet family of protocols. This has three main fields. The
       field "sin_family" specifies the family and is therefore AF_INET
       for the internet family. The field "sin_addr" specifies the
       internet address of the server. This field is set to INADDR_ANY
       for machines having a single IP address. The field "sin_port"
       specifies the port number of the server.
    */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    /* With the information provided in serv_addr, we associate the server
       with its port using the bind() system call.
    */
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    listen(sockfd, 5); /* This specifies that up to 5 concurrent client
                  requests will be queued up while the system is
                  executing the "accept" system call below.
               */

    /* In this program we are illustrating an iterative server -- one
       which handles client connections one by one.i.e., no concurrency.
       The accept() system call returns a new socket descriptor
       which is used for communication with the server. After the
       communication is over, the process comes back to wait again on
       the original socket descriptor.
    */
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

        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           &clilen);

        if (newsockfd < 0)
        {
            perror("Accept error\n");
            exit(0);
        }
        if (fork() == 0)
        {
            close(sockfd);

            /* We initialize the buffer, copy the message to it,
                and send the message to the client.
            */

            // strcpy(buf, "Message from server");
            // send(newsockfd, buf, 100, 0);

            // recv(newsockfd, buf, 100, 0);
            // printf("%s\n", buf);

            recv(newsockfd, type, 10, 0);
            printf("%s ", type);
            recv(newsockfd, header_url, 1024, 0);
            printf("%s ", header_url);
            strcpy(folder, header_url);
            recv(newsockfd, protocol_version, 50, 0);
            printf("%s\n", protocol_version);
            recv(newsockfd, header_ip, 1024, 0);
            printf("%s\n", header_ip);
            recv(newsockfd, port, 100, 0);
            // printf("%s\n", port);
            recv(newsockfd, connection, 100, 0);
            printf("%s\n", connection);
            recv(newsockfd, Date, 100, 0);
            printf("%s\n", Date);
            recv(newsockfd, buf, 100, 0);
            printf("%s\n", buf);
            recv(newsockfd, Accept, 100, 0);
            printf("%s\n", Accept);
            recv(newsockfd, modi_time, 100, 0);
            printf("%s\n", modi_time);
            recv(newsockfd, buf, 100, 0);
            printf("%s\n", buf);
            if (strcmp(type, "PUT") == 0)
            {
                recv(newsockfd, content_len, 100, 0);
                printf("%s\n", content_len);
                recv(newsockfd, content_type, 100, 0);
                printf("%s\n", content_type);
                // int i = 0;
                // while (i<content_len)
                // {
                //     char get_line[1024];
                //     recv(newsockfd, get_line, 1024, 0);
                //     printf("%s", get_line);
                //     i++;
                // }
            }

            char *token = strtok(Date, " ");
            char *date_token[100];
            int k = 0;
            while (token != NULL)
            {
                // printf("%s\n", token);
                date_token[k] = token;
                // printf("%s\n", date_token[k]);
                token = strtok(NULL, " ");
                k++;
            }
            char month[3];
            if (strcmp(date_token[3], "Jan") == 0)
                strcpy(month, "01");
            if (strcmp(date_token[3], "Feb") == 0)
                strcpy(month, "02");
            if (strcmp(date_token[3], "Mar") == 0)
                strcpy(month, "03");
            if (strcmp(date_token[3], "Apr") == 0)
                strcpy(month, "04");
            if (strcmp(date_token[3], "May") == 0)
                strcpy(month, "05");
            if (strcmp(date_token[3], "Jun") == 0)
                strcpy(month, "06");
            if (strcmp(date_token[3], "Jul") == 0)
                strcpy(month, "07");
            if (strcmp(date_token[3], "Aug") == 0)
                strcpy(month, "08");
            if (strcmp(date_token[3], "Sep") == 0)
                strcpy(month, "09");
            if (strcmp(date_token[3], "Oct") == 0)
                strcpy(month, "10");
            if (strcmp(date_token[3], "Nov") == 0)
                strcpy(month, "11");
            if (strcmp(date_token[3], "Dec") == 0)
                strcpy(month, "12");
            char year[5];
            char *p = date_token[4];
            strncpy(year, p + 2, 2);
            year[2] = '\0';
            char *time_token[5];
            token = strtok(date_token[5], ":");
            k = 0;
            while (token != NULL)
            {
                // printf("%s\n", token);
                time_token[k] = token;
                // printf("%s\n", time_token[k]);
                token = strtok(NULL, ":");
                k++;
            }
            char *port_data;
            token = strtok(port, " ");
            while (token != NULL)
            {
                port_data = token;
                token = strtok(NULL, " ");
            }

            char *ip_data;
            token = strtok(header_ip, " ");
            while (token != NULL)
            {
                ip_data = token;
                token = strtok(NULL, " ");
            }

            FILE *fp;
            fp = fopen("AccessLog.txt", "a");
            if (fp == NULL)
            {
                printf("Error opening file\n");
                exit(1);
            }
            // printf("%s %s %s", date_token[2], month, year);
            fprintf(fp, "%s%s%s:%s%s%s:%s:%s:%s:%s\n", date_token[2], month, year, time_token[0], time_token[1], time_token[2], ip_data, port_data, type, header_url);
            fclose(fp);
            date();

            char ser_version[100];
            char status[100];
            char *f_name;
            token = strtok(header_url, "/");
            while (token != NULL)
            {
                f_name = token;
                token = strtok(NULL, "/");
            }
            // printf("f_name : %s\n", f_name);
            char *pt = folder;
            // printf("%s\n", header_url);
            char get_file_name[100];
            strncpy(get_file_name, pt + 1, strlen(folder) - 1);
            get_file_name[strlen(folder) - 1] = '\0';
            // printf("%s\n", get_file_name);

            // sleep(4);

            strcpy(ser_version, "HTTP/1.1 ");
            send(newsockfd, ser_version, 100, 0);
            // printf("%s", header_url);
            int flag = 1;
            if (strcmp(type, "GET") && strcmp(type, "PUT"))
            {
                flag = 0;
                strcpy(status, "400 Bad Request\n");
                send(newsockfd, status, 100, 0);
                exit(0);
                return 0;
            }
            // else if (strcmp(type, "GET") == 0)
            // {
            //     FILE *fp;
            //     // int lines = 0;
            //     // char ch;

            //     fp = fopen(get_file_name, "r");
            //     if (fp == NULL)
            //     {
            //         // printf("Unable to open file.\n");
            //         // return 0;
            //         strcpy(status, "404 Not Found\n");
            //         send(newsockfd, status, 100, 0);
            //         return 0;
            //     }
            //     fclose(fp);
            // }

            // printf("%s", type);
            // char *f_name;
            // token = strtok(header_url, "/");
            // while (token != NULL)
            // {
            //     f_name = token;
            //     token = strtok(NULL, "/");
            // }
            // printf("f_name : %s\n", f_name);
            // char *pt = folder;
            // printf("%s\n", header_url);
            // char get_file_name[100];
            // strncpy(get_file_name, pt + 1, strlen(folder) - 1);
            // get_file_name[strlen(folder) - 1] = '\0';
            // printf("%s\n", get_file_name);
            int lines = 0;
            char buffer[100];
            if (strcmp(type, "GET") == 0)
            { // printf("Hello world");

                FILE *fp;

                char ch;

                fp = fopen(get_file_name, "r");
                if (fp == NULL)
                {
                    flag = 0;
                    strcpy(status, "404 Not Found\n");
                    send(newsockfd, status, 100, 0);
                    return 0;
                }
                while ((ch = fgetc(fp)) != EOF)
                {
                    if (ch == '\n')
                        lines++;
                }
                fclose(fp);
                lines++;
            }
            if (flag == 1)
            {
                strcpy(status, "200 OK\n");
                send(newsockfd, status, 100, 0);
            }
            strcpy(buf, "Expires: ");
            strcat(buf, modified_date);
            // printf("%s", modified_date);
            send(newsockfd, buf, 100, 0);
            strcpy(cache_control, "Cache-control: no-store\n");
            send(newsockfd, cache_control, 100, 0);
            strcpy(buf, "Content-language: en-us\n");
            send(newsockfd, buf, 100, 0);
            // printf("%s", content_len);
            // printf("%s", type);

            // printf("%s", type);
            if (strcmp(type, "PUT") == 0)
            {
                // printf("Hellow world");
                char file_name_cli[100];
                int n = recv(newsockfd, file_name_cli, 100, 0);
                strcat(f_name, "/");
                strcat(f_name, file_name_cli);
                // printf("%s", f_name);
                char get_line[100];
                char *token = strtok(content_len, " ");
                char *len;
                while (token != NULL)
                {
                    // printf("%s\n", token);
                    len = token;
                    token = strtok(NULL, " ");
                }
                // printf("%d", atoi(len));
                FILE *fp;

                // Open the file in write mode
                fp = fopen(f_name, "w");
                if (fp == NULL)
                {
                    printf("Error opening file!\n");
                    return 1;
                }

                // Write to the file

                // Close the file
                int size;
                while ((size = recv(newsockfd, get_line, 100, 0)))
                {
                    fwrite(get_line, 1, size, fp);
                    for (int i = 0; i < 100; i++)
                    {
                        get_line[i] = '\0';
                    }
                }
                fclose(fp);
            }

            if (strcmp(type, "GET") == 0)
            {
                // printf("Hello world");
                sprintf(buffer, "Content-length: %d\n", lines);
                send(newsockfd, buffer, 100, 0);

                strcpy(buf, "Content-type: en-us\n");
                send(newsockfd, buf, 100, 0);
                char Date[100];
                struct stat fileStat;
                char modi_date[100];
                if (stat(get_file_name, &fileStat) == 0)
                {
                    struct tm *ptr;
                    time_t t, t_modified;
                    t = time(NULL);
                    ptr = gmtime(&fileStat.st_atime);
                    strftime(Date, 80, "%a, %d %b %Y %H:%M:%S GMT\n", ptr);
                    // printf("Date :   %s", Date);
                    
                    strcpy(modi_date, "Last modified: ");
                    strcat(modi_date, Date);
                    send(newsockfd,modi_date, 100, 0);
                }
                else
                {
                    strcpy(modi_date, "Failed to get file stats\n");
                    send(newsockfd,modi_date, 100, 0);
                }
                
                char buffer_line[100];
                char buffer[100];
                FILE *fp;
                int lines = 0;
                char ch;

                fp = fopen(get_file_name, "r");
                if (fp == NULL)
                {
                    printf("Unable to open file.\n");
                    return 0;
                }
                // while (fgets(buffer_line, 1024, fp) != NULL)
                // {
                //     for(int k=0; k<5000; k++)buffer_line[k]='\0';
                //     printf("%s", buffer_line);
                //     send(newsockfd, buffer_line, 5000, 0);
                // }

                int size;
                while (size = fread(buffer_line, 1, 100, fp))
                {
                    send(newsockfd, buffer_line, size, 0);
                    for (int i = 0; i < 100; i++)
                    {
                        buffer_line[i] = '\0';
                    }
                }
                fclose(fp);
            }

            close(newsockfd);
            exit(0);
        }
        close(newsockfd);
    }
    return 0;
}
