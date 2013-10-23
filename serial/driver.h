#ifndef               __DRIVER_H__
# define              __DRIVER_H__

# include             <linux/serial_reg.h>
# include             <linux/serial_core.h>

# define              SERIAL_PORT_BASE 0x03F8
# define              SERIAL_PORT_END 0x03FF

# define              UART_FIFO_SIZE 16

/* Lists */
struct                circ_buff
{
  char                c;
  struct list_head    list;
};

struct                my_uart_port
{
  struct uart_port    uart;
};

#endif                /* __DRIVER_H__ */
