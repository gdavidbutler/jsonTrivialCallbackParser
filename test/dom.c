/*
 * jsonTrivialCallbackParser - a JSON callback parser in C
 * Copyright (C) 2018 G. David Butler <gdb@dbSystems.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "json.h"

static void
jsonNodeVisit(
  jsonNode_t *n
 ,unsigned int d
 ,jsonNodeVisit_t v
 ,void *c
){
  (void)c;
  if (!d) /* skip "document" node */
    return;
  switch (v) {

  case jsonNodeVisitPreorder:
    if (n->name.s)
      printf("%c %u %u %.*s\n", n->type == jsonTp_Jb ? 'O' : 'A', d, n->parent->nodeW, n->name.l, n->name.s);
    else
      printf("%c %u %u [%u]\n", n->type == jsonTp_Jb ? 'O' : 'A', d, n->parent->nodeW, n->name.l);
    break;

  case jsonNodeVisitInorder:
    break;

  case jsonNodeVisitPostorder:
    break;

  case jsonNodeVisitLeaf:
    switch (n->type) {

    case jsonTp_Jb:
      if (n->name.s)
        printf("O %u %u %.*s\n", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("O %u %u [%u]\n", d, n->parent->nodeW, n->name.l);
      break;

    case jsonTp_Je:
      if (n->name.s)
        printf("A %u %u %.*s\n", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("A %u %u [%u]\n", d, n->parent->nodeW, n->name.l);
      break;

    case jsonTp_Js:
      if (n->name.s)
        printf("S %u %u %.*s", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("S %u %u [%u]", d, n->parent->nodeW, n->name.l);
      {
        unsigned char *s;
        int j;

        j = -1;
        if (!(s = malloc(n->value.l))
         || (j = jsonDecodeString(s, n->value.l, n->value.s, n->value.l)) < 0
         || j > (int)n->value.l)
          printf(" (%.*s)(%d|%u)", n->value.l, n->value.s, j, n->value.l);
        else
          printf(" %.*s", j, s);
        free(s);
      }
      putchar('\n');
      break;

    case jsonTp_Jn:
      if (n->name.s)
        printf("N %u %u %.*s", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("N %u %u [%u]", d, n->parent->nodeW, n->name.l);
      printf(" %.*s\n", n->value.l, n->value.s);
      break;

    case jsonTp_Jt:
      if (n->name.s)
        printf("T %u %u %.*s\n", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("T %u %u [%u]\n", d, n->parent->nodeW, n->name.l);
      break;

    case jsonTp_Jf:
      if (n->name.s)
        printf("F %u %u %.*s\n", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("F %u %u [%u]\n", d, n->parent->nodeW, n->name.l);
      break;

    case jsonTp_Ju:
      if (n->name.s)
        printf("N %u %u %.*s\n", d, n->parent->nodeW, n->name.l, n->name.s);
      else
        printf("N %u %u [%u]\n", d, n->parent->nodeW, n->name.l);
      break;
    }
    break;
  }
}

int
main(
  int argc
 ,char *argv[]
){
  jsonNode_t doc; /* "document" node */
  jsonSt_t tg[64];
  int fd;
  int sz;
  unsigned char *bf;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    return (1);
  }
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    fprintf(stderr, "%s: Can't open %s\n", argv[0], argv[1]);
    return (1);
  }
  sz = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  bf = malloc(sz);
  if (read(fd, bf, sz) != sz) {
    fprintf(stderr, "%s: read fail on %s\n", argv[0], argv[1]);
    return (1);
  }
  close(fd);
  memset(&doc, 0, sizeof(doc));
  printf("%d %d\n", sz, json2node(realloc, &doc, tg, bf, sizeof (tg) / sizeof (tg[0]), sz));
  jsonNodeWalk(&doc, jsonNodeVisit, 0);
  jsonNodeFree(free, &doc);
  free(bf);
  return (0);
}
