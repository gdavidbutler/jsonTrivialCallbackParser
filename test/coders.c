#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "json.h"

int
main(
){
  static const unsigned char enc[] = "This is a test of \"\\\b\t\f\n\r\" how did it do?";
  static const unsigned char dnc[] = "This is a test of\\n(c) white on black \\u00a9\\n(c) black on white \\uD83C\\uDD52\\nhow did it do?";
  static const char b64[] = "QmFzZTY0";
  static const unsigned char s64[] = "Base64";
  int sz;
  unsigned char *bf;
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
