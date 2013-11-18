#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


#define NETMALLOC_AUTHOR "Florian | Cliff"
#define NETMALLOC_DESC "SysCall which create a specific memory region in the calling process."

static int __init net_malloc_start() {

}

static int __exit net_malloc_exit() {

}

module_init(net_malloc_start);
module_exit(net_malloc_exit);

MODULE_AUTHOR(NETMALLOC_AUTHOR)
MODULE_DESCRIPTION(NETMALLOC_DESC)
