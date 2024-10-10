
/*    THE CLIENT PROCESS */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include<poll.h>
struct tm *modified;
char modified_date[100];
void extract_header_info(const char *request_string, char *url, char *ip, char *port, char *file_name)
{
    const char *start = request_string;
    const char *end = start + strlen(request_string);
    const char *p = start;
    const char *q = p;
    // while (*p != ' ')
    //     ++p;
    while (*p != '/')
        ++p;
    ++p;
    ++p;
    q = p;
    while (*q != '/')
        ++q;
    strncpy(ip, p, q - p);
    ip[q - p] = '\0';
    // ++q;
    p = q;
    while (*q != ':' && q < end)
        ++q;
    strncpy(url, p, q - p);
    url[q - p] = '\0';
    if (q < end)
    {
        ++q;
        p = q;
        while (q < end && *q != ' ')
            ++q;
        strncpy(port, p, q - p);
        port[q - p] = '\0';
        // ++p;
        // q = p;
    }

    if (q < end)
    {
        ++q;
        p = q;
        while (q < end)
            ++q;
        strncpy(file_name, p, q - p);
        port[q - p] = '\0';
    }
}

char *date()
{
    char *Date = (char *)malloc(80 * sizeof(char));
    struct tm *ptr;
    time_t t, t_modified;
    t = time(NULL);
    ptr = gmtime(&t);

    strftime(Date, 80, "%a, %d %b %Y %H:%M:%S GMT", ptr);
    // printf("%s\n", Date);
    t_modified = t - (2 * 24 * 60 * 60);
    modified = gmtime(&t_modified);
    strftime(modified_date, 80, "%a, %d %b %Y %H:%M:%S GMT", modified);

    return Date;
}

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;

    int i;
    char buf[100];
    char request_string[3000];
    char header_url[1024];
    char header_ip[1024];
    char port[10] = "80";
    char type[10];
    char file_name[1000];
    char protocol_version[50];
    char host[1024];
    char connection[100];
    char Date[100];
    char content_len[100];
    char content_type[100];
    char accept[100];
    char c_type[100];
    char port_ser[100];
    char Expire_date[100];
    char content_lan[100];
    char cache_control[100];
    char folder[100];
    char last_modi[100];
    while (1)
    {
        printf("MyOwnBrowser>");
        scanf("%s", type);
        // printf("Type: %s\n", type);
        scanf("%s", request_string);
        if (strcmp(type, "QUIT") == 0)
            exit(0);
        if (strcmp(type, "PUT") == 0)
        {
            scanf("%s", file_name);
        }
        extract_header_info(request_string, header_url, header_ip, port, file_name);
        // printf("\nType: %s\n", type);
        // printf("Header URL: %s\n", header_url);
        // printf("Header IP: %s\n", header_ip);
        // printf("Port: %s\n", port);
        // printf("file_name: %s\n", file_name);
        date();

        /* Opening a socket is exactly similar to the server process */
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Unable to create socket\n");
            exit(0);
        }

        serv_addr.sin_family = AF_INET;
        inet_aton(header_ip, &serv_addr.sin_addr);
        serv_addr.sin_port = htons(atoi(port));

        if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                     sizeof(serv_addr))) < 0)
        {
            perror("Unable to connect to server\n");
            exit(0);
        }

        // for (i = 0; i < 100; i++)
        //     buf[i] = '\0';
        // recv(sockfd, buf, 100, 0);
        // printf("%s\n", buf);

        // strcpy(buf, "Message from client");
        // send(sockfd, buf, 100, 0);
        // printf("%s\n", type);
        send(sockfd, type, 10, 0);
        // printf("%s\n", header_url);
        strcpy(folder, header_url);
        send(sockfd, header_url, 1024, 0);
        strcpy(protocol_version, "HTTP/1.1");
        send(sockfd, protocol_version, 50, 0);
        strcpy(host, "Host: ");
        strcat((char *)host, (char *)header_ip);
        send(sockfd, host, 1024, 0);
        strcpy(port_ser, "Port: ");
        strcat((char *)port_ser, (char *)port);
        // printf("%s\n", port_ser);
        send(sockfd, port_ser, 100, 0);

        strcpy(connection, "Connection: close");
        send(sockfd, connection, 100, 0);
        char Date[100];
        strcpy(Date, "Date: ");
        char *tempDate = date();
        strcat(Date, tempDate);
        // printf("%s\n", Date);
        free(tempDate);
        send(sockfd, Date, 100, 0);

        char *token = strtok(header_url, ".");
        char *extention;
        while (token != NULL)
        {
            // printf("%s\n", token);
            extention = token;
            token = strtok(NULL, ".");
        }
        // printf("%s", extention);

        strcpy(accept, "Accept: ");
        if (strcmp(extention, "html") == 0)
        {
            strcpy(c_type, "text/html");
            strcat(accept, c_type);
            send(sockfd, accept, 100, 0);
        }
        else if (strcmp(extention, "pdf") == 0)
        {
            strcpy(c_type, "application/pdf");
            strcat(accept, c_type);
            send(sockfd, accept, 100, 0);
        }
        else if (strcmp(extention, "jpg") == 0)
        {
            strcpy(c_type, "image/jpeg");
            strcat(accept, c_type);
            send(sockfd, accept, 100, 0);
        }
        else
        {
            strcpy(c_type, "text/*");
            strcat(accept, c_type);
            send(sockfd, accept, 100, 0);
        }
        strcpy(buf, "Accept-Language: en-us,en;q=0.5");
        send(sockfd, buf, 100, 0);

        char modi_time[100];
        strcpy(modi_time, "If-modified-since: ");
        strcat(modi_time, modified_date);
        // printf("%s", modi_time);
        send(sockfd, modi_time, 100, 0);
        strcpy(buf, "Content-Language: en-us");
        send(sockfd, buf, 100, 0);



        if (strcmp(type, "PUT") == 0)
        {
            char buffer[100];
            // char get_line[1024];
            FILE *fp;
            int lines = 0;
            char ch;

            fp = fopen(file_name, "r");
            if (fp == NULL)
            {
                printf("Unable to open file.\n");
                return 0;
            }
            while ((ch = fgetc(fp)) != EOF)
            {
                if (ch == '\n')
                    lines++;
                // printf("%c", ch);
            }
            int i = 0;
            // while (i < lines) {
            sprintf(buffer, "Content-length: %d", lines);
            send(sockfd, buffer, 100, 0);

            strcpy(content_type, "Content-type: ");
            strcat(content_type, c_type);
            send(sockfd, content_type, 100, 0);
            // fgetc(stdin);
            // fgets(get_line, 1024, fp);
            // printf("%s", get_line);
            // fgets(get_line, 1024, fp);
            // printf("%s", get_line);
            // send(sockfd, get_line, 1024, 0);
            // }

            fclose(fp);
        }


        char status[100];
        char ser_version[100];

        struct pollfd fds;
        fds.fd = sockfd;
        fds.events = POLLIN;
        int timeout = 3*1000;
        int rv = poll(&fds, 1, timeout);

        if(rv == 0)
        {
            printf("close the connection\n");
            close(sockfd);
            continue;
        }
        recv(sockfd, ser_version, 100, 0);
        printf("%s", ser_version);
        recv(sockfd, status, 100, 0);
        printf("%s", status);
        recv(sockfd, Expire_date, 100, 0);
        printf("%s", Expire_date);
        recv(sockfd, cache_control, 100, 0);
        printf("%s", cache_control);
        recv(sockfd, content_lan, 100, 0);
        printf("%s", content_lan);



        if (strcmp(type, "GET") == 0)
        {
            recv(sockfd, content_len, 100, 0);
            printf("%s", content_len);
            recv(sockfd, content_type, 100, 0);
            printf("%s", content_type);
            recv(sockfd, last_modi, 100, 0);
            printf("%s", last_modi);
        }




        if (strcmp(type, "PUT") == 0)
        {
            // char get_line[1024];
           send(sockfd, file_name, 100, 0);

            // printf("%s\n\n%d", file_name, n);
            FILE *file;
            char buffer_line[100];
            
            file = fopen(file_name, "r");
            if (file == NULL)
            {
                perror("Error opening file");
                exit(1);
            }

           int size;
            while(size = fread(buffer_line, 1, 100, file))
            {
                send(sockfd, buffer_line, size, 0);
                for(int i=0; i<100; i++)
                {
                    buffer_line[i] = '\0';
                }
            }

            fclose(file);
        }
            




        
        if (strcmp(type, "GET") == 0)
        {
            char *p = folder;
            // printf("%s\n", header_url);
            char get_file_name[100];
            strncpy(get_file_name, p + 1, strlen(folder) - 1);
            get_file_name[strlen(folder) - 1] = '\0';
            // printf("%s\n", get_file_name);
            // printf("%s\n", content_len);
            // printf("%s\n", file_name);
            token = strtok(content_len, " ");
            char *len;
            while (token != NULL)
            {
                len = token;
                token = strtok(NULL, " ");
            }
            token = strtok(get_file_name, "/");
            char *gt_file_name;
            while (token != NULL)
            {
                gt_file_name = token;
                token = strtok(NULL, "/");
            }
            // printf("%d\n", atoi(len));
            // printf("%s\n", gt_file_name);
            FILE *fp;

                // Open the file in write mode
                fp = fopen(gt_file_name, "w");
                if (fp == NULL)
                {
                    printf("Error opening file!\n");
                    return 1;
                }
                char get_line[100];
                i=0;
                // while (i < atoi(len))
                // {
                //     // printf("hello\n");
                //     for(int k=0; k<5000; k++)get_line[k]='\0';
                //     recv(sockfd, get_line, 5000, 0);
                //     // printf("%s", get_line);
                //     fprintf(fp, "%s",get_line);
                //     i++;
                // }
                int size;
                while ((size = recv(sockfd, get_line, 100, 0)))
                {
                    fwrite(get_line, 1, size, fp);
                    for (int i = 0; i < 100; i++)
                    {
                        get_line[i]= '\0';
                    }
                    
                }
                
                fclose(fp);
            if(fork()==0)
            {
                if (strcmp(c_type, "text/html") == 0)
            {
                execlp("firefox", "firefox", gt_file_name, (char *) NULL);
            }
            else if (strcmp(c_type, "application/pdf") == 0)
            {
                execlp("xdg-open", "xdg-open", gt_file_name, (char *) NULL);
            }
            else if (strcmp(c_type, "image/jpeg") == 0)
            {
                execlp("gthumb", "gthumb", gt_file_name, (char *) NULL);
            }
            else if (strcmp(c_type, "text/*") == 0)
            {
                execlp("gedit", "gedit", gt_file_name, (char *) NULL);
            }
            }
        }

        close(sockfd);
    }
    return 0;
}
