#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include <arpa/inet.h>
#include "socket_helper.h"

#define LISTENQ 10

#define SERVER_PORT 12345
#define MAX_LINE 256

int max(int a, int b);

/**
 * Programa servidor, que irá aceitar conexões via socket em determinado endereço,
 * recebendo mensagens e retornando um echo destas mensagens
 */
int main(int argc, char **argv) {
    // declaração das variáveis que serão utilizadas para a comunicação
    int listenfd, connfd;
    struct sockaddr_in socket_address;
    char buf[MAX_LINE];
    struct sockaddr_in bound_addr;
    unsigned int len;
    char recvline[MAX_LINE + 1];
    int i, maxfd, new_s, n_clients, clients[FD_SETSIZE];
    fd_set read_set, all_fd_set;
    struct timeval tv;

    // configura o valor do endereço IP do servidor
    bzero(&socket_address, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(SERVER_PORT);

    // cria o socket e armazena em listenfd
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    // atribui o endereço ao socket
    Bind(listenfd, (struct sockaddr *) &socket_address, sizeof(socket_address));

    // habilita o recebimento de conexões por esse socket, o transformando em um socket de servidor
    Listen(listenfd, LISTENQ);

    maxfd = listenfd;

    // Limpa vetor de file descriptors dos clients
    n_clients = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        clients[i] = -1;

    printf("Listening for connections on port %d...\n", SERVER_PORT);

    // Limpa flags do fdset que mantem todos os file descriptors a serem observados
    FD_ZERO(&all_fd_set);

    // Seta o file descriptor do servidor para observar por dados
    FD_SET(listenfd, &all_fd_set);

    // servidor fica aceitando novas conexões enquanto não ocorre erro,
    // porém sequencialmente (trata a comunicação atual antes de esperar pela próxima)
    for (;;) {

        // Copia fdset de all_fd_set para read_set
        read_set = all_fd_set;

        // Define timeout para select
        tv.tv_sec = 2;
        tv.tv_usec = 500000;

        // Chama funçao select, esperando por notificaçoes para os file descriptors de read_set
        select(maxfd + 1, &read_set, NULL, NULL, &tv);

        // Verifica se ha uma nova conexao
        if (FD_ISSET(listenfd, &read_set)) {
            // Aceita uma nova conexão e salva as informações em connfd
            connfd = Accept(listenfd, (struct sockaddr *) &bound_addr, &len);

            printf("Connected to %d - %s:%d\n",
                   n_clients + 1,
                   inet_ntoa(bound_addr.sin_addr),
                   ntohs(bound_addr.sin_port));

            // Encontra posicao vazia no vetor de clientes
            for (i = 0; i < FD_SETSIZE; i++) {
                if (clients[i] < 0) {
                    clients[i] = connfd;
                    break;
                }
            }

            // Define o numero de clientes conectados
            n_clients = max(i, n_clients);

            // Define o maior file descriptor a ser observado
            maxfd = max(maxfd, connfd);

            // Define file descriptor da nova conexao para ser observado
            FD_SET(connfd, &all_fd_set);
        }

        for (i = 0; i <= n_clients; i++) {
            // Verifica se cliente possui dados a serem lidos
            if (clients[i] >= 0 && FD_ISSET(clients[i], &read_set)) {
                // le o conteúdo enviado pelo servidor e armazena em recvline
                new_s = (int) read_line(clients[i], recvline, MAX_LINE);

                // verifica se a leitura do socket falhou
                if (new_s < 0) {
                    // imprime a informação do erro e termina o programa
                    perror("read error");
                    exit(1);
                } else if (new_s == 0) {
                    // remove cliente do vetor de clientes
                    clients[i] = -1;
                    FD_CLR(clients[i], &all_fd_set);
                } else {
                    strcpy(buf, recvline);

                    // imprime mensagem recebida do cliente
                    printf("%d -> %s", i, buf);

                    // escreve a resposta no socket do cliente
                    write(clients[i], buf, strlen(buf));
                }
            }
        }
    }

    return 0;
}

int max(int val1, int val2) {
    return (val1 > val2 ? val1 : val2);
}