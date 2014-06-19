/* Memory management routine
   Copyright (C) 1998 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _ZEBRA_MEMORY_H
#define _ZEBRA_MEMORY_H
#ifdef  __cplusplus
extern "C"{
#endif
/* #define MEMORY_LOG */

/* For tagging memory, below is the type of the memory. */
enum
{
  MTYPE_TMP = 1,
  MTYPE_THREAD,
  MTYPE_THREAD_MASTER,
  MTYPE_MAX
};

#ifdef MEMORY_LOG
#define XMALLOC(mtype, size) \
  mtype_zmalloc (__FILE__, __LINE__, (mtype), (size))
#define XCALLOC(mtype, size) \
  mtype_zcalloc (__FILE__, __LINE__, (mtype), (size))
#define XREALLOC(mtype, ptr, size)  \
  mtype_zrealloc (__FILE__, __LINE__, (mtype), (ptr), (size))
#define XFREE(mtype, ptr) \
  mtype_zfree (__FILE__, __LINE__, (mtype), (ptr))
#define XSTRDUP(mtype, str) \
  mtype_zstrdup (__FILE__, __LINE__, (mtype), (str))
#else
#define XMALLOC(mtype, size)       zmalloc ((mtype), (size))
#define XCALLOC(mtype, size)       zcalloc ((mtype), (size))
#define XREALLOC(mtype, ptr, size) zrealloc ((mtype), (ptr), (size))
#define XFREE(mtype, ptr)          zfree ((mtype), (ptr))
#define XSTRDUP(mtype, str)        zstrdup ((mtype), (str))
#endif /* MEMORY_LOG */

/* Prototypes of memory function. */
void *zmalloc (int type, size_t size);
void *zcalloc (int type, size_t size);
void *zrealloc (int type, void *ptr, size_t size);
void  zfree (int type, void *ptr);
char *zstrdup (int type, char *str);

void *mtype_zmalloc (const char *file,
		     int line,
		     int type,
		     size_t size);

void *mtype_zcalloc (const char *file,
		     int line,
		     int type,
		     size_t num,
		     size_t size);

void *mtype_zrealloc (const char *file,
		     int line,
		     int type, 
		     void *ptr,
		     size_t size);

void mtype_zfree (const char *file,
		  int line,
		  int type,
		  void *ptr);

char *mtype_zstrdup (const char *file,
		     int line,
		     int type,
		     char *str);
void memory_init ();
#ifdef  __cplusplus
}
#endif
#endif /* _ZEBRA_MEMORY_H */
