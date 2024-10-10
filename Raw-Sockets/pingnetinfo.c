#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/sem.h>
#include <poll.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <ifaddrs.h>
#include <netdb.h>

struct hostent *host_ip;
unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

const char *getip(char *arg)
{
    host_ip = gethostbyname(arg);
    if (host_ip == NULL)
    {
        printf("IP address not found\n");
        exit(0);
    }
    return inet_ntoa(*(struct in_addr *)host_ip->h_addr_list[0]);
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Three arguments are required in command line\n");
        exit(0);
    }
    double rtt_min = 1000000, rtt_max = 0, rtt_sum = 0;
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    char buf[4096] = {0};
    struct ip *iph = (struct ip *)buf;
    int ttl = 1;

    int one = 1;
    const int *val = &one;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
        printf("HDRINCL not set properly\n");

    struct sockaddr_in addr;
    addr.sin_port = htons(7);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, getip(argv[1]), &(addr.sin_addr));
    char dest[100];
    strcpy(dest, inet_ntoa(addr.sin_addr));

    int n = atoi(argv[2]);
    if(n<5){
        n=5;
    }
    while (1){
        double bandwidth = 0;
        // setting IP header

        iph->ip_hl = 5;
        iph->ip_v = 4;
        iph->ip_tos = 0;
        iph->ip_len = 20 + 8;
        iph->ip_id = 10000;
        iph->ip_off = 0;
        iph->ip_ttl = ttl;
        iph->ip_p = IPPROTO_ICMP;

        struct ifaddrs *ifaddr, *ifa;
        int family, s;

        char host[INET_ADDRSTRLEN];
        if (getifaddrs(&ifaddr) == -1)
        {
            perror("Error in storing the list of struct ifaddrs\n");
            exit(0);
        }
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
                continue;
            family = ifa->ifa_addr->sa_family;
            if (family == AF_INET)
            {
                s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
                if (s != 0)
                {
                    printf("nameinfo not found\n");
                    exit(0);
                }
                if (strcmp(ifa->ifa_name, "eth0") == 0) break;
            }
        }
        freeifaddrs(ifaddr);

        inet_pton(AF_INET, host, &(iph->ip_src)); // source IP address
        printf("\nHost IP address: %s\n", host);
        printf("Destination IP address: %s\n", dest);
        inet_pton(AF_INET, getip(argv[1]), &(iph->ip_dst));
        iph->ip_sum = csum((unsigned short *)buf, 9);

        // setting ICMP header
        struct icmphdr *icmp = (struct icmphdr *)(buf + 20);
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->checksum = 0;
        icmp->un.echo.id = getpid();
        icmp->un.echo.sequence = ttl + 1;
        icmp->checksum = csum((unsigned short *)(buf + 20), 4);

        for (int i = 0; i < n; i++){
            usleep(atoi(argv[3]) * 1000000);
            double start = clock();

            if (sendto(fd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, (struct sockaddr *)&addr, sizeof addr) <= 0)
            {
                printf("\nUnable to send packet\n");
            }
            printf("\n\n****Sent request to server****\n");
            char ch = 'a';
            strncat(buf, &ch, 1);
            printf("Data sent: %s\n", buf);
            char buff[4096] = {0};
            struct sockaddr_in addr2;
            socklen_t len = sizeof(struct sockaddr_in);

            if (recvfrom(fd, buff, sizeof(buff), 0, (struct sockaddr *)&addr2, &len) <= 0)
            {
                printf("\nUnable to receive packet\n");
            }
            struct icmphdr *icmp2 = (struct icmphdr *)(buff + 20);
            if (icmp2->type != 0)
                printf("no. of hops: %d \nNext hop IP address: %s\n", ttl, inet_ntoa(addr2.sin_addr));
            else{
                printf("Reached destination: %s with ttl limit: %d\n", inet_ntoa(addr2.sin_addr), ttl);
                exit(0);
            }
            double end = clock();

            double timeElapsed = ((double)(end - start)) / 1000000.0;
            double rtt_msec = (end - start) * 1000.0 + timeElapsed;

            if (rtt_min > rtt_msec) rtt_min = rtt_msec;
            
            printf("****Received reply from server****\n\n");

            printf("latency: %f ms\n", (end - start) * 500 / CLOCKS_PER_SEC);
            printf("Rtt : %lf ms \n", rtt_msec);

            if (i != 0)
            {
                // calculate bandwidth
                bandwidth = 448/ ((rtt_msec - rtt_min) / 1000);
            }
            
            printf("type: %d\n", icmp2->type);
            printf("Checksum: %d \n", icmp->checksum);
            printf("ID: %d \n", icmp->un.echo.id);
            printf("Sequence: %d \n", icmp->un.echo.sequence);
            printf("ip_hl: %d \n",iph->ip_hl);
            printf("ip_v: %d \n",iph->ip_v);
            printf("ip_tos: %d \n",iph->ip_tos);
            printf("ip_len: %d \n",iph->ip_len);
            printf("ip_off: %d \n",iph->ip_off);
            printf("ip_ttl: %d \n",iph->ip_ttl);
            printf("ip_p: %d \n\n",iph->ip_p);

            //  iph->ip_hl = 5;
            // iph->ip_v = 4;
            // iph->ip_tos = 0;
            // iph->ip_len = 20 + 8;
            // iph->ip_id = 10000;
            // iph->ip_off = 0;
            // iph->ip_ttl = ttl;
            // iph->ip_p = IPPROTO_ICMP;

        }
        printf("Bandwidth = %.2f Mbps\n\n", bandwidth);
        ttl++;
    }
    close(fd);
    return 0;
}