#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int bool;
enum { false, true };

typedef struct car {
    short int length;
    short int direction; // 1 vertical, 0 horizontal
    short int position;
    long long int speed;
    int initial_time;
} Car;


void times_in_grid(int gridPos, Car c, double *time_arrival, double *time_leaving);

bool check_crash(Car* cars, Car cur, int gridPos, int n_cars);

bool check_has_crashed(Car* cars, Car cur, int n_cars);

int Socket(int family, int type, int flags);

void Connect(int sockfd,
            struct sockaddr *sockaddr,
            int socklen);

void Bind(int sockfd,
          struct sockaddr *sockaddr,
          int socklen);

void Listen(int sockfd,
            int back_log);

int Accept(int sockfd,
          struct sockaddr *sockaddr,
          socklen_t *socklen);

ssize_t read_line(int fd, void *buffer, size_t n);

int current_time();