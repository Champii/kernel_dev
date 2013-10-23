#ifndef                       __SOCKET_HELPER_H__
# define                      __SOCKET_HELPER_H__

# include                     <linux/net.h>
# include                     <linux/socket.h>

unsigned int                  inet_addr(char *);
int                           write_socket(struct socket *, void *);
int                           read_socket(struct socket *, void *);

#endif                        /* __SOCKET_HELPER_H__ */