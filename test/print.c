#include <stdio.h>
#include <stdlib.h>
#include "json.h"

static int
cb(
  jsonTp_t typ
 ,unsigned int l
 ,const jsonSt_t *tg
 ,const jsonSt_t *vl
 ,void *v
#define V ((int *)v)
){
  int i;

  (void)v;
  switch (typ) {
  case jsonTp_Jb:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    if (vl->l)
      printf("{\n"/*}*/);
    else
      printf("[\n"/*]*/);
    *V = 0;
    break;
  case jsonTp_Je:
    if (l) {
      for (i = 0; i < (int)l; ++i)
        putchar(' ');
#if 0
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    if (vl->l)
      printf(/*{*/"}\n");
    else
      printf(/*[*/"]\n");
    *V = 1;
    break;
  case jsonTp_Js:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = jsonDecodeString(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("\"%.*s\"\n", vl->l, vl->s);
      else
        printf("\"%.*s\"\n", i, d);
      free(d);
    }
    *V = 1;
    break;
  case jsonTp_Jn:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    printf("%.*s\n", vl->l, vl->s);
    *V = 1;
    break;
  case jsonTp_Jt:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    printf("true\n");
    *V = 1;
    break;
  case jsonTp_Jf:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    printf("false\n");
    *V = 1;
    break;
  case jsonTp_Ju:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("[%d]:", (tg + l - 1)->l);
#endif
    }
    printf("null\n");
    *V = 1;
    break;
  }
  return (0);
}
#undef V

int
main(
){
  unsigned char *bf;
  jsonSt_t tg[64];
  int sz;
  int i;

  if (!(bf = malloc(BUFSIZ)))
    return (-1);
  sz = 0;
  while ((i = fread(bf + sz, 1, BUFSIZ, stdin)) > 0) {
    sz += i;
    if (!(bf = realloc(bf, sz + BUFSIZ)))
      return (-1);
  }
  i = 0;
  jsonParse(cb, sizeof (tg) / sizeof (tg[0]), tg, bf, sz, &i);
  return (0);
}
