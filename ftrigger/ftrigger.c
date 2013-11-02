#include                      <linux/module.h>
#include                      <linux/net.h>
#include                      <linux/socket.h>
#include                      <linux/in.h>
#include                      <linux/slab.h>
#include                      <linux/fs.h>
#include                      <linux/namei.h>
#include                      <linux/dcache.h>
#include                      <linux/kthread.h>
#include                      <linux/delay.h>

#include                      <asm/uaccess.h>
#include                      "include/ftrigger.h"
#include                      "include/socket_helper.h"
#include                      "include/protocole.h"

#define FTRIGGER_BASE_INODE_MODE S_IFDIR | S_IRWXU | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH

#define FT_MAGIC 0x42424242

struct socket                 *socket = NULL;
struct dentry                 *root = NULL;
struct super_block            *sb = NULL;

// int                           send_open_request(struct file *file)
// {
//   struct s_proto              proto =
//   {
//     .code = 4,
//     .args = file.name,
//   };
//   return 0;
// }

int                           ftrigger_f_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "Open ! %s", inode->i_private);
  file->private_data = inode->i_private;

  return 0;
}

ssize_t                       ftrigger_f_read(struct file *file, char __user *buff, size_t len, loff_t *off)
{
  struct s_proto proto =
  {
    .code = 4,
  };
  strcpy(proto->args, file->private_data);

  send_socket(socket, &proto);
  printk(KERN_INFO "Read !");
  return 0;
}

ssize_t                       ftrigger_f_write(struct file *file, const char __user *buff, size_t len, loff_t *off)
{
  printk(KERN_INFO "Write !");
  return 0;
}

struct file_operations        i_fop =
{
  .open = ftrigger_f_open,
  .read = ftrigger_f_read,
  .write = ftrigger_f_write,
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
  else
    printk(KERN_INFO "ERROR : NO New inode");
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
  inode->i_size = 4096;

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
  char  *mount_point_buff = kmalloc(255, GFP_KERNEL);

  struct s_proto              test =
  {
    .code = 1,
    .args = "",
  };

  save_mount_options(superblock, data);
  superblock -> s_op = &s_op;

  superblock -> s_maxbytes = 10000000;
  superblock -> s_blocksize = PAGE_SIZE;
  superblock -> s_blocksize_bits = PAGE_SHIFT;
  superblock -> s_magic = FT_MAGIC;
  inode -> i_ino = iunique(superblock, 0);
  inode_init_owner(inode, NULL, FTRIGGER_BASE_INODE_MODE);
  inode->i_op = &simple_dir_inode_operations;
  inode->i_fop = &simple_dir_operations;
  inode->i_size = 4096;
  inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
  inode->i_flags = S_DIRSYNC;
  rootPath = d_make_root(inode);
  if (!rootPath) {
      return -ENOMEM;
  }
  mount_point = dentry_path_raw(rootPath, mount_point_buff, 255);
  printk(KERN_INFO "Mount point = %s", mount_point);
  superblock -> s_root = rootPath;

  root = rootPath; //Global root
  sb = superblock;

  if ((r = write_socket(socket, &test)) < 0)
    return r;

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

  if ((r = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &socket)) < 0)
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
  struct dentry               *currentDir = root;
  struct dentry               *parentDir = root;
  struct dentry               *created = NULL;

  int i = 0;

  tab = str_to_wordtab(proto->args, '/');
  while (i < count_word(proto->args, '/'))
  {
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
        if ((created = ftrigger_create_file(sb, parentDir, tab[i])) == NULL)
        {
          printk(KERN_ERR "Cannot create file %s", tab[i]);
          return -1;
        }
        else
          created->d_inode->i_private = proto->args;
      }
      else if (proto->code == FOLDER)
      {
        if ((r = ftrigger_create_dir(sb, parentDir, tab[i])) == NULL)
        {
          printk(KERN_ERR "Cannot create folder %s", tab[i]);
          return -1;
        }
      }
    }
    else if (currentDir->d_name.name == &name.name)
    {
      printk(KERN_INFO "ERROR : Existing path ! ");
      return -1;
    }
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
    memset(mess, 0, sizeof(struct s_proto));

    if ((r = read_socket(socket, mess)) <= 0)
    {
      printk(KERN_ERR "read_socket returned r = %d", r);
      return r;
    }

    if (!mess)
    {
      printk(KERN_ERR "Noting read !");
      return r;
    }

    // printk(KERN_INFO "read length = %d, mess.code = %d, mess.args = %s", r, mess->code, mess->args);

    if ((r = create_path(mess)) < 0)
      return r;

    kfree(mess);
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
