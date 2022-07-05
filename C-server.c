// Server side implementation of UDP client-server model
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <klee/klee.h>

#define PORT 55555
#define MAXLINE 1024

// Driver code
int serverLogic(char buffer[], char reply[])
{
    int count = 0;
    printf(
        "Part C: Client sends anything to have the server return data from a "
        "local file WITHOUT TRIGGERS.\n");
    int sockfd;

    char largeReply[] =
        "a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d"
        "4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7"
        "h8i9j10a1b2c3d4e5f6g7h8i9j10";
    char smallReply[] = "5";
    // char dataStructure[50] = "";
    // char *dataStructure;
    // dataStructure = malloc(sizeof(char));

    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;                     // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // bind to 127.0.0.1 local IP address
    servaddr.sin_port = htons(PORT);                   // specify port to listen on

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int n, o;

    socklen_t len = sizeof(cliaddr); // len is value/result

    while (1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                     (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);

        char fileBuff[MAXLINE];
        FILE *f = fopen("data.txt", "r");
        fgets(fileBuff, MAXLINE, f);
        fclose(f);

        sendto(sockfd, (const char *)fileBuff, strlen(fileBuff), MSG_CONFIRM,
               (const struct sockaddr *)&cliaddr, len);
        memcpy(reply, fileBuff, strlen(fileBuff));
        count += strlen(fileBuff);
        // Manually adding amplification features
        sendto(sockfd, (const char *)fileBuff, strlen(fileBuff), MSG_CONFIRM,
               (const struct sockaddr *)&cliaddr, len);
        memcpy(reply + count, fileBuff, strlen(fileBuff));
        printf("***SENT***\n");
    }

    return 0;
}

int driver()
{
    char buffer[MAXLINE];
    // printf("The size of buffer is %zu\n", sizeof(buffer));
    char reply[MAXLINE * 1000];
    klee_make_symbolic(&buffer, sizeof(buffer), "buffer");

    // klee_assume(buffer[MAXLINE - 1] == '\0');
    klee_make_symbolic(&reply, sizeof(reply), "reply");
    return serverLogic(buffer, reply);
}

int main()
{
    driver();
    return 0;
}