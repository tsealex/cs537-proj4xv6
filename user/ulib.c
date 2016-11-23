#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "param.h"

char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int
thread_create(void(*start_routine)(void*), void *arg)
{
  // allocate a page-aligned memory block
  char* stack;
  char* ptr = malloc(PGSIZE * 2);
  if (ptr == NULL)
    return -1;
  // case 1: the allocated pointer is 0x26A000 (ends with 0x000)
  if (((uint)ptr & ~0xFFF) == (uint)ptr)
    stack = ptr;
  else
    // case 2: stack is let say 0x26A004 (not end with 0x000) => 0x26B000 (new pointer)
    stack = (void*)(((uint)ptr + PGSIZE) & ~0xFFF);
  // create a tail: [...wasted...][...stack...][*tail=ptr]
  uint* tail = (uint*)&stack[PGSIZE];
  *tail = (uint)ptr;
  return clone(start_routine, arg, stack);
}

int
thread_join()
{
  int tid;
  char* stack;
  if ((tid = join((void**)&stack)) != -1) {
    // free the stack but first we need to obtain the original ptr
    uint* tail = (uint*)&stack[PGSIZE];
    free((void*)*tail);
  }
  return tid;
}