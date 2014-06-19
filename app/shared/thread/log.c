
#include "mwsys.h"

#include "log.h"
/* Message lookup function. */
char *
lookup (struct message *mes, int key)
{
  struct message *pnt;

  for (pnt = mes; pnt->key != 0; pnt++) 
    if (pnt->key == key) 
      return pnt->str;

  return (char *)"";
}

