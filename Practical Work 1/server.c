#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void handle_file(int cli){
    // it's client turn to chat, I wait and read message from client
    printf("Receiving file content.... \n");
    printf("\n");

    char s[255];
    read(cli, s, sizeof(s) + 1);

    FILE *file  = fopen("output.txt", "w");
    fputs(s, file);

    printf("Writing to: \'output.txt\'\n");

    fclose(file);

    printf("OK\n");
    char r[4] = "OK";
    write(cli, r, strlen(r) + 1);
}

int main() {
    int ss, cli, pid;
    struct sockaddr_in ad;
    char s[255];
    socklen_t ad_length = sizeof(ad);

    // create the socket
    ss = socket(AF_INET, SOCK_STREAM, 0);

    // bind the socket to port 12345
    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY;
    ad.sin_port = htons(12345);
    bind(ss, (struct sockaddr *)&ad, ad_length);

    // then listen
    listen(ss, 0);

    while (1) {
        // an incoming connection
        cli = accept(ss, (struct sockaddr *)&ad, &ad_length);

        pid = fork();
        if (pid == 0) {
            // I'm the son, I'll serve this client
            printf("Client connected\n");
            handle_file(cli);
            return 0;
        }
        else {
            // I'm the father, continue the loop to accept more clients
            continue;
        }
    }
    // disconnect
    close(cli);

}