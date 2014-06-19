
#include "mwsys.h"

#include "log.h"
#include "memory.h"

void alloc_inc (int);
void alloc_dec (int);
struct message mstr [] =
{
  { MTYPE_THREAD, (char *)"thread" },
  { MTYPE_THREAD_MASTER, (char *)"thread_master" },
  { 0, NULL },
};

/* Fatal memory allocation error occured. */
static void
zerror (const char *fname, int type, size_t size)
{
  fprintf (stderr, "%s : can't allocate memory for `%s' size %d\n", 
	   fname, lookup (mstr, type), (int) size);
  exit (1);
}

/* Memory allocation. */
void *
zmalloc (int type, size_t size)
{
  void *memory;

  memory = malloc (size);

  if (memory == NULL)
    zerror ("malloc", type, size);

  alloc_inc (type);

  return memory;
}

/* Memory allocation with num * size with cleared. */
void *
zcalloc (int type, size_t size)
{
  void *memory;

  memory = calloc (1, size);

  if (memory == NULL)
    zerror ("calloc", type, size);

  alloc_inc (type);

  return memory;
}

/* Memory reallocation. */
void *
zrealloc (int type, void *ptr, size_t size)
{
  void *memory;

  memory = realloc (ptr, size);
  if (memory == NULL)
    zerror ("realloc", type, size);
  return memory;
}

/* Memory free. */
void
zfree (int type, void *ptr)
{
  alloc_dec (type);
  free (ptr);
}

/* String duplication. */
char *
zstrdup (int type, char *str)
{
  void *dup;

  dup = strdup (str);
  if (dup == NULL)
    zerror ("strdup", type, strlen (str));
  alloc_inc (type);
  return (char *)dup;
}

#ifdef MEMORY_LOG
struct 
{
  char *name;
  unsigned long alloc;
  unsigned long t_malloc;
  unsigned long c_malloc;
  unsigned long t_calloc;
  unsigned long c_calloc;
  unsigned long t_realloc;
  unsigned long t_free;
  unsigned long c_strdup;
} mstat [MTYPE_MAX];

void
mtype_log (char *func, void *memory, const char *file, int line, int type)
{
  syslog(LOG_DEBUG,"%s: %s %p %s %d", func, lookup (mstr, type), memory, file, line);
}

void *
mtype_zmalloc (const char *file, int line, int type, size_t size)
{
  void *memory;

  mstat[type].c_malloc++;
  mstat[type].t_malloc++;

  memory = zmalloc (type, size);
  mtype_log ("zmalloc", memory, file, line, type);

  return memory;
}

void *
mtype_zcalloc (const char *file, int line, int type, size_t size)
{
  void *memory;

  mstat[type].c_calloc++;
  mstat[type].t_calloc++;

  memory = zcalloc (type, size);
  mtype_log ("xcalloc", memory, file, line, type);

  return memory;
}

void *
mtype_zrealloc (const char *file, int line, int type, void *ptr, size_t size)
{
  void *memory;

  /* Realloc need before allocated pointer. */
  mstat[type].t_realloc++;

  memory = zrealloc (type, ptr, size);

  mtype_log ("xrealloc", memory, file, line, type);

  return memory;
}

/* Important function. */
void 
mtype_zfree (const char *file, int line, int type, void *ptr)
{
  mstat[type].t_free++;

  mtype_log ("xfree", ptr, file, line, type);

  zfree (type, ptr);
}

char *
mtype_zstrdup (const char *file, int line, int type, char *str)
{
  char *memory;

  mstat[type].c_strdup++;

  memory = zstrdup (type, str);
  
  mtype_log ("xstrdup", memory, file, line, type);

  return memory;
}
#else
struct tag_mstat
{
  char *name;
  unsigned long alloc;
} mstat [MTYPE_MAX];
#endif /* MTPYE_LOG */

/* Increment allocation counter. */
void
alloc_inc (int type)
{
  mstat[type].alloc++;
}

/* Decrement allocation counter. */
void
alloc_dec (int type)
{
  mstat[type].alloc--;
}
