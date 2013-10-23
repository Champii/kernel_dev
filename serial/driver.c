#include                      <linux/module.h>
#include                      <linux/version.h>
#include                      <linux/kernel.h>
#include                      <linux/types.h>
#include                      <linux/kdev_t.h>
#include                      <linux/fs.h>
#include                      <linux/cdev.h>
#include                      <linux/sched.h>
#include                      <linux/platform_device.h>
#include                      <linux/slab.h>
#include                      <linux/mutex.h>
#include                      <asm/uaccess.h>
#include                      <asm/io.h>

#include                      "driver.h"

/* Members and early init */

static struct my_uart_port    serial_ports[1];

static struct uart_driver     udriver =
{
  .owner        = THIS_MODULE,
  .driver_name  = "driver_uart",
  .dev_name     = "ttyS",
  .major        = TTY_MAJOR,
  .minor        = 64,
  .nr           = 1,
  .cons         = NULL
};

static struct cdev            char_cdev;

static struct resource        driver_resources[] =
{
  {
    .start        = SERIAL_PORT_BASE,
    .end          = SERIAL_PORT_END,
    .flags        = IORESOURCE_MEM,
    .name         = "io-memory"
  },
  {
    .start        = 4,
    .end          = 4,
    .flags        = IORESOURCE_IRQ,
    .name         = "irq",
  }
};

static struct platform_device pdev = {
  .name           = "driver_uart",
  .resource       = driver_resources,
  .num_resources  = ARRAY_SIZE(driver_resources),
};

static struct circ_buff       buffer;

spinlock_t                    spin_buffer;

/* Functions */
static int                    my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver : Open()\n");

  return 0;
}

static int                    my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver : Close()\n");

  return 0;
}

static void                   flush_buffer(void)
{
  struct list_head            *ptr;
  struct list_head            *ptr_next;
  struct circ_buff            *buff;

  list_for_each_safe(ptr, ptr_next, &buffer.list)
  {
    buff = list_entry(ptr, struct circ_buff, list);
    list_del(ptr);
    kfree(buff);
  }
}

static ssize_t                my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  int                         i;
  u8                          msr;
  u8                          mcr;
  size_t                      len_buff = len;
  struct circ_buff            *entry;
  struct list_head            *ptr;

  // msr = inb(SERIAL_PORT_BASE + UART_MSR); // DSR/CTS ?
  // printk(KERN_INFO "Driver :   DSR = %x", msr & UART_MSR_DSR);
  // printk(KERN_INFO "Driver :   CTS = %x", msr & UART_MSR_CTS);

  // if (!(msr & UART_MSR_DSR) || !(msr & UART_MSR_CTS))
  //   return 0;

  // mcr = inb(SERIAL_PORT_BASE + UART_MCR);
  // outb(mcr | UART_MCR_RTS, SERIAL_PORT_BASE + UART_MCR); // RTS = 1


  printk(KERN_INFO "Driver : Read(), len = %d\n", len);

  // spin_lock(&spin_buffer);
  // spin_lock(&serial_ports[0].uart.lock);
  // mutex_lock(&udriver.state->port.mutex);

  for (ptr = buffer.list.next; ptr != &buffer.list && len - 1; ptr = ptr->next, --len)
  {
    entry = list_entry(ptr, struct circ_buff, list);
    *buf++ = entry->c;
    printk(KERN_INFO "Read value from buffer : %c", entry->c);
  }

  flush_buffer();

  // mutex_unlock(&udriver.state->port.mutex);
  // spin_unlock(&serial_ports[0].uart.lock);
  // spin_unlock(&spin_buffer);

  // *off = *off + len_buff - len;

  return len_buff - len;
}

