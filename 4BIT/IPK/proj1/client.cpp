#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <cstdio>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <regex>

#define BUFSIZE 1024
#define ERR_ARGS 1

int main(int argc, char **argv) {

    if (argc != 6 && argc != 7) {   // kontrola poctu argumentov
        printf("%d\n",argc);
        fprintf(stderr, R"(Error: use %s -h "hostname" -p "port" -n/f/l "login" )", argv[0]);
        exit(ERR_ARGS);
    }

    int c;
    char *pflag = "0";  // mod pre server
    std::string flag_param;
    while ((c = getopt (argc, argv, "h:p:n:f:l::")) != -1)
        switch (c) {
            case 'h': break;
            case 'p': break;
            case 'n': pflag = "1"; flag_param.assign(argv[6]); break;
            case 'f': pflag = "2"; flag_param.assign(argv[6]); break;
            case 'l': pflag = "3";
                      if (argc == 7) {  //ak je pri -l aj login
                          flag_param.assign(argv[6]);
                      }
                      break;
            default: printf("error\n"); break;
        }

    int client_socket, port_number, bytestx, bytesrx;
    socklen_t serverlen;
    const char *server_hostname;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[BUFSIZE];

    server_hostname = argv[2];
    port_number = atoi(argv[4]);

    /* 2. ziskani adresy serveru pomoci DNS */
    if ((server = gethostbyname(server_hostname)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    /* 3. nalezeni IP adresy serveru a inicializace struktury server_address */
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    /* Vytvoreni soketu */
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
    {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }

    /* odeslani zpravy na server */
    bytestx = send(client_socket, pflag, 1, 0); //mod pre server
    if (bytestx < 0)
        perror("ERROR in sendto");

    const char *param = flag_param.c_str();
    bytestx = send(client_socket, param, strlen(param), 0); //login
    if (bytestx < 0)
        perror("ERROR in sendto");

    /* prijeti odpovedi a jeji vypsani */
    bytesrx = recv(client_socket, buf, BUFSIZE, 0);
    if (bytesrx < 0)
        perror("ERROR in recvfrom");

    while(1) {

        bytestx = send(client_socket, "rdy\0", 4, 0);   //odoslanie spravy ze je klient pripraveny
        if (bytestx < 0)
            perror("ERROR in sendto");

        bzero(buf, BUFSIZE);

        /* prijeti odpovedi */
        bytesrx = recv(client_socket, buf, BUFSIZE, 0);
        if (bytesrx < 0)
            perror("ERROR in recvfrom");

        if (strcmp(buf,"end") == 0) {   //koniec prenosu - ukoncnie klienta
            break;
        }

        printf("%s\n", buf);


    }

    close(client_socket);

    return 0;
}