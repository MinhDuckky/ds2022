
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

void handle_file(int cli, char *s, int size){

    memset(s, 0, size);
    
    char filename[100];

    int len = read(cli, filename, sizeof(filename));
    printf("Received file: %s", filename);
    int file = open(filename, O_RDWR);

    while ((len -  read(cli, s, size)) > 0) {

        write(file, s, size);
    }

    close(file);
}

int main() {
    int ss, cli, pid;
    struct sockaddr_in ad;
    char s[100];
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
            
            handle_file(cli, s, sizeof(s));

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