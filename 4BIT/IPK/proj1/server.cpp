#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {

    if (argc != 3) {
        fprintf(stderr,"usage: %s -p <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int c;
    while ((c = getopt (argc, argv, "p:")) != -1)
        switch (c) {
            case 'p': break;
            default: printf("error\n"); break;
        }

    int rc;
    int welcome_socket;
    struct sockaddr_in6 sa;
    struct sockaddr_in6 sa_client;
    char str[INET6_ADDRSTRLEN];
    int port_number;

    port_number = atoi(argv[2]);

    /* vytvorenie socketu */
    socklen_t sa_client_len=sizeof(sa_client);
    if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    memset(&sa,0,sizeof(sa));
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port_number);

    /* ziskanie portu */
    if ((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0)
    {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
    if ((listen(welcome_socket, 1)) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len); //pripojenie klienta
        if (comm_socket > 0)
        {
            if(inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {   //nove pripojenie
                printf("INFO: New connection:\n");
                printf("INFO: Client address is %s\n", str);
                printf("INFO: Client port is %d\n", ntohs(sa_client.sin6_port));
            }

            char mode[1];   //mod servera
            char param[1024];   //login
            char enable[4];     //buffer pre potvrdenie komunikacie
            int res = 0;
            for (;;)
            {
                // prijatie parametrov
                bzero(mode, 1);
                res = recv(comm_socket, mode, 1,0);
                if (res <= 0)
                    break;
                bzero(param, 1024);
                res = recv(comm_socket, param, 1024, 0);
                if (res <= 0)
                    break;
                // otvorenie /etc/passwd
                std::ifstream usrs ("/etc/passwd");
                if (!usrs) {
                    perror("ERROR: opening file failed!");
                    exit(EXIT_FAILURE);
                }
                std::string usr;
                /* -n -f parametre klienta */
                if (mode[0] == '1' || mode[0] == '2') {
                    send(comm_socket, "rdy\0", 4, 0); //sprava pre klienta - server je pripraveny
                    char login[1024];
                    bzero(login, 1024);
                    while (std::getline(usrs, usr) && usr.size() > 9) { //citanie riadku
                        int i;
                        for (i = 0; i < 1022; i++) {
                            char c = usr.at(i);
                            if (i < strlen(param)) {
                                if (c != param[i]) {        //hladanie podla loginu
                                    continue;  //ak nesedi - dalsi raidok
                                }
                            }
                            else if (i == strlen(param)) {  //najdeny spravny riadok
                                if (c == ':') {
                                    int j = i + 1;  //index kde sme skoncili po kontrole loginu
                                    int ddot = 0;   //pocet preskocenych ':'
                                    int idx = 0;    //index pre ukladanie informacii
                                    int delim = 4;  // pocet ':' ktore treba preskocit aby sme dostali spravnu informaciu
                                    if (mode[0] == '1') {   //ak -n preskocime len 3 ':'
                                        delim = 3;
                                    }
                                    while ( ddot != delim) {    // citame riadok kym neprejdeme na spravne miesto, za ':'
                                        if (usr.at(j++) == ':')
                                            ddot++;
                                    }
                                    while ((c = usr.at(j++)) != ':') {  //ukladame info po dalsiu ':'
                                        login[idx++] = c;
                                    }
                                    login[strlen(login)] = '\0';
                                    res = recv(comm_socket, enable, 4, 0);  // cakame na odpoved od klienta ci je pripraveny
                                    if (res <= 0)
                                        break;
                                    send(comm_socket, login, strlen(login), 0); //odoslanie informacii
                                    res = recv(comm_socket, enable, 4, 0);      //potvrdenie prijatia
                                    send(comm_socket, "end\0", 4, 0);           //koniec komunikacie
                                    break;
                                }
                                else {
                                    continue;
                                }
                            }
                        }
                    }
                    /* nebol najdeny login */
                    send(comm_socket, "end\0", 4, 0);
                    break;

                }
                /* -l parameter klienta */
                if (mode[0] == '3') {
                    send(comm_socket, "rdy\0", 4, 0); //sprava pre klienta - server je pripraveny
                    char login[1024];
                    while (std::getline(usrs, usr) && usr.size() > 9) { //citanie riadku
                        bool match = true;
                        int i;
                        for (i = 0; i < 1022; i++) {
                            char c = usr.at(i);
                            if (i < strlen(param)) {    //porovnanie loginov
                                if (c != param[i]) {
                                    match = false;      //ak nesedi
                                }
                            }
                            if (c != ':') { //hladame kym neprecitame ':'
                                login[i] = c;
                            } else {
                                break;
                            }
                        }
                        if (match == true || strlen(param) == 0) {  //login najdeny alebo posiela vsetko - ak nebol zadany
                            login[i] = '\0';
                            res = recv(comm_socket, enable, 4, 0);  //prijatie spravy ze je klient pripraveny
                            if (res <= 0)
                                break;
                            printf("%s\n", login);
                            send(comm_socket, login, strlen(login), 0); //poslanie informacie
                        }

                    }
                    send(comm_socket, "end\0", 4, 0);   //ukoncenie komunikacie
                }

            }
        }
        else
        {
            printf(".");
        }
        printf("Connection to %s closed\n",str);
        close(comm_socket);
    }

}