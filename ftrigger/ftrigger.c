#include                      <linux/module.h>
#include                      <linux/net.h>
#include                      <linux/socket.h>
#include                      <linux/in.h>
#include                      <linux/slab.h>
#include                      <linux/fs.h>
#include                      <asm/uaccess.h>

#include                      "include/ftrigger.h"
#include                      "include/socket_helper.h"
#include                      "include/protocole.h"

struct socket                 *socket = NULL;


static struct dentry          *ftrigger_mount(struct file_system_type *fstype, int flags, const char *name, void *data)
{
  printk(KERN_INFO "MOUNT !");
  return NULL;
}

static void                   ftrigger_kill_sb(struct super_block *sb)
{
  printk(KERN_INFO "KILL SB!");

}

struct file_system_type       fstype =
{
  .name = "ftrigger",
  .fs_flags = 0,
  .mount = ftrigger_mount,
  .kill_sb = ftrigger_kill_sb,
  .owner = THIS_MODULE,
  .next = NULL,
  // .s_lock_key = "ftrigger",
  // .s_umount_key = "ftrigger",
};

static int                    socket_init(void)
{
  int                         r = -1;
  struct sockaddr_in          servaddr;

  if ((r = sock_create_kern(PF_INET, SOCK_STREAM, IPPROTO_TCP, &socket)) < 0)
  {
    printk(KERN_INFO "Error socket : sock_create : ret = %d\n", r);

    return r;
  }

  memset(&servaddr,0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(12345);
  servaddr.sin_addr.s_addr = inet_addr("192.168.0.12");

  printk(KERN_INFO "Socket = %x, servaddr = %x\n", socket, &servaddr);

  if ((r = socket->ops->connect(socket, (struct sockaddr *) &servaddr, sizeof(servaddr), O_RDWR)) < 0)
  {
    printk(KERN_INFO "Error socket : connect : ret = %d\n", r);
    socket->ops->release(socket);

    return r;
  }

  return 0;
}

static int __init             ftrigger_init(void)
{
  int                         r = -1;


  if ((r = register_filesystem(&fstype)) < 0)
  {
    printk(KERN_INFO "Error registrating filesystem : ret = %d", r);

    return r;
  }

//   struct s_proto              test =
//   {
//     .i = 1,
//     .c = 'a',
//   };

//   if ((r = socket_init()) < 0)
//     return r;

// /* tests */

//   if ((r = write_socket(socket, &test)) < 0)
//     return r;

//   if ((r = read_socket(socket, &test)) < 0)
//     return r;
//   else
//     printk(KERN_INFO "i = %d, c = %c", test.i, test.c);

/* /tests */

  return 0;
}

static void __exit            ftrigger_exit(void)
{
  int                         r;

  if ((r = unregister_filesystem(&fstype)) < 0)
    printk(KERN_ERR "Error unregistering filesystem : ret = %d", r);

  // socket->ops->release(socket);
}

module_init(ftrigger_init);
module_exit(ftrigger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Greiner <greine_f@epitech.eu>");
MODULE_DESCRIPTION("Our First Serial Driver");
