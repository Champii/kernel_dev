#include                      <linux/module.h>
#include                      <linux/net.h>
#include                      <linux/socket.h>
#include                      <linux/in.h>
#include                      <linux/slab.h>
#include                      <asm/uaccess.h>

#include                      "include/socket_helper.h"
#include                      "include/protocole.h"

unsigned int                  inet_addr(char *str)
{
  int                         a, b, c, d;
  char                        arr[4];

  sscanf(str,"%d.%d.%d.%d",&a,&b,&c,&d);
  arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;

  return *(unsigned int*)arr;
}

int                           write_socket(struct socket *socket, void *buffer)
{
  int                         r = 0;
  mm_segment_t                oldfs;

  struct iovec                iov =
  {
    .iov_base = buffer,
    .iov_len  = sizeof(struct s_proto),
  };

  struct msghdr               msg =
  {
    .msg_name       = 0,
    .msg_namelen    = 0,
    .msg_iov        = &iov,
    .msg_iovlen     = 1,
    .msg_control    = NULL,
    .msg_controllen = 0,
    .msg_flags      = 0,
  };

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  if ((r = sock_sendmsg(socket, &msg, sizeof(struct s_proto))) < 0)
  {
    printk(KERN_INFO "Error socket : send : ret = %d\n", r);

    socket->ops->release(socket);
    set_fs(oldfs);
    return r;
  }

  set_fs(oldfs);
  return 0;
}

int                           read_socket(struct socket *socket, void *buffer)
{
  int                         r = 0;
  mm_segment_t                oldfs;

  struct iovec                iov =
  {
    .iov_base = buffer,
    .iov_len  = sizeof(struct s_proto),
  };

  struct msghdr               msg =
  {
    .msg_name       = 0,
    .msg_namelen    = 0,
    .msg_iov        = &iov,
    .msg_iovlen     = 1,
    .msg_control    = NULL,
    .msg_controllen = 0,
    .msg_flags      = 0,
  };

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  if ((r = sock_recvmsg(socket, &msg, sizeof(struct s_proto), MSG_WAITALL)) <= 0)
  {
    printk(KERN_INFO "Error socket : send : ret = %d\n", r);

    socket->ops->release(socket);
    set_fs(oldfs);
    return r;
  }

  set_fs(oldfs);
  return r;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Greiner <greine_f@epitech.eu>");
MODULE_DESCRIPTION("Our First Serial Driver");
