#ifndef                       __PROTOCOLE_H__
# define                      __PROTOCOLE_H__

# define                      MOUNT     1
# define                      FILE      2
# define                      FOLDER    3
# define                      OPEN      4
# define                      ASK_READ  5
# define                      READ      6
# define                      WRITE     7
# define                      CLOSE     8
# define                      ERROR     9

struct                        s_proto
{
  int                         code;
  int                         len;
  int                         off;
  char                        args[255];
  char                        path[255]; // usefull for write
};

#endif                        /* __PROTOCOLE_H__ */
