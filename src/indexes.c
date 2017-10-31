/* Copyright 2017 The serverX Authors. All Rights Reserved.
 * Maintainer: Bytes Club (https://bytesclub.github.io)<bytes-club@googlegroups.com>

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
==============================================================================*/

// Library Source File
// indexes.c

// include header
#include <serverX.h>

/**
 * Checks, in order, whether index.php or index.html exists inside of path.
 * Returns path to first match if so, else NULL.
 */
char* indexes(const char* path)
{
    const int ext_len = 11;
    int path_len = strlen(path);
    char *file = malloc(path_len + ext_len);
    strncpy(file, path, path_len);
    strcat(file, "index.php"); // This line have some memory issues, details below

    if (! access(file, F_OK)) {
        // index.php exists
        return file;
    } else if (strcpy(strstr(file, ".php"), ".html") && ! access(file, F_OK)) {
        // index.html exists
        return file;
    }
    //default return
    return NULL;
}

/** Valgrind Error Details
==4660== Thread 2:
==4660== Conditional jump or move depends on uninitialised value(s)
==4660==    at 0x402084: indexes (indexes.c:34)
==4660==    by 0x402863: process (process.c:111)
==4660==    by 0x513EDC4: start_thread (in /usr/lib64/libpthread-2.17.so)
==4660==    by 0x544A76C: clone (in /usr/lib64/libc-2.17.so)
==4660==
 */
