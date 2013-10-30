#ifndef                       __PROTOCOLE_H__
# define                      __PROTOCOLE_H__

# define  MOUNT   1
# define  FILE    2
# define  FOLDER  3

struct s_proto
{
  int     code;
  char    args[255];
};

#endif                        /* __PROTOCOLE_H__ */
