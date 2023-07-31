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

#include "json.h"

int
jsonParse(
  jsonCb_t c
 ,unsigned int m
 ,jsonSt_t *t
 ,const unsigned char *s
 ,unsigned int l
,void *v
){
  const unsigned char *sb;
  unsigned int tL; /* current level */
  jsonSt_t vl;

  if (!(sb = s))
    return (-1);
  tL = 0;

  goto bgn;

err:
  l++, s--;
  goto rtn;

sep:
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case ':':
    if (!tL)
      goto err;
    (t + tL - 1)->s = vl.s;
    (t + tL - 1)->l = vl.l;
    goto bgn;

  default:
    if (c && c(jsonTp_Js, tL, t, &vl, v))
      goto rtn;
    l++, --s;
    goto bgn;
  }
  if (c)
    c(jsonTp_Js, tL, t, &vl, v);
  goto rtn;

str:
  vl.s = s;
  for (; l--;) switch (*s++) {
  case '"':
    vl.l = s - vl.s - 1;
    goto sep;

  case '\\':
    if (l > 0
     && (*(s + 0) == '"'
     || *(s + 0) == '\\'))
      l--, s++;
    break;

  default:
    break;
  }
  goto rtn;

nbr:
  vl.s = s - 1;
  for (; l--;) switch (*s++) {
  case '+': case '-': case '.':
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
  case 'E': case 'e':
    break;

  default:
    vl.l = s - vl.s - 1;
    if (c && c(jsonTp_Jn, tL, t, &vl, v))
      goto rtn;
    l++, --s;
    goto bgn;
  }
  if (c)
    c(jsonTp_Jn, tL, t, &vl, v);
  goto rtn;

bgn:
  if (tL && !(t + tL - 1)->s)
    ++(t + tL - 1)->l;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case ',': /* interesting, comma can be treated as whitespace */
    break;

  case '[':/*']'*/
    if (tL == m)
      goto rtn;
    vl.s = s - 1, vl.l = 0;
    if (c && c(jsonTp_Jb, tL, t, &vl, v))
      goto rtn;
    (t + tL)->s = 0;
    (t + tL)->l = 0;
    ++tL;
    break;

  /*'['*/case ']':
    if (!tL)
      goto err;
    --tL;
    vl.s = s - 1, vl.l = 0;
    if (c && c(jsonTp_Je, tL, t, &vl, v))
      goto rtn;
    goto bgn;

  case '{':/*'}'*/
    if (tL == m)
      goto rtn;
    vl.s = s - 1, vl.l = 1;
    if (c && c(jsonTp_Jb, tL, t, &vl, v))
      goto rtn;
    (t + tL)->s = s;
    (t + tL)->l = 0;
    ++tL;
    goto bgn;

  /*'{'*/case '}':
    if (!tL)
      goto err;
    --tL;
    vl.s = s - 1, vl.l = 1;
    if (c && c(jsonTp_Je, tL, t, &vl, v))
      goto rtn;
    goto bgn;

  case '"':
    goto str;

  case '-':
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
    goto nbr;

  case 'f':
    if (l > 3
     && *(s + 0) == 'a'
     && *(s + 1) == 'l'
     && *(s + 2) == 's'
     && *(s + 3) == 'e') {
      if (c && c(jsonTp_Jf, tL, t, 0, v))
        goto rtn;
      l -= 4, s += 4;
      goto bgn;
    } else
      goto err;

  case 't':
    if (l > 2
     && *(s + 0) == 'r'
     && *(s + 1) == 'u'
     && *(s + 2) == 'e') {
      if (c && c(jsonTp_Jt, tL, t, 0, v))
        goto rtn;
      l -= 3, s += 3;
      goto bgn;
    } else
      goto err;

  case 'n':
    if (l > 2
     && *(s + 0) == 'u'
     && *(s + 1) == 'l'
     && *(s + 2) == 'l') {
      if (c && c(jsonTp_Ju, tL, t, 0, v))
        goto rtn;
      l -= 3, s += 3;
      goto bgn;
    } else
      goto err;

  default:
    goto err;
  }

rtn:
  return (s - sb);
}

