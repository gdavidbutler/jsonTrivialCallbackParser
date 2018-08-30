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
  case jsonTp_tb:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    if (vl)
      printf("{\n"/*}*/);
    else
      printf("[\n"/*]*/);
    *V = 0;
    break;
  case jsonTp_te:
    if (l) {
      for (i = 0; i < (int)l; ++i)
        putchar(' ');
#if 0
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    if (vl)
      printf(/*{*/"}\n");
    else
      printf(/*[*/"]\n");
    *V = 1;
    break;
  case jsonTp_ts:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    printf("\"%.*s\"\n", vl->l, vl->s);
    *V = 1;
    break;
  case jsonTp_tn:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    printf("%.*s\n", vl->l, vl->s);
    *V = 1;
    break;
  case jsonTp_tt:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    printf("true\n");
    *V = 1;
    break;
  case jsonTp_tf:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    printf("false\n");
    *V = 1;
    break;
  case jsonTp_tu:
    if (l) {
      for (i = *V ? 1 : 0; i < (int)l; ++i)
        putchar(' ');
      if (*V)
        putchar(',');
      if ((tg + l - 1)->s)
        printf("\"%.*s\":", (tg + l - 1)->l, (tg + l - 1)->s);
#if 0
      else
        printf("%d:", (tg + l - 1)->l);
#endif
    }
    printf("null\n");
    *V = 1;
    break;
  case jsonTp_tr:
    putchar('!');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/%d", (tg + i)->l);
    } else
      putchar('/');
    printf(":%.*s\n", vl->l, vl->s);
    break;
  }
  return 0;
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
    return -1;
  sz = 0;
  while ((i = fread(bf + sz, 1, BUFSIZ, stdin)) > 0) {
    sz += i;
    if (!(bf = realloc(bf, sz + BUFSIZ)))
      return -1;
  }
  *(bf + sz) = '\0';
  i = 0;
  jsonParse(cb, sizeof(tg) / sizeof(tg[0]), tg, bf, &i);
  return 0;
}
