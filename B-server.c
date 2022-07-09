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

//server logic
int serverLogic(char buffer[], char reply[]) {
    // printf(
    //     "Part B: Client sends 'a' to create a data structure. Client sends 'b' "
    //     "to trigger the bandwidth bug. \n\n-----------------\nphp client.php "
    //     "a\n------------------\nthen\n-----------------\nphp client.php "
    //     "b\n------------------\n");
    int sockfd;
    // char buffer[MAXLINE];
    // char largeReply[] =
    //     "a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d"
    //     "4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7h8i9j10a1b2c3d4e5f6g7"
    //     "h8i9j10a1b2c3d4e5f6g7h8i9j10";
    char smallReply[] = "5";
    //char dataStructure[50] = "";
    char *dataStructure;
    dataStructure = malloc(sizeof(char));

    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;  // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // bind to 169.235.26.114 local IP address
    servaddr.sin_port = htons(PORT); //specify port to listen on

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) <
        0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    unsigned int len, n, o;

    len = sizeof(cliaddr);  // len is value/resuslt

    while (1) {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
    
        printf("Client : %s\n", buffer);

        if (strcmp(buffer, "a\n") == 0) {
            time_t mytime = time(NULL);
            char *time_str = ctime(&mytime);
            // time_str[strlen(time_str) - 1] = '\0';
            //printf("A: %zu\n", sizeof(time_str));

            dataStructure = (char *) realloc(dataStructure, MAXLINE);
            memset(dataStructure, 0, MAXLINE);
            strcpy(dataStructure, time_str);
            printf("***DATA STRUCTURE SET***\n");
        } else {
            //dataStructure = (char *) realloc(dataStructure, sizeof(dataStructure));
            sendto(sockfd, (const char *)dataStructure, MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
            strcpy(reply, dataStructure);
            printf("***SENT***\n");
        }
    }

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
    // char *reply = malloc(MAXLINE);
    klee_make_symbolic(&buffer, sizeof(buffer), "buffer");
    klee_make_symbolic(&reply, sizeof(reply), "reply");
    return serverLogic(buffer, reply);
}

int main()
{
    driver();

    return 0;
}