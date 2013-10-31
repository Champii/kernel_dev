#include                      <linux/module.h>
#include                      <linux/net.h>
#include                      <linux/socket.h>
#include                      <linux/in.h>
#include                      <linux/slab.h>
#include                      <linux/fs.h>
#include                      <linux/namei.h>
#include                      <linux/kthread.h>
#include                      <asm/uaccess.h>
#include                      <linux/delay.h>
#include                      "include/ftrigger.h"
#include                      "include/socket_helper.h"
#include                      "include/protocole.h"

#define FTRIGGER_BASE_INODE_MODE S_IFDIR | S_IRWXU | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH

#define FT_MAGIC 0x42424242

struct socket                 *socket = NULL;
struct dentry                 *root = NULL;
struct super_block            *sb = NULL;

int                           ftrigger_f_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "Open !");

  return 0;
}

struct file_operations        i_fop =
{
  .open = ftrigger_f_open,
};

int                           ftrigger_i_create(struct inode *inode,struct dentry *dentry,int i, struct nameidata *name)
{
  printk(KERN_INFO "Create !");

  return 0;
}

struct dentry                 *ftrigger_i_lookup(struct inode *inode, struct dentry *dentry, struct nameidata *nameidata)
{
  printk(KERN_INFO "nameidata path = %s", nameidata->path.dentry->d_name.name);
  return NULL;
}

int                           ftrigger_s_statfs(struct dentry *dentry, struct kstatfs *kstat)
{
  printk(KERN_INFO "Statfs !");

  return 0;
}

static struct inode *ftrigger_make_inode(struct super_block *sb, int mode)
{
  struct inode *ret = new_inode(sb);

  if (ret) {
    ret->i_mode = mode;
    ret->i_uid = ret->i_gid = 0;
    ret->i_blocks = 4096;
    ret->i_ino = iunique(sb, 0);
    ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
  }
  return ret;
}

struct inode                  *ftrigger_s_alloc_inode(struct super_block *sb)
{
  printk(KERN_INFO "alloc inode !");

  return ftrigger_make_inode(sb, S_IFDIR | 0644);
}

int                           ftrigger_i_permission(struct inode *inode, int mode)
{
  printk(KERN_INFO "Permission !");
  return FTRIGGER_BASE_INODE_MODE;
}

struct inode_operations       i_op =
{
  .create = ftrigger_i_create,
  .lookup = ftrigger_i_lookup,
  // .permission = ftrigger_i_permission,
};

struct super_operations       s_op =
{
  .statfs = ftrigger_s_statfs,
  // .alloc_inode = ftrigger_s_alloc_inode,
};

struct dentry                 *d_make_root(struct inode *root_inode)
{
  struct dentry               *res = NULL;

  if (root_inode)
  {
    res = d_alloc_root(root_inode);
    if (!res)
    {
      printk(KERN_INFO "Alloc root = null");
      iput(root_inode);
    }
  }
  return res;
}

static struct dentry          *ftrigger_create_file (struct super_block *sb, struct dentry *dir, const char *name)
{
  struct dentry *dentry;
  struct inode *inode;
  struct qstr qname;

  qname.name = name;
  qname.len = strlen (name);
  qname.hash = full_name_hash(name, qname.len);

  dentry = d_alloc(dir, &qname);
  if (! dentry)
    goto out;
  inode = ftrigger_make_inode(sb, S_IFREG | 0644);
  if (! inode)
    goto out_dput;
  inode->i_fop = &i_fop;

  d_add(dentry, inode);
  return dentry;

  out_dput:
  dput(dentry);
  out:
  return 0;
}


static struct dentry          *ftrigger_create_dir (struct super_block *sb,
    struct dentry *parent, const char *name)
{
  struct dentry *dentry;
  struct inode *inode;
  struct qstr qname;

  qname.name = name;
  qname.len = strlen (name);
  qname.hash = full_name_hash(name, qname.len);
  dentry = d_alloc(parent, &qname);
  if (! dentry)
    goto out;

  inode = ftrigger_make_inode(sb, S_IFDIR | 0644);
  if (! inode)
    goto out_dput;
  inode->i_op = &simple_dir_inode_operations;
  inode->i_fop = &simple_dir_operations;
  inode->i_blocks = 4096;

  d_add(dentry, inode);
  return dentry;

  out_dput:
  dput(dentry);
  out:
  return 0;
}

