#include <sys/time.h>
#include "common.h"

/// Protocolo de mensagens entre cliente e servidor:
///
/// Mensagens enviadas pelo cliente:
/// Type (0 - entertainment, 1 - comfort/bla, 2 - safety); length ([0, 5]); direction ({0, 1}); position ([0, 50]); speed ([-MAX_LONG_LONG, MAX_LONG_LONG])
///
/// Mensagens enviadas pelo servidor:
/// Type (0 - entertainment, 1 - comfort/bla, 2 - safety); action (0 - Accelerate, 1 - Brake, 2 - Call ambulance)

// Função que retorna, em time_arrival e time_leaving, o intervalo de tempo em que um carro C ocupa
// o cruzamento definido pela posição gridPos.
void times_in_grid(int gridPos, Car c, double *time_arrival, double *time_leaving) {

    // Presume-se que a posição do carro é o quadrado mais pro seu fundo, de forma que ele ocupa
    // as posições [position, position + length]
    if (c.speed > 0) {

        int distToGrid = gridPos - c.position;
        distToGrid -= c.length - 1;

        *time_arrival = c.initial_time + (float)distToGrid / c.speed;
        *time_leaving = *time_arrival + ((float)(c.length + 1) / (float)c.speed);

    } else if (c.speed < 0) {

        // Para velocidades positivas e negativas, ja que a posicao é sempre a traseira do carro
        int distToGrid = c.position - (gridPos + 1);

        *time_arrival = c.initial_time - (float)distToGrid / c.speed;
        *time_leaving = *time_arrival - ((float)(c.length + 1)/(float)c.speed);

    } else {

        *time_arrival = -1;
        *time_leaving = -1; // Vai ter codigo para, se o tempo for negativo, mandar chamar a ambulancia

    }
}

// Checa se haverá colisão entre o carro horizontal 'cur' e os verticais de 'cars'
bool check_crash(Car* cars, Car cur, int gridPos, int n_cars) {
    double time_a_horz, time_l_horz;
    double time_a_vert, time_l_vert;

    times_in_grid(gridPos, cur, &time_a_horz, &time_l_horz);

    for (int j = 0; j < n_cars; j++) {

        times_in_grid(gridPos, cars[j], &time_a_vert, &time_l_vert);

        if ((time_a_horz > time_a_vert && time_a_horz < time_l_vert) ||
            (time_l_horz > time_a_vert && time_l_horz < time_l_vert)) {
            return true;
        }
    }

    return false;
}

bool check_has_crashed(Car* cars, Car cur, int n_cars) {

    for (int j = 0; j < n_cars; j++) {

        if (cars[j].position >= cur.position && cars[j].position <= cur.position + cur.length) {
            return true;
        }
    }

    return false;
}

int current_time() {
    struct timeval te;

    gettimeofday(&te, NULL);

    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;

    return (int) (milliseconds / 60000);
}

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
