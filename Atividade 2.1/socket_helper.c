#include "socket_helper.h"

int Socket(int family, int type, int flags) {
    int sockfd;
    if ((sockfd = socket(family, type, flags)) < 0) {
        perror("socket");
        exit(1);
    } else {
        return sockfd;
    }
}

void Connect(int sockfd, struct sockaddr *sockaddr, int socklen) {
    if (connect(sockfd, sockaddr, (socklen_t) socklen) < 0) {
        // imprime a informação do erro e terminamos o programa
        perror("connect error");
        exit(1);
    }
}

void Bind(int sockfd, struct sockaddr *sockaddr, int socklen) {
    if (bind(sockfd, sockaddr, (socklen_t) socklen) == -1) {
        // caso o ocorra erro durante a atribuição do endereço,
        // imprime o erro e terminamos o programa
        perror("bind");
        exit(1);
    }
}

void Listen(int listenfd, int back_log) {
    if (listen(listenfd, back_log) == -1) {
        // caso ocorra um erro nesse processo,
        // imprimimos o erro e terminamos o programa
        perror("listen");
        exit(1);
    }
}

int Accept(int sockfd, struct sockaddr *sockaddr, socklen_t *socklen) {
    int connfd;
    if ((connfd = accept(sockfd, sockaddr, socklen)) == -1) {
        // caso o ocorra erro ao aceitar uma conexao
        // imprime o erro e terminamos o programa
        perror("accept");
        exit(1);
    }

    return connfd;
}

// Funçao para ler uma unica linha de um file descriptor.
// Autor: Michael Kerrisk - The Linux Programming Interface
ssize_t read_line(int fd, void *buffer, size_t n) {
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}
