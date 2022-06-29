// Server side implementation of UDP client-server model
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <klee/klee.h>
#define PORT 55555
#define MAXLINE 1024

// Driver code
void driver()
{
    char reply[MAXLINE];
    // char *reply = malloc(MAXLINE);
    klee_make_symbolic(&reply, sizeof(reply), "reply");
}

// server logic
void serverLogic(char buffer[], int sockfd, struct sockaddr_in servaddr, struct sockaddr_in cliaddr, char reply[])
{
    printf("Part A: Bandwidth bug on only certain path. Only triggers the bug with client sending a single char 'a'. \n\n-----------------\nphp client.php a\n------------------\n");

    char largeReply[] =
        "a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10";
    char smallReply[] = "5";

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    unsigned int len, n, o;

    len = sizeof(cliaddr);
    n = recvfrom(sockfd, (char *)buffer, MAXLINE * sizeof(char), MSG_WAITALL,
                 (struct sockaddr *)&cliaddr, &len);
    // buffer[n] = '\0';
    while (1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);

        if (strcmp(buffer, "a") == 0)
        {
            sendto(sockfd, (const char *)largeReply, sizeof(largeReply),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        }
        else
        {
            sendto(sockfd, (const char *)smallReply, sizeof(smallReply),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        }

        printf("***SENT***\n");
    }
    printf("\n -------------- \n");
}

int main()
{
    char buffer[MAXLINE];
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    klee_make_symbolic(&buffer, sizeof(buffer), "buffer");
    // klee_make_symbolic(&sockfd, sizeof(sockfd), "sockfd");
    driver(buffer, sockfd, servaddr, cliaddr, reply);

    return 0;
}