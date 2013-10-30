#include <linux/slab.h>

void    *xkmalloc(size_t size, int flags)
{
  void  *res = NULL;

  if ((res = kmalloc(size, flags)) == NULL)
    printk(KERN_ERR "No more space available");

  return res;
}

void    free_tab(char **tab)
{
  int   i;

  i = 0;
  while (tab[i] != NULL)
    {
      kfree(tab[i]);
      i++;
    }
  kfree(tab);
}

int   my_tablen(char **tab)
{
  int   i;

  i = 0;
  while (tab[i] != NULL)
    i++;
  return (i);
}

int   count_word(char *str, char sep)
{
  int   i;
  int   nb_word;

  i = 0;
  nb_word = 1;
  while (str[i] != '\0')
    {
      if (str[i] == sep && (i > 0 && str[i - 1] != sep))
  {
    while (str[i] != '\0' && str[i] == sep)
      i++;
    if (str[i] != '\0')
      nb_word++;
  }
      else
  i++;
    }
  return (nb_word);
}

char    *get_word(char *str, int tmp, int len)
{
  int   i;
  char    *res;

  i = 0;
  res = xkmalloc((len + 1) * sizeof(*res), GFP_KERNEL);
  while (i < len)
    res[i++] = str[tmp++];
  res[i] = '\0';
  return (res);
}

char    **str_to_wordtab(char *str, char sep)
{
  char    **res;
  int   lines;
  int   len;
  int   tmp;
  int   i;

  res = xkmalloc((count_word(str, sep) + 1) * sizeof(*res), GFP_KERNEL);
  lines = 0;
  i = 0;
  while (str[i] != '\0')
    {
      if (str[i] != sep)
  {
    tmp = i;
    len = 0;
    while (str[i] != '\0' && str[i++] != sep)
      len++;
    res[lines] = get_word(str, tmp, len);
    lines++;
  }
      else
  i++;
    }
  res[lines] = NULL;
  return (res);
}
