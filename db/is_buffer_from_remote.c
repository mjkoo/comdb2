/*
   Copyright 2015 Bloomberg Finance L.P.

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

/* is buffer from remote checks for prccom/bigsnd remote bit.
    paul x1552 */

int is_buffer_from_remote(const void *buf)
{
    unsigned char *u = (unsigned char *)buf;
    return (u[0] & 0x10) != 0; /*this is the historical 'from-remote' bit*/
}
