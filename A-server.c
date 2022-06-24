// Server side implementation of UDP client-server model
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 55555
#define MAXLINE 1024

// Driver code
int main() {
    printf("Part A: Bandwidth bug on only certain path. Only triggers the bug with client sending a single char 'a'. \n\n-----------------\nphp client.php a\n------------------\n");
    int sockfd;
    char buffer[MAXLINE];
    char largeReply[] =
        "a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10";
    char smallReply[] = "5";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;  // IPv4, match the socket() call
    servaddr.sin_addr.s_addr = inet_addr(" 127.0.0.1"); // bind to 169.235.26.114 IP address
    servaddr.sin_port = htons(PORT); //specify port to listen on

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) <
        0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n, o;

    len = sizeof(cliaddr);  // len is value/resuslt

    while (1) {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);

        if (strcmp(buffer, "a") == 0) {
            sendto(sockfd, (const char *)largeReply, sizeof(largeReply),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        } else {
            sendto(sockfd, (const char *)smallReply, sizeof(smallReply),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        }

        printf("***SENT***\n");
    }

    return 0;
}