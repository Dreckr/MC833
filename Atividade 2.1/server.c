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
    int new_s;

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

    printf("Listening for connections on port %d...\n", SERVER_PORT);

    // servidor fica aceitando novas conexões enquanto não ocorre erro,
    // porém sequencialmente (trata a comunicação atual antes de esperar pela próxima)
    for (;;) {

        // espera por uma conexão e salva as informações em connfd
        connfd = Accept(listenfd, (struct sockaddr *) &bound_addr, &len);

        if (fork() == 0) {
            close(listenfd);

            printf("Connected to %s:%d\n", inet_ntoa(bound_addr.sin_addr), ntohs(bound_addr.sin_port));

            do {
                // le o conteúdo enviado pelo servidor e armazena em recvline
                new_s = (int) read_line(connfd, recvline, MAX_LINE);

                // verifica se a leitura do socket falhou
                if (new_s < 0) {
                    // imprime a informação do erro e termina o programa
                    perror("read error");
                    exit(1);
                } else if (new_s > 0) {
                    strcpy(buf, recvline);

                    // imprime mensagem recebida do cliente
                    printf("%s:%d -> %s", inet_ntoa(bound_addr.sin_addr), ntohs(bound_addr.sin_port), buf);

                    // escreve a resposta no socket do cliente
                    write(connfd, buf, strlen(buf));
                }
            } while (new_s > 0);

            // finaliza a comunicação com o cliente
            close(connfd);

            exit(0);
        }
    }

    return 0;
}
