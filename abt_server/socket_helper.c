#include "socket_helper.h"

typedef struct car{
	short int length;
	short int direction;
	short int position;
	long long int speed;
} Car;

/// Type (0 - entertainment, 1 - comfort/bla, 2 - safety); length ([0, 5]); direction ({0, 1}); position ([0, 50]); speed ([-MAX_LONG_LONG, MAX_LONG_LONG])
char client_message[] = "%d;%d;%d;%d;%d;";

/// Type (0 - entertainment, 1 - comfort/bla, 2 - safety); action (0 - Accelerate, 1 - Brake, 2 - Call your momma)
char server_message[] = "%d;%d";

// Função que retorna, em time_arrival e time_leaving, o intervalo de tempo em que um carro C ocupa
// o cruzamento definido pela posição gridPos.
void times_in_grid(int gridPos, Car c, long int seconds_since_sim_start, double *time_arrival, double *time_leaving)
{
	// Presume-se que a posição do carro é o quadrado mais pro seu fundo, de forma que ele ocupa
	// as posições [position, position + length]
	if (c.speed > 0) // Os calculos sao meio diferentes
	{
		int distToGrid 	= gridPos - c.position;
		distToGrid 		-= c.length - 1;
		
		double timeIn = seconds_since_sim_start;
		*time_arrival = timeIn + (float)distToGrid / c.speed;
		*time_leaving = *time_arrival + ((float)(c.length + 1)/(float)c.speed); 
	}
	
	else if (c.speed < 0) // Pra velocidades positivas e negativas, ja q a posicao é sempre a "bunda" do carro
	{
		int distToGrid 	= c.position - (gridPos + 1);
		
		double timeIn = seconds_since_sim_start;
		*time_arrival = timeIn - (float)distToGrid / c.speed;
		*time_leaving = *time_arrival - ((float)(c.length + 1)/(float)c.speed); 
	}
	
	else // velocidade = 0
	{
		*time_arrival = -1;
		*time_leaving = -1; // Vai ter codigo para, se o tempo for negativo, mandar chamar a ambulancia
	}
}

// Função que simplesmente altera a velocidade de um carro. Cliente a utiliza para atualizar o carro ao 
// receber uma mensagem "Acelere!" do servidor.
void accelerate(Car *c)
{
	if (c->speed < 0)
	{
		c->speed -= 2;
	}
	
	else // Velocidade nunca vai ser 0; accelerate só é chamada depois de times_in_grid, que já checa isso
	{
		c->speed += 2; // Mas se for, aceleramos de qq forma
	}
}

void read_client_message(char* msg, short int* len, short int* dir, short int* pos, long long int* spd)
{
	// Sabemos que a estrutura da mensagem é "%d;%d;%d;%d;%d", então os dois primeiros caracteres já terão sido
	// lidos quando esta função for chamada. Lemos então a partir de i = 2.
	int ind_0 = 0;
	int ind_1 = 0;
	long long int vals[4];
	for (int i = 2; msg[i] != '\0'; i++)
	{
		char substr[20];
		if (msg[i] != ';')
		{
			substr[ind_0] = msg[i];
			ind_0++;
		}
		else
		{
			substr[ind_0] = '\0';
			ind_0 = 0;
			vals[ind_1] = atoi(substr);
		}
	}
	
	*len = vals[0];
	*dir = vals[1];
	*pos = vals[2];
	*spd = vals[3];
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
