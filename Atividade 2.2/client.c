#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "socket_helper.h"

#define MAX_LINE 256
#define SERVER_PORT 12345

/**
 Programa cliente, que irá enviar uma  servidor, através da rede utilizando socket.
 */
int main(int argc, char **argv) {
    // declara variáveis que iremos utilizar adiante para a comunicação utilizando socket
    fd_set read_set;
    bool show_enter_message;
    struct timeval tv;

    int sockfd, n;
    char recvline[MAX_LINE + 1];
    char error[MAX_LINE + 1];
    struct sockaddr_in socket_address;
    struct sockaddr_in bound_address;
    struct hostent *host_address;
    unsigned int len;
    char buf[MAX_LINE + 1];
    int i;
    char ch;

    // valida parâmetros
    if (argc != 2) {
        // Número incorreto de parâmetros, então imprimimos como é o uso correto
        strcpy(error, "use: ");
        strcat(error, argv[0]);
        strcat(error, " <IPaddress>");
        perror(error);
        exit(1);
    }

    // resolve endereço do servidor
    host_address = gethostbyname(argv[1]);
    struct in_addr **address_list = (struct in_addr **)host_address->h_addr_list;

    printf("Server IP: %s\n", inet_ntoa(*address_list[0]));

    bzero(&bound_address, sizeof(bound_address));

    // configura o endereço do servidor para o socket
    bzero(&socket_address, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(SERVER_PORT);
    socket_address.sin_addr = *address_list[0];


    // cria um socket e armazena em sockfd
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    // conecta o socket ao endereço do servidor
    Connect(sockfd, (struct sockaddr *) &socket_address, sizeof(socket_address));

    printf("Connected to server of IP: %s, on local port: %d\n",
           inet_ntoa(*address_list[0]),
           ntohs(bound_address.sin_port));

    len = sizeof(bound_address);
    getsockname(sockfd, (struct sockaddr *) &bound_address, &len);

    show_enter_message = true;
    for (;;) {

        if (show_enter_message) {
            printf("-------\n");

            // le cadeia da entrada padrao
            printf("Enter the message you wish to send:\n");
        }

        show_enter_message = false;

        FD_ZERO(&read_set);
        FD_SET(sockfd, &read_set);
        FD_SET(0, &read_set);

        tv.tv_sec = 2;
        tv.tv_usec = 500000;

        select(sockfd + 1, &read_set, NULL, NULL, &tv);

        if (FD_ISSET(0, &read_set)) {
            for (i = 0; (ch = getchar()) != '\n'; i++) {
                buf[i] = ch;
            }

            if (strcmp(buf, "quit") == 0) {
                break;
            }

            buf[i] = '\n';
            buf[i+1] = 0;

            // envia cadeia digitada para o servidor
            write(sockfd, buf, strlen(buf));
        } else if (FD_ISSET(sockfd, &read_set)) {

            // le o conteúdo enviado pelo servidor e armazena em recvline
            if ((n = read_line(sockfd, recvline, MAX_LINE)) > 0) {
                recvline[n] = 0; // adiciona um \0 como caracter terminador da string
            }

            // verifica se a leitura do socket falhou
            if (n < 0) {
                // imprime a informação do erro e termina o programa
                perror("read error");
                exit(1);
            } else {
                // imprime o conteúdo recebido na saída padrão
                printf("\nServer sent: %s\n", recvline);
            }

            show_enter_message = true;
        }

    }

    close(sockfd);

    // termina o programa com sucesso
    exit(0);
}