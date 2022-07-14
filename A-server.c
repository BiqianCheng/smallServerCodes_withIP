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

// server logic
int serverLogic(char buffer[], char reply[])
{
    printf("Part A: Bandwidth bug on only certain path. Only triggers the bug with client sending a single char 'a'. \n\n-----------------\nphp client.php a\n------------------\n");

    char largeReply[MAXLINE] =
        "a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10";
    char smallReply[MAXLINE] = "5";
    int sockfd;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    unsigned int len, n, o;

    len = sizeof(cliaddr);
    // buffer[n] = '\0';
    while (1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
        // buffer[n] = '\0';
        printf("Client : %s\n", buffer);

        if (strcmp(buffer, "a\n") == 0)
        {
            sendto(sockfd, (const char *)largeReply, sizeof(largeReply),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
            strcpy(reply,largeReply);
        }
        else
        {
            sendto(sockfd, (const char *)smallReply, sizeof(smallReply),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
            strcpy(reply,smallReply);
        }

        printf("***SENT***\n");
    }
    printf("\n -------------- \n");
    return 0;
}

// Driver code
int driver()
{
    char buffer[MAXLINE];
    // int sockfd;
    // struct sockaddr_in servaddr, cliaddr;
    char reply[MAXLINE * 1000];
    memset(buffer, 0, MAXLINE);
    memset(reply, 0, MAXLINE * 1000);

    klee_make_symbolic(&buffer, sizeof(buffer), "buffer");
    klee_make_symbolic(&reply, sizeof(reply), "reply");
    klee_assume(reply[0] == '5');
    return serverLogic(buffer, reply);
}

int main()
{
    driver();

    return 0;
}