#ifndef                       __PROTOCOLE_H__
# define                      __PROTOCOLE_H__

# define                      P_MOUNT     1
# define                      P_FILE      2
# define                      P_FOLDER    3
# define                      P_OPEN      4
# define                      P_ASK_READ  5
# define                      P_READ      6
# define                      P_WRITE     7
# define                      P_CLOSE     8
# define                      P_ERROR     9

struct                        s_proto
{
  int                         code;
  int                         len;
  int                         off;
  int                         size;
  char                        args[255];
  char                        path[255]; // usefull for write
};

#endif                        /* __PROTOCOLE_H__ */
