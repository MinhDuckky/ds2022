#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void send_file(int serv, char *filename) {

    printf("Sending file content....");
    printf("\n");

    FILE *file  = fopen(filename, "r");

    printf("File: \'%s\'\n", filename);

    char s[255] = {0};

    char *line;

    while (fgets(line, 100, file))
    {
        strncat(s, line, 100);
    }
    printf("%s", s);
    printf("\n");
    // send some data to server

    write(serv, s, sizeof(s) + 1);

    fclose(file);

    // then it's server turn
    read(serv, s, sizeof(s) + 1);

    printf("Server says: %s\n", s);
}

int main(int argc, char* argv[]) {
    int so;
    char s[255];
    struct sockaddr_in ad;

    socklen_t ad_length = sizeof(ad);
    struct hostent *hep;

    // create socket
    int serv = socket(AF_INET, SOCK_STREAM, 0);

    // init address
    hep = gethostbyname(argv[1]);
    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;
    ad.sin_addr = *(struct in_addr *)hep->h_addr_list[0];
    ad.sin_port = htons(12345);

    // connect to server
    connect(serv, (struct sockaddr *)&ad, ad_length);
    send_file(serv, argv[2]);

}