static int                    ftrigger_super(struct super_block *superblock, void *data, int silent)
{
  struct inode *inode = new_inode(superblock);
  struct dentry *rootPath;
  int r = 0;

  save_mount_options(superblock, data);
  superblock -> s_op = &s_op;

  superblock -> s_maxbytes = 10000000;
  superblock -> s_blocksize = PAGE_SIZE;
  superblock -> s_blocksize_bits = PAGE_SHIFT;
  superblock -> s_magic = FT_MAGIC;
  inode -> i_ino = iunique(superblock, 0);
  inode_init_owner(inode, NULL, FTRIGGER_BASE_INODE_MODE);
  // inode_sb_list_add(inode);
  inode->i_op = &simple_dir_inode_operations;
  inode->i_fop = &simple_dir_operations;
  // inode -> i_op = &i_op;
  // inode -> i_fop = &i_fop;
  // inode->i_blocks = 8;
  inode->i_size = 4096;
  // inode->i_bytes = 4096;
  inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
  inode->i_flags = S_DIRSYNC;
  rootPath = d_make_root(inode);
  if (!rootPath) {
      return -ENOMEM;
  }
  superblock -> s_root = rootPath;

  root = rootPath; //Global root
  sb = superblock;

  struct s_proto              test =
  {
    .code = 1,
    .args = "",
  };
  if ((r = write_socket(socket, &test)) < 0)
    return r;

  // char i = 'A';
  // char j = 'A';
  // char buff[3];
  // while (i < 'Z')
  // {
  //   while (j < 'Z')
  //   {
  //     buff[0] = i;
  //     buff[1] = j++;
  //     ftrigger_create_dir(superblock, root, buff);
  //     printk(KERN_INFO "Created : %s", buff);
  //     // mdelay(500);
  //   }
  //   j = 'A';
  //   i++;
  // }

  // ftrigger_create_file(superblock, ftrigger_create_dir(superblock, root, "test2"), "test");
  return 0;
}

static struct dentry          *ftrigger_mount(struct file_system_type *fstype, int flags, const char *name, void *data)
{
  return mount_nodev(fstype, flags, data, &ftrigger_super);
}

static void                   ftrigger_kill_sb(struct super_block *sb)
{
  printk(KERN_INFO "KILL SB!");

}

struct file_system_type       fstype =
{
  .name = "ftrigger",
  .fs_flags = FS_BINARY_MOUNTDATA | FS_REVAL_DOT | FS_RENAME_DOES_D_MOVE,
  .mount = ftrigger_mount,
  .kill_sb = ftrigger_kill_sb,
  .owner = THIS_MODULE,
  .next = NULL,
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
  servaddr.sin_addr.s_addr = inet_addr("192.168.56.1");

  printk(KERN_INFO "Socket = %x, servaddr = %x\n", socket, &servaddr);

  if ((r = socket->ops->connect(socket, (struct sockaddr *) &servaddr, sizeof(servaddr), O_RDWR)) < 0)
  {
    printk(KERN_INFO "Error socket : connect : ret = %d\n", r);
    socket->ops->release(socket);

    return r;
  }

  return 0;
}

int                           create_path(struct s_proto *proto)
{
  char                        **tab;
  struct qstr                 name;
  int                         r = 0;
  // struct s_proto              *proto = data;
  struct dentry               *currentDir = root;
  struct dentry               *parentDir = root;

  int i = 0;
  tab = str_to_wordtab(proto->args, '/');
  while (i < count_word(proto->args, '/'))
  {
    printk(KERN_INFO "Word : %s, currentDir = %x, nbWord = %d", tab[i], currentDir, count_word(proto->args, '/'));
    name.name = tab[i];
    name.len = strlen(tab[i]);
    name.hash = full_name_hash(name.name, name.len);

    if (!currentDir)
    {
      printk(KERN_INFO "NO more dir");
      return 0;
    }

    parentDir = currentDir;
    currentDir = d_lookup(parentDir, &name);
    if (!currentDir)
    {
      if (proto->code == FILE)
      {
        if ((r = ftrigger_create_file(sb, parentDir, tab[i])) == NULL)
        {
          printk(KERN_ERR "Cannot create file %s", tab[i]);
          return -1;
        }
      }
      else if (proto->code == FOLDER)
      {
        if ((r = ftrigger_create_dir(sb, parentDir, tab[i])) == 0)
        {
          printk(KERN_ERR "Cannot create folder %s", tab[i]);
          return -1;
        }
      }
    }
    // if (parentDir && parentDir != root)
    //   dput(parentDir);

    printk(KERN_INFO "created file");
    i++;
  }
  return 1;
}

int                           async_read(void *data)
{
  int                         r;
  struct s_proto              *mess = NULL;
  // struct task_struct          *file_thread;

  for (;;)
  {
    mess = kmalloc(sizeof(struct s_proto), GFP_KERNEL);
    if (!mess)
    {
      printk(KERN_ERR "No more memory to alloc");
      return -1;
    }

    if ((r = read_socket(socket, mess)) < 0)
      return r;

    printk(KERN_INFO "mess.code = %d, mess.args = %s", mess->code, mess->args);
    // kthread_run(&create_path, mess, "file_thread");
    create_path(mess);
  }
  return 0;
}

static int __init             ftrigger_init(void)
{
  int                         r = -1;
  struct task_struct          *thread;


  if ((r = register_filesystem(&fstype)) < 0)
  {
    printk(KERN_INFO "Error registrating filesystem : ret = %d", r);

    return r;
  }

  if ((r = socket_init()) < 0)
    return r;

  thread = kthread_run(&async_read, NULL, "async_read");

  if (!thread)
  {
    printk(KERN_INFO "Error creating threads");

    return -1;
  }

  return 0;
}

static void __exit            ftrigger_exit(void)
{
  int                         r;

  if ((r = unregister_filesystem(&fstype)) < 0)
    printk(KERN_ERR "Error unregistering filesystem : ret = %d", r);

  socket->ops->release(socket);
}

module_init(ftrigger_init);
module_exit(ftrigger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Greiner <greine_f@epitech.eu>");
MODULE_DESCRIPTION("Our First Serial Driver");