static ssize_t                my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  u8                          msr;
  u8                          mcr;
  int                         i;

  printk(KERN_INFO "Driver : Write() length = %d\n", len);

  // spin_lock(&serial_ports[0].uart.lock);
  // mutex_lock(&udriver.state->port.mutex);

  mcr = inb(SERIAL_PORT_BASE + UART_MCR);
  if (!(mcr & UART_MCR_RTS) || !(mcr & UART_MCR_DTR))
    outb(mcr | UART_MCR_RTS | UART_MCR_DTR, SERIAL_PORT_BASE + UART_MCR); // RTS/DTR = 1

  msr = inb(SERIAL_PORT_BASE + UART_MSR); // DSR/CTS ?

  if (!(msr & UART_MSR_CTS))
  {
    printk(KERN_INFO "Driver :   DSR = %d", msr & UART_MSR_DSR);
    printk(KERN_INFO "Driver :   CTS = %d", msr & UART_MSR_CTS);

    return 0;
  }

  for (i = 0; i < len; i++)
  {
    outb(buf[i], SERIAL_PORT_BASE);
    printk(KERN_INFO "Written : %c", buf[i]);
  }

  // mutex_unlock(&udriver.state->port.mutex);
  // spin_unlock(&serial_ports[0].uart.lock);
  // *off = *off + len;

  return len;
}

static struct file_operations fops =
{
  .owner        = THIS_MODULE,
  .open         = my_open,
  .release      = my_close,
  .read         = my_read,
  .write        = my_write,
};

static int                    serial_driver_startup(struct uart_port *port)
{
  printk(KERN_INFO "serial_driver_startup");
  return 0;
}

static void                   serial_driver_shutdown(struct uart_port *port)
{
  printk(KERN_INFO "serial_driver_shutdown");
}

static void                   serial_driver_start_tx(struct uart_port *port)
{
  printk(KERN_INFO "serial_driver_start_tx");
}

static void                   serial_driver_stop_tx(struct uart_port *port)
{
  printk(KERN_INFO "serial_driver_stop_tx");
}

static void                   serial_driver_stop_rx(struct uart_port *port)
{
  printk(KERN_INFO "serial_driver_stop_rx");
}

static void                   serial_driver_set_mctrl(struct uart_port *port, unsigned int mctl)
{
  printk(KERN_INFO "serial_driver_set_mctl");
}

static unsigned int           serial_driver_get_mctrl(struct uart_port *port)
{
  printk(KERN_INFO "serial_driver_get_mctl");
  return 0;
}

static void                   serial_config_port(struct uart_port *port, int i)
{
  printk(KERN_INFO "serial_driver_config_port");
}

static struct uart_ops        ops =
{
  .startup      = serial_driver_startup,
  .shutdown     = serial_driver_shutdown,

  .start_tx     = serial_driver_start_tx,
  .stop_tx      = serial_driver_stop_tx,

  .set_mctrl    = serial_driver_set_mctrl,
  .get_mctrl    = serial_driver_get_mctrl,

  .stop_rx      = serial_driver_stop_rx,

  .config_port  = serial_config_port,
};


static int                    serial_driver_init_port(struct my_uart_port *port, struct platform_device *pdev)
{
  struct uart_port            *uart = &port->uart;

  uart->iotype        = UPIO_MEM;
  uart->flags         = UPF_BOOT_AUTOCONF;
  uart->type          = PORT_UNKNOWN;
  uart->ops           = &ops;
  uart->fifosize      = UART_FIFO_SIZE;
  uart->line          = pdev->id;
  uart->dev           = &pdev->dev;

  uart->mapbase       = pdev->resource[0].start;
  uart->irq           = pdev->resource[1].start;

  uart->membase = ioremap(pdev->resource[0].start, resource_size(&pdev->resource[0]));
  if (!uart->membase)
    return -ENOMEM;

  // spin_lock_init(&uart->lock);

  return 0;
}

static void                   fill_buffer(unsigned long unused)
{
  u8                          buff;
  int                         i;
  struct circ_buff            *new;

  // spin_lock(&spin_buffer);

  i = 0;
  while (i++ < UART_FIFO_SIZE - 3)
  {
    buff = inb(SERIAL_PORT_BASE);
    printk(KERN_INFO "Read from FIFO : %c", buff);

    if ((new = kmalloc(GFP_ATOMIC, sizeof(struct circ_buff))) == NULL)
    {
      printk(KERN_ERR "Cannot allocate space.");
      return ;
    }

    new->c = buff;
    list_add_tail(&new->list, &buffer.list);
  }

  // spin_unlock(&spin_buffer);
}

/* FIXME */
DECLARE_TASKLET(tasklet, fill_buffer, 0);

static irqreturn_t            interrupt_handler(int irq, void *dev_id)
{
  printk(KERN_INFO "Interrupt ! irq = %x", irq);

  tasklet_schedule(&tasklet);

  return IRQ_HANDLED;
}