int
jsonDecodeString(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  int len;

  len = 0;
  for (; ilen--;) switch (*in) {
  case '\\':
    if (!(in++,ilen--)) goto err; else switch (*in) {
    case 'b':
      if (olen > 0) {
        *out++ = '\b';
        olen--;
      }
      in++;
      len++;
      break;
    case 't':
      if (olen > 0) {
        *out++ = '\t';
        olen--;
      }
      in++;
      len++;
      break;
    case 'n':
      if (olen > 0) {
        *out++ = '\n';
        olen--;
      }
      in++;
      len++;
      break;
    case 'f':
      if (olen > 0) {
        *out++ = '\f';
        olen--;
      }
      in++;
      len++;
      break;
    case 'r':
      if (olen > 0) {
        *out++ = '\r';
        olen--;
      }
      in++;
      len++;
      break;
    case 'u': {
      unsigned long s;
      unsigned long c;
      unsigned int i;

      s = 0;
sur:
      c = 0;
      for (i = 4; i; --i) if (!(in++,ilen--)) goto err; else switch (*in) {
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        c *= 16;
        c += *in - '0';
        break;
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        c *= 16;
        c += 10 + (*in - 'A');
        break;
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        c *= 16;
        c += 10 + (*in - 'a');
        break;
      default:
        goto err;
      }
      in++;
      if (c > 0xd7ff && c < 0xdc00) {
        if (!s
         && ilen
         && *(in + 0) == '\\'
         && *(in + 1) == 'u') {
          s = c;
          ++in;
          --ilen;
          goto sur;
        } else
          goto err;
      }
      if (c > 0xdbff && c < 0xe000) {
        if (!s)
          goto err;
        s -= 0xd800;
        s *= 0x400;
        c -= 0xdc00;
        c += s + 0x10000;
      }
      /* https://en.wikipedia.org/wiki/UTF-8 */
      if (c <= 0x7f) { /* 7 bits */
        if (olen > 0) {
          *out++ = c;
          olen--;
        }
        len++;
      } else if (c <= 0x7ff) { /* 11 bits */
        if (olen > 1) {
          *out++ = 0xc0 | (c >> 6);
          *out++ = 0x80 | (c & 0x3f);
          olen -= 2;
        }
        len += 2;
      } else if (c <= 0xffff) { /* 16 bits */
        if (olen > 2) {
          *out++ = 0xe0 | (c >> 12);
          *out++ = 0x80 | (c >> 6 & 0x3f);
          *out++ = 0x80 | (c & 0x3f);
          olen -= 3;
        }
        len += 3;
      } else if (c <= 0x1fffff) { /* 21 bits */
        if (olen > 3) {
          *out++ = 0xf0 | (c >> 18);
          *out++ = 0x80 | (c >> 12 & 0x3f);
          *out++ = 0x80 | (c >> 6 & 0x3f);
          *out++ = 0x80 | (c & 0x3f);
          olen -= 4;
        }
        len += 4;
      } else if (c <= 0x3ffffff) { /* 26 bits */
        if (olen > 4) {
          *out++ = 0xf8 | (c >> 24);
          *out++ = 0x80 | (c >> 18 & 0x3f);
          *out++ = 0x80 | (c >> 12 & 0x3f);
          *out++ = 0x80 | (c >> 6 & 0x3f);
          *out++ = 0x80 | (c & 0x3f);
          olen -= 5;
        }
        len += 5;
      } else if (c <= 0x7fffffff) { /* 31 bits */
        if (olen > 5) {
          *out++ = 0xfc | (c >> 30);
          *out++ = 0x80 | (c >> 24 & 0x3f);
          *out++ = 0x80 | (c >> 18 & 0x3f);
          *out++ = 0x80 | (c >> 12 & 0x3f);
          *out++ = 0x80 | (c >> 6 & 0x3f);
          *out++ = 0x80 | (c & 0x3f);
          olen -= 6;
        }
        len += 6;
      } else
        goto err;
      } break;
    default:
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
      break;
    }
    break;
  default:
    /* https://en.wikipedia.org/wiki/UTF-8 */
    if ((*in & 0x80) == 0x00) { /* 1 byte */
      if (olen > 0) {
        *out++ = *in++;
        olen--;
      } else
        in++;
      len++;
    } else if (ilen > 0
            && (*in & 0xe0) == 0xc0 /* 2 bytes */
            && (*(in + 1) & 0xc0) == 0x80) {
      if (olen > 1) {
        *out++ = *in++;
        *out++ = *in++;
        olen -= 2;
      } else
        in += 2;
      len += 2;
      ilen--;
    } else if (ilen > 1
            && (*in & 0xf0) == 0xe0 /* 3 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80) {
      if (olen > 2) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 3;
      } else
        in += 3;
      len += 3;
      ilen -= 2;
    } else if (ilen > 2
            && (*in & 0xf8) == 0xf0 /* 4 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80) {
      if (olen > 3) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 4;
      } else
        in += 4;
      len += 4;
      ilen -= 3;
    } else if (ilen > 3
            && (*in & 0xfc) == 0xf8 /* 5 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80) {
      if (olen > 4) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 5;
      } else
        in += 5;
      len += 5;
      ilen -= 4;
    } else if (ilen > 4
            && (*in & 0xfe) == 0xfc /* 6 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80
            && (*(in + 5) & 0xc0) == 0x80) {
      if (olen > 5) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 6;
      } else
        in += 6;
      len += 6;
      ilen -= 5;
    } else
      goto err;
    break;
  }
  return (len);
err:
  return (-1);
}

int
jsonEncodeString(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  static const char hex[] = "0123456789ABCDEF";
  int len;

  len = 0;
  for (; ilen--;) switch (*in) {
  case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                                   case 0x0b:                       case 0x0e: case 0x0f:
  case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
  case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
    if (olen > 5) {
      *out++ = '\\';
      *out++ = 'u';
      *out++ = '0';
      *out++ = '0';
      *out++ = hex[*in >> 4];
      *out++ = hex[*in & 0x0f];
      olen -= 6;
    }
    in++;
    len += 6;
    break;
  case '\b': /* 0x08 */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = 'b';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '\t': /* 0x09 */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = 't';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '\n': /* 0x0a */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = 'n';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '\f': /* 0x0c */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = 'f';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '\r': /* 0x0d */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = 'r';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '"': /* 0x22 */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = '"';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '/': /* 0x2f */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = '/';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  case '\\': /* 0x5c */
    if (olen > 1) {
      *out++ = '\\';
      *out++ = '\\';
      olen -= 2;
    }
    in++;
    len += 2;
    break;
  default:
    /* https://en.wikipedia.org/wiki/UTF-8 */
    if ((*in & 0x80) == 0x00) { /* 1 byte */
      if (olen > 0) {
        *out++ = *in++;
        olen--;
      } else
        in++;
      len++;
    } else if (ilen > 0
            && (*in & 0xe0) == 0xc0 /* 2 bytes */
            && (*(in + 1) & 0xc0) == 0x80) {
      if (olen > 1) {
        *out++ = *in++;
        *out++ = *in++;
        olen -= 2;
      } else
        in += 2;
      len += 2;
      ilen--;
    } else if (ilen > 1
            && (*in & 0xf0) == 0xe0 /* 3 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80) {
      if (olen > 2) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 3;
      } else
        in += 3;
      len += 3;
      ilen -= 2;
    } else if (ilen > 2
            && (*in & 0xf8) == 0xf0 /* 4 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80) {
      if (olen > 3) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 4;
      } else
        in += 4;
      len += 4;
      ilen -= 3;
    } else if (ilen > 3
            && (*in & 0xfc) == 0xf8 /* 5 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80) {
      if (olen > 4) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 5;
      } else
        in += 5;
      len += 5;
      ilen -= 4;
    } else if (ilen > 4
            && (*in & 0xfe) == 0xfc /* 6 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80
            && (*(in + 5) & 0xc0) == 0x80) {
      if (olen > 5) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 6;
      } else
        in += 6;
      len += 6;
      ilen -= 5;
    } else
      return (-1);
    break;
  }
  return (len);
}

int
jsonDecodeBase64(
  unsigned char *out
 ,unsigned int olen
 ,char const *in
 ,unsigned int ilen
){
  static unsigned char const b64[] = {
    66, 66, 66, 66,  66, 66, 66, 66,  66, 64, 64, 66,  66, 64, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    64, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 62,  66, 66, 66, 63,
    52, 53, 54, 55,  56, 57, 58, 59,  60, 61, 66, 66,  66, 65, 66, 66,
    66,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
    15, 16, 17, 18,  19, 20, 21, 22,  23, 24, 25, 66,  66, 66, 66, 66,
    66, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
    41, 42, 43, 44,  45, 46, 47, 48,  49, 50, 51, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66
  };
  unsigned long buf;
  int len;

  buf = 1;
  len = 0;
  while (ilen-- > 0) {
    unsigned char c;

    switch ((c = b64[*(unsigned char*)in++])) {
    case 66: /* invalid */
      return (-1);
    case 64: /* whitespace */
      continue;
    case 65: /* pad */
      ilen = 0;
      break;
    default:
      buf = buf << 6 | c;
      if (buf & 0x1000000) {
        if (olen >= 3) {
          *out++ = buf >> 16;
          *out++ = buf >> 8;
          *out++ = buf;
          olen -= 3;
        }
        len += 3;
        buf = 1;
      }
      break;
    }
  }
  if (buf & 0x40000) {
    if (olen >= 2) {
      *out++ = buf >> 10;
      *out++ = buf >> 2;
    }
    len += 2;
  } else if (buf & 0x1000) {
    if (olen >= 1)
      *out++ = buf >> 4;
    len++;
  }
  return (len);
}

int
jsonEncodeBase64(
  char *out
 ,unsigned int olen
 ,unsigned char const *in
 ,unsigned int ilen
){
  static const char b64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int len;

  for (len = 0; ilen >= 3; in += 3, ilen -= 3, len += 4) {
    if (olen >= 4) {
      *out++ = b64[in[0] >> 2];
      *out++ = b64[((in[0] << 4) & 0x30) | (in[1] >> 4)];
      *out++ = b64[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
      *out++ = b64[in[2] & 0x3f];
      olen -= 4;
    }
  }
  if (ilen) {
    if (olen >= 4) {
      unsigned char frag;

      *out++ = b64[in[0] >> 2];
      frag = (in[0] << 4) & 0x30;
      if (ilen > 1)
          frag |= in[1] >> 4;
      *out++ = b64[frag];
      *out++ = (ilen > 1) ? b64[(in[1] << 2) & 0x3c] : '=';
      *out++ = '=';
    }
    len += 4;
  }
  return (len);
}

/* jsonNode routines */

struct jsonNodeCx {
  void *(*a)(void *, unsigned long);
  jsonNode_t *n;
};

static int
jsonNodeCb(
  jsonTp_t t
 ,unsigned int l
 ,const jsonSt_t *g
 ,const jsonSt_t *v
 ,void *x
#define X ((struct jsonNodeCx *)x)
){
  jsonNode_t *tn;
  void *tv;

  switch (t) {

  case jsonTp_Jb:
    if (!(tv = X->a(X->n->node, (X->n->nodeN + 1) * sizeof (*X->n->node))))
      goto exit;
    X->n->node = tv;
    if (!(tn = X->a(0, sizeof (*tn))))
      goto exit;
    *(X->n->node + X->n->nodeN++) = tn;
    tn->parent = X->n;
    tn->node = 0;
    tn->beg = v->s;
    tn->user = 0;
    tn->name.s = l ? (g + (l - 1))->s : 0;
    tn->name.l = l ? (g + (l - 1))->l : 0;
    tn->value.s = 0;
    tn->value.l = 0;
    tn->type = v->l ? jsonTp_Jb : jsonTp_Je;
    tn->nodeN = 0;
    tn->nodeW = 0;
    X->n = tn;
    break;

  case jsonTp_Je:
    X->n->end = v->s;
    X->n = X->n->parent;
    break;

  default:
    if (!(tv = X->a(X->n->node, (X->n->nodeN + 1) * sizeof (*X->n->node))))
      goto exit;
    X->n->node = tv;
    if (!(tn = X->a(0, sizeof (*tn))))
      goto exit;
    *(X->n->node + X->n->nodeN++) = tn;
    tn->parent = X->n;
    tn->node = 0;
    tn->user = 0;
    tn->name.s = l ? (g + (l - 1))->s : 0;
    tn->name.l = l ? (g + (l - 1))->l : 0;
    tn->value.s = v ? v->s : 0;
    tn->value.l = v ? v->l : 0;
    tn->type = t;
    tn->nodeN = 0;
    tn->nodeW = 0;
    break;
  }
  return (0);
exit:
  return (1);
}
#undef X

int
json2node(
  void *(*a)(void *, unsigned long)
 ,jsonNode_t *n
 ,jsonSt_t *t
 ,const unsigned char *s
 ,unsigned int m
 ,unsigned int l
){
  struct jsonNodeCx cx;

  if (!a || !n || !m || !t || !s)
    return (-1);
  if (!l)
    return (0);
  cx.a = a;
  cx.n = n;
  return (jsonParse(jsonNodeCb, m, t, s, l, &cx));
}

void
jsonNodeWalk(
  jsonNode_t *n
 ,void (*a)(jsonNode_t *, unsigned int, jsonNodeVisit_t, void *)
 ,void *c
){
  unsigned int d;

  d = 0;
  while (n) {
    if (!n->nodeN) {
      a(n, d, jsonNodeVisitLeaf, c);
      n = n->parent;
      --d;
      continue;
    }
    if (!n->nodeW)
      a(n, d, jsonNodeVisitPreorder, c);
    else
      a(n, d, jsonNodeVisitInorder, c);
    if (n->nodeW < n->nodeN) {
      n = *(n->node + n->nodeW++);
      ++d;
    } else {
      a(n, d, jsonNodeVisitPostorder, c);
      n->nodeW = 0;
      n = n->parent;
      --d;
    }
  }
}

void
jsonNodeFree(
  void (*d)(void *)
 ,jsonNode_t *n
){
  while (n) {
    for (; n->nodeW < n->nodeN && (*(n->node + n->nodeW))->type != jsonTp_Jb && (*(n->node + n->nodeW))->type != jsonTp_Je; ++n->nodeW);
    if (n->nodeW < n->nodeN)
      n = *(n->node + n->nodeW++);
    else {
      while (n->nodeN)
        d(*(n->node + --n->nodeN));
      d(n->node);
      n->node = 0;
      n->nodeW = 0;
      n = n->parent;
    }
  }
}
