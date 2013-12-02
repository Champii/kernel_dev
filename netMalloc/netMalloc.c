#include <linux/sched.h>
#include <linux/kernel.h>


// thoses function allow to build a new page tables to map a range a physical addresses
int remap_pfn_range(struct vm_area_struct *vma,
		    unsigned long virt_addr, 
		    unsigned long pfn,
		    unsigned long size, 
		    pgprot_t prot);

int io_remap_page_range(struct vm_area_struct *vma,
			unsigned long virt_addr, 
			unsigned long phys_addr,
			unsigned long size, 
			pgprot_t prot);


// These operations are called whenever a process opens or closes the VMA; in particular, the open
// method is invoked anytime a process forks and creates a new reference to the VMA
void simple_vma_open(struct vm_area_struct *vma)
{
  printk(KERN_NOTICE "Simple VMA open, virt %lx, phys %lx\n",
	 vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}
void simple_vma_close(struct vm_area_struct *vma)
{
  printk(KERN_NOTICE "Simple VMA close.\n");
}

static struct vm_operations_struct simple_remap_vm_ops = {
  .open = simple_vma_open,
  .close = simple_vma_close,
  // rajouter fault check dans mmh
};



static int simple_remap_mmap(struct file *filp, struct vm_area_struct *vma)
{
  if (remap_pfn_range(vma, vma->vm_start, vm->vm_pgoff,
		      vma->vm_end - vma->vm_start,
		      vma->vm_page_prot))
    return -EAGAIN;
  vma->vm_ops = &simple_remap_vm_ops;
  simple_vma_open(vma); 
  return 0;
}

asmlinkage int sys_net_malloc(unsigned long size) {
  // structure qui contient les informations sur l’espace mémoire du processus cible // info ->mm
  struct task_struct *current;
  // une description de la région que vous voulez rajouter 
  struct vm_area_struct *vma;
  
  // contient le fault handler...
  struct vm_struct_operations* ops ;

  unsigned long newbrk, oldbrk;
  
  current = (struct vm_area_struct *)kmalloc(sizeof(struct vm_area_struct), GFP_KERNEL);

  vma = (struct vm_area_struct *)kmalloc(sizeof(struct vm_area_struct), GFP_KERNEL);
  if (!vma)
    return -ENOMEM;
  
  current = current_thread_info(); // check for include

  //   
  vma->vm_mm = current->mm; 

  // The virtual address range covered by this VMA. These fields are the first two fields
  vma->vm_start = addr; // a voir
  vma->vm_end = addr+len; // a voir
  
  //  A set of flags describing this area. The flags of the most interest to device driver
  //  writers are VM_IO and VM_RESERVED. VM_IO marks a VMA as being a memory-
  //  mapped I/O region. Among other things, the VM_IO flag prevents the region from
  //  being included in process core dumps. VM_RESERVED tells the memory manage-
  //  ment system not to attempt to swap out this VMA; it should be set in most
  //  device mappings.
  vma->vm_flags = 0;
  
 
  

  // contien un pointeur sur la struct operation
  vma->vm_ops = NULL;

  // permet d'ajouter une region memmoire
  // insert_vm_struct();

  printk("hello i am the syscall name's netMalloc");
  return 0;
}