static void                   serial_driver_init_reg(void)
{
  u8                          mcr;

  /* Interrupts */
  outb(UART_IER_RDI, SERIAL_PORT_BASE + UART_IER);

  /* mcr */
  mcr = inb(SERIAL_PORT_BASE + UART_MCR);
  printk(KERN_INFO "Mcr = %x", mcr);
  outb(UART_MCR_LOOP, SERIAL_PORT_BASE + UART_MCR); // RTS/DTR = 0, LOOP = 1

  /* fcr */
  outb(UART_FCR_ENABLE_FIFO | UART_FCR_TRIGGER_8, SERIAL_PORT_BASE + UART_FCR);

}

static int __devinit          serial_driver_probe(struct platform_device *pdev)
{
  int                         ret = 0;
  struct my_uart_port         *port;

  printk(KERN_INFO "Driver : probe");

  port = &serial_ports[0];

  if ((ret = serial_driver_init_port(port, pdev)) < 0)
  {
    printk(KERN_ERR "Driver : serial_driver_init_port failed : returned %d", ret);

    return ret;
  }

  if ((ret = uart_add_one_port(&udriver, &port->uart)) < 0)
  {
    printk(KERN_ERR "Driver : uart_add_one_port failed : returned %d", ret);
    uart_unregister_driver(&udriver);

    return ret;
  }

  platform_set_drvdata(pdev, port);

  ret = request_irq(driver_resources[1].start, interrupt_handler, 0, "driver_uart", NULL);
  if (ret < 0)
  {
    printk(KERN_ERR "Error : irq");
    return ret;
  }

  serial_driver_init_reg();

  return 0;
}

static int __devexit          serial_driver_remove(struct platform_device *pdev)
{
  struct uart_port            *port = platform_get_drvdata(pdev);

  free_irq(driver_resources[1].start, NULL);
  platform_set_drvdata(pdev, NULL);
  uart_remove_one_port(&udriver, port);
  return 0;
}

static int                    serial_driver_suspend(struct platform_device *dev, pm_message_t state)
{
  return 0;
}

static int                    serial_driver_resume(struct platform_device *dev)
{
  return 0;
}

static struct platform_driver serial_driver = {
  .probe        = serial_driver_probe,
  .remove       = __devexit_p(serial_driver_remove),
  .suspend      = serial_driver_suspend,
  .resume       = serial_driver_resume,
  .driver       =
  {
    .name         = "driver_uart",
    .owner        = THIS_MODULE,
  },
};

/* Constructor */
static int __init             serial_init(void)
{
  int                         ret = 0;

  if ((ret = uart_register_driver(&udriver)) < 0)
  {
    printk(KERN_ERR "Driver : uart_register_driver failed : returned %d", ret);

    return ret;
  }

  if ((ret = platform_device_register(&pdev)) < 0)
  {
    printk(KERN_ERR "Driver : platform_device_register failed : returned %d", ret);
    uart_unregister_driver(&udriver);

    return ret;
  }

  if ((ret = platform_driver_register(&serial_driver)) < 0)
  {
    printk(KERN_ERR "Driver : plateform_driver_register : returned %d", ret);
    platform_device_unregister(&pdev);
    uart_unregister_driver(&udriver);

    return ret;
  }

  cdev_init(&char_cdev, &fops);

  if ((ret = cdev_add(&char_cdev, MKDEV(4, 64), 1)) < 0)
  {
    printk(KERN_ERR "Driver : cdev_add failed : returned %d", ret);

    return ret;
  }

  /* List init */
  // spin_lock_init(&spin_buffer);
  INIT_LIST_HEAD(&buffer.list);

  printk(KERN_INFO "Driver : serial driver registered");

  return 0;
}

/* Destructor */
static void __exit            serial_exit(void)
{
  cdev_del(&char_cdev);
  platform_driver_unregister(&serial_driver);
  platform_device_unregister(&pdev);
  uart_unregister_driver(&udriver);
  printk(KERN_INFO "Driver : serial driver unregistered");
}

module_init(serial_init);
module_exit(serial_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Greiner <greine_f@epitech.eu>");
MODULE_DESCRIPTION("Our First Serial Driver");
