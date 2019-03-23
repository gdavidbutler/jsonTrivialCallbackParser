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

/* callback event types */
typedef enum {
  jsonTp_tb /* begin */
 ,jsonTp_te /* end */
 ,jsonTp_ts /* string */
 ,jsonTp_tn /* number */
 ,jsonTp_tt /* true */
 ,jsonTp_tf /* false */
 ,jsonTp_tu /* null */
 ,jsonTp_tr /* error */
} jsonTp_t;

/* pointers referencing fragments of constant buffer must include length */
typedef struct {
  const unsigned char *s; /* if  s, is object attribute with name length l */
  unsigned int l;         /* if !s, is array element with offset l */
} jsonSt_t;

/* prototype of a callback function */
/* returns 0 on success else aborts parse */
typedef int (*jsonCb_t)(
  jsonTp_t
 ,unsigned int numberOfLevels
 ,const jsonSt_t *level
 ,const jsonSt_t *value /* for begin/end, 0 = array --- !0 = object */
 ,void *userContext
);

/* return -1 on error else offset of last char parsed */
/* provide a tag buffer large enough for the deepest level of nesting supported */
int jsonParse(jsonCb_t, unsigned int numberOfLevelsInBuf, jsonSt_t *levelBuf, const unsigned char *json, unsigned int jlen, void *userContext);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonDecodeString(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);
            
/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonEncodeString(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonDecodeUri(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonEncodeUri(char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* estimated length of needed out buffer */
#define jsonDecodeBase64Need(inl) (((inl + 3) / 4) * 3)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonDecodeBase64(unsigned char *out, unsigned int olen, const char *in, unsigned int ilen);

/* estimated length of needed out buffer */
#define jsonEncodeBase64Need(inl) (((inl + 2) / 3) * 4)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int jsonEncodeBase64(char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);
