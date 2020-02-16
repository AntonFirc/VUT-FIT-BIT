#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <chrono>
#include <vector>
#include <cmath>

#define ERR_ARGS 1
#define BUFSIZE 512

int main(int argc, char** argv) {

    if (argc <= 1) { //malo argumentov
        fprintf(stderr, R"(Error: use %s meter -h "host" -p "port" -s "size" -t "time")", argv[0]);
        exit(ERR_ARGS);
    }

    if (strcmp(argv[1], "meter") == 0) {    //meter
        if (argc != 10) {   // kontrola poctu argumentov
            fprintf(stderr, R"(Error: use %s meter -h "host" -p "port" -s "size" -t "time")", argv[0]);
            exit(ERR_ARGS);
        }
        int port_number, test_time, test_size;
        const char *server_hostname;
        int c;
        while ((c = getopt (argc, argv, "h:p:s:t:")) != -1)
            switch (c) {
                case 'h': server_hostname = argv[3]; break;
                case 'p': port_number = atoi(optarg); break;
                case 's': test_size = atoi(optarg); break;
                case 't': test_time = atoi(optarg); break;
                default: printf("error\n"); break;
            }

        printf("Starting bandwidth measurement: \n"
                       "*Target host: %s, Port: %d, "
                       "Packet size: %d, Test duration: %d*\n", server_hostname, port_number, test_size, test_time);

        int client_socket, bytestx, bytesrx;
        socklen_t serverlen;
        struct hostent *server;
        struct sockaddr_in server_address;
        char buf[BUFSIZE];

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

        /* tiskne informace o vzdalenem soketu */
        printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

        /* Vytvoreni soketu */
        if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        {
            perror("ERROR: socket");
            exit(EXIT_FAILURE);
        }

        /* vutvoerenie paketu o zadanej velkosti */
        test_size++;
        char test[test_size];
        for (int i = 0; i < test_size; i++) {
            test[i] = 'a';
        }
        test[test_size -1] = '\0'; //ukoncenie paketu
        /* statisticke data */
        float real_test_time = 0.0;
        int cycles = 0;
        float avg_speed = 0.0;
        float max_speed = 0.0;
        float min_speed;
        float avg_RTT = 0.0;
        std::vector< float > arr; //pole pro ulozeni nameranych rychlosti

        while (real_test_time < test_time) {    //pokial nebol prekroceny cas merania
            /* odeslani zpravy na server */
            serverlen = sizeof(server_address);
            bytestx = sendto(client_socket, test, test_size, 0, (struct sockaddr *) &server_address, serverlen);
            auto start = std::chrono::high_resolution_clock::now(); //zaznamenaie casu
            if (bytestx < 0)
                perror("ERROR: sendto");

            /* prijeti odpovedi a jeji vypsani */
            bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *) &server_address, &serverlen);
            auto finish = std::chrono::high_resolution_clock::now();    //zaznamenanie casu
            if (bytesrx < 0)
                perror("ERROR: recvfrom");
            std::chrono::duration<double> elapsed = finish - start; //vypocet RTT
            float speed = (8 * test_size) / elapsed.count();    // vypocet rychlosti
            arr.push_back(speed / 8096);
            avg_RTT += elapsed.count();
            avg_speed += speed;
            if (cycles == 0) {  // prepisovanie max a min rychlosti
                min_speed = speed;  //pri prvom spusteni nastavi min rychlost na aktualnu
            }
            else if (speed > max_speed) {
                max_speed = speed;
            }
            else if (speed < min_speed) {
                min_speed = speed;
            }
            real_test_time += elapsed.count();  //spocitavanie behu merania
            if ((++cycles % 3000) == 0) {  //vypis info kazdych 10000 odoslanych paketov
                std::cout << "Actual upload speed: " << speed << " Actual RTT: " << elapsed.count() << " Time elapsed: " << real_test_time << " seconds" << std::endl;
            }
        }

        double deviation = 0.0;

        /* vypocet standardnej odchylky */
        for (int i = 0; i < arr.size(); i++) {
            deviation += pow(arr.front() - avg_speed / 8096, 2);
            arr.pop_back();
        }
        deviation = deviation / cycles;
        deviation = sqrt(deviation);

        /* vypis informacii o merani */
        printf("*****************************************\n");
        printf("Average speed : %f MB/s\n", (avg_speed / 8096) / cycles);
        printf("Maximal speed : %f MB/s\n", max_speed / 8096);
        printf("Minimal speed : %f MB/s\n", min_speed / 8096);
        printf("Average RTT   : %f seconds\n", avg_RTT / cycles);
        printf("Standard deviation : %f MB \n", deviation);

        return 0;

    }
    else if (strcmp(argv[1], "reflect") == 0) {     //reflektor
        if (argc != 4) {   // kontrola poctu argumentov
            fprintf(stderr, R"(Error: use %s reflect -p "port")", argv[0]);
            exit(ERR_ARGS);
        }
        int port_number;
        int c;
        while ((c = getopt (argc, argv, "h:p:s:t:")) != -1)
            switch (c) {
                case 'p': port_number = atoi(optarg); break;
                default: printf("error\n"); break;
            }

        printf("Reflector started on port number %d.\n", port_number);

        char buf[BUFSIZE];
        int server_socket, bytestx, bytesrx;
        socklen_t clientlen;
        struct sockaddr_in client_address, server_address;
        int optval;
        const char * hostaddrp;
        struct hostent *hostp;

        /* Vytvoreni soketu */
        if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        {
            perror("ERROR: socket");
            exit(EXIT_FAILURE);
        }
        /* potlaceni defaultniho chovani rezervace portu ukonceni aplikace */
        optval = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

        /* adresa serveru, potrebuje pro prirazeni pozadovaneho portu */
        bzero((char *) &server_address, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons((unsigned short)port_number);

        if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        {
            perror("ERROR: binding");
            exit(EXIT_FAILURE);
        }
        while(1)    //nekonecna smycka cakanie na prichod paketu
        {
            /* prijeti paketu */
            clientlen = sizeof(client_address);
            bytesrx = recvfrom(server_socket, buf, BUFSIZE, 0, (struct sockaddr *) &client_address, &clientlen);
            if (bytesrx < 0)
                perror("ERROR: recvfrom:");

            hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr,
                                  sizeof(client_address.sin_addr.s_addr), AF_INET);

            hostaddrp = inet_ntoa(client_address.sin_addr);

            /* odeslani zpravy zpet klientovi  */
            bytestx = sendto(server_socket, "\0", 1, 0, (struct sockaddr *) &client_address, clientlen);
            if (bytestx < 0)
                perror("ERROR: sendto:");
        }


    }
    else {
        fprintf(stderr, R"(Error: use %s reflect/meter ...)", argv[0]);
        exit(ERR_ARGS);
    }

    return 0;

}
