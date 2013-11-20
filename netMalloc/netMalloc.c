#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#define NETMALLOC_AUTHOR "Florian Greiner <greine_f@epitech.eu> | Cliff Grente <cgrente@gmail.com>"
#define NETMALLOC_DESC "SysCall which create a specific memory region in the calling process."



static int __init net_malloc_start(unsigned long size) {
  puts("toto");
}

static int __exit net_malloc_exit(void) {

}

module_init(net_malloc_start);
module_exit(net_malloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(NETMALLOC_AUTHOR)
MODULE_DESCRIPTION(NETMALLOC_DESC)
