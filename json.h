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

/* pointers referencing fragments of constant buffer must include length */
typedef struct {
  const unsigned char *s;
  unsigned int l;
} jsonSt_t;

/* callback event types */
typedef enum {
  jsonTp_Jb /* begin - value->l is zero if array else object, value->s points to '[' or '{' */
 ,jsonTp_Je /* end - value->l is zero if array, else object, value->s points to ']' or '}' */
 ,jsonTp_Js /* string */
 ,jsonTp_Jn /* number */
 ,jsonTp_Jt /* true */
 ,jsonTp_Jf /* false */
 ,jsonTp_Ju /* null */
} jsonTp_t;

/* prototype of a callback function */
/* returns 0 on success else aborts parse */
typedef int (*jsonCb_t)(
  jsonTp_t
 ,unsigned int numberOfLevels
 ,const jsonSt_t *level /* ->s is NULL if array element at offset ->l, otherwise attribute name with length ->l */
 ,const jsonSt_t *value
 ,void *userContext
);

/* return -1 on error else offset of last char parsed */
/* provide a tag buffer large enough for the deepest level of nesting supported */
int jsonParse(
  jsonCb_t
 ,unsigned int numberOfLevelsInBuf
 ,jsonSt_t *levelBuf
 ,const unsigned char *json
 ,unsigned int jlen
 ,void *userContext
);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonDecodeString(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);
            
/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonEncodeString(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* estimated length of needed out buffer */
#define jsonDecodeBase64Need(inl) (((inl + 3) / 4) * 3)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonDecodeBase64(
  unsigned char *out
 ,unsigned int olen
 ,const char *in
 ,unsigned int ilen
);

/* estimated length of needed out buffer */
#define jsonEncodeBase64Need(inl) (((inl + 2) / 3) * 4)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonEncodeBase64(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);
