/*
   Copyright 2018 Bloomberg Finance L.P.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#ifndef __TOHEX_H__
#define __TOHEX_H__
#include <logmsg.h>
#include <build/db_dbt.h>
char *util_tohex(char *out, const char *in, size_t len);
void hexdumpbuf(char *key, int keylen, char **buf);
void hexdump(loglvl lvl, unsigned char *key, int keylen);
void hexdumpdbt(DBT *dbt);
void hexdumpfp(FILE *fp, unsigned char *key, int keylen);
#endif
