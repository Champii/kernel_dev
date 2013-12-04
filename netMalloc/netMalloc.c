#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/thread_info.h>
#include <linux/mm.h>
#include <linux/list.h>

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

void vma_open(struct vm_area_struct *vma) {
  printk("Function : vma_open\n");
}

void vma_close(struct vm_area_struct *vma) {
  printk("Function : vma_close\n");
}

void vma_fault(struct vm_area_struct *vma) {
   printk("Function : vma_fault\n");
}

static struct vm_operation_struct operations = {
  .open = vma_open,
  .close = vma_close,
  .flaut = vma_fault
};

void init_struct(struct task_struct *curr, struct vm_area_struct *vma) {
  struct vm_struct_operations *ops;
  struct list_head anon_vma_chain;

  curr = (struct task_struct *)kmalloc(sizeof(struct task_struct), GFP_KERNEL);
  vma = (struct vm_area_struct *vma)kmalloc(sizeof(struct vm_area_struct), GFP_KERNEL);
  if (!curr && !vma)
    return -ENOMEM;
  
  curr = current_thread_info();

  vma->vm_mm = curr->mm;
  vma->vm_start = (char*) KERN_PG_HEAP; // checker
  vma->vm_end = (char*) KERN_PG_HEAP_LIM; // checker
  vma->flags = VM_READ | VM_WRITE | VM_EXEC;
  vma->anon_vma_chain = INIT_LIST_HEAD(&anon_vma_chain);
  vma->vm_ops = &operations;
}

asmlinkage int sys_net_malloc(unsigned long size_ask) {
  
  // structure qui contient les informations sur l’espace mémoire du processus cible // info ->mm
  struct task_struct *curr;  
  // une description de la région que vous voulez rajouter 
  struct vm_area_struct *vma;

  // function to initialaze all struct we need
  init_struct(current, vma);
  
  // permet d'ajouter une region memmoire
  // insert_vm_struct(curr, vma);

  printk("hello i am the syscall name's netMalloc");
  return 0;
}
