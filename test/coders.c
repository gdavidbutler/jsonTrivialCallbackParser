/*
 * jsonTrivialCallbackParser - a XML callback parser in C
 * Copyright (C) 2018-2023 G. David Butler <gdb@dbSystems.com>
 *
 * This file is part of jsonTrivialCallbackParser
 *
 * jsonTrivialCallbackParser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jsonTrivialCallbackParser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "json.h"

int
main(
  void
){
  static const unsigned char enc[] = "This is a test of \"\\\b\t\f\n\r\" how did it do?";
  static const unsigned char dnc[] = "This is a test of\\n(c) white on black \\u00a9\\n(c) black on white \\uD83C\\uDD52\\nhow did it do?";
  static const char b64[] = "KFVybCAgZG8/KQ==";
  static const char u64[] = "KFVybCAgZG8_KQ";
  static const unsigned char s64[] = "(Url  do?)";
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
  if ((sz = jsonDecodeBase64Url((unsigned char *)bf, BUFSIZ, u64, sizeof (u64) - 1)) > BUFSIZ)
    return (2);
  printf("jsonDecodeBase64Url(%s)\n->\n%.*s\n", u64, sz, bf);
  putchar('\n');
  if ((sz = jsonEncodeBase64Url((char *)bf, BUFSIZ, s64, sizeof (s64) - 1)) > BUFSIZ)
    return (2);
  printf("jsonEncodeBase64Url(%s)\n->\n%.*s\n", s64, sz, bf);
  putchar('\n');
  free(bf);
  return (0);
}
