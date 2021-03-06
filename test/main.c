#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "json.h"

static int
cb(
  jsonTp_t typ
 ,unsigned int l
 ,const jsonSt_t *tg
 ,const jsonSt_t *vl
 ,void *v
){
  int i;

  (void)v;
  switch (typ) {
  case jsonTp_Jb:
    putchar('B');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    if (vl->l)
      printf("{ @%p %c\n"/*}*/, (void *)vl->s, *vl->s);
    else
      printf("[ @%p %c\n"/*]*/, (void *)vl->s, *vl->s);
    break;
  case jsonTp_Je:
    putchar('E');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    if (vl->l)
      printf(/*{*/"} @%p %c\n", (void *)vl->s, *vl->s);
    else
      printf(/*[*/"] @%p %c\n", (void *)vl->s, *vl->s);
    break;
  case jsonTp_Js:
    putchar('S');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = jsonDecodeString(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf(": \"%.*s\"(%d:%u)\n", vl->l, vl->s, i, vl->l);
      else
        printf(": \"%.*s\"(%.*s)\n", vl->l, vl->s, i, d);
      free(d);
    }
    break;
  case jsonTp_Jn:
    putchar('N');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    printf(": %.*s\n", vl->l, vl->s);
    break;
  case jsonTp_Jt:
    putchar('T');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    putchar('\n');
    break;
  case jsonTp_Jf:
    putchar('F');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    putchar('\n');
    break;
  case jsonTp_Ju:
    putchar('U');
    putchar(' ');
    if (l) {
      for (i = 0; i < (int)l; ++i)
        if ((tg + i)->s)
          printf("/\"%.*s\"", (tg + i)->l, (tg + i)->s);
        else
          printf("/[%d]", (tg + i)->l);
    } else
      putchar('/');
    putchar('\n');
    break;
  }
  return (0);
}

int
main(
  int argc
 ,char *argv[]
){
  jsonSt_t tg[64];
  int fd;
  int sz;
  unsigned char *bf;

  if (argc == 2) {
    static const unsigned char enc[] = "This is a test of \"\\\b\t\f\n\r\" how did it do?";
    static const unsigned char dnc[] = "This is a test of\\n(c) white on black \\u00a9\\n(c) black on white \\uD83C\\uDD52\\nhow did it do?";
    static const char b64[] = "QmFzZTY0";
    static const unsigned char s64[] = "Base64";
    unsigned char *tbf;

    if (!(bf = malloc(BUFSIZ)))
      return (2);
    if ((sz = jsonEncodeString(bf, BUFSIZ, enc, sizeof (enc) - 1)) > BUFSIZ)
      return (2);
    printf("jsonEncodeString()->%.*s\n", sz, bf);
    putchar('\n');
    if (!(tbf = malloc(BUFSIZ)))
      return (2);
    if ((sz = jsonDecodeString(tbf, BUFSIZ, bf, sz)) > BUFSIZ)
      return (2);
    if (sz != sizeof (enc) - 1 || memcmp(tbf, enc, sizeof (enc) - 1))
      return (2);
    free(tbf);
    if ((sz = jsonDecodeString(bf, BUFSIZ, dnc, sizeof (dnc) - 1)) > BUFSIZ)
      return (2);
    printf("jsonDecodeString(%s)->%.*s\n", dnc, sz, bf);
    putchar('\n');
    if ((sz = jsonDecodeBase64((unsigned char *)bf, BUFSIZ, b64, sizeof (b64) - 1)) > BUFSIZ)
      return (2);
    printf("jsonDecodeBase64(%s)\n->\n%.*s\n", b64, sz, bf);
    putchar('\n');
    if ((sz = jsonEncodeBase64((char *)bf, BUFSIZ, s64, sizeof (s64) - 1)) > BUFSIZ)
      return (2);
    printf("jsonEncodeBase64(%s)\n->\n%.*s\n", s64, sz, bf);
    putchar('\n');
    free(bf);
    return (0);
  }
  if (argc != 3) {
    fprintf(stderr, "Usage: %s any | 0|1 file\n", argv[0]);
    return (1);
  }
  if ((fd = open(argv[2], O_RDONLY)) < 0) {
    fprintf(stderr, "%s: Can't open %s\n", argv[0], argv[2]);
    return (1);
  }
  sz = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  bf = malloc(sz);
  if (read(fd, bf, sz) != sz) {
    fprintf(stderr, "%s: read fail on %s\n", argv[0], argv[2]);
    return (1);
  }
  close(fd);
  printf("%d %d\n", sz, jsonParse(atoi(argv[1]) ? cb : 0, sizeof (tg) / sizeof (tg[0]), tg, bf, sz, 0));
  free(bf);
  return (0);
}
