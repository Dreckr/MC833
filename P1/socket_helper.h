#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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