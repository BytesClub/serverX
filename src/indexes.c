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
    const int path_len = strlen(path);
    const int index_len = path_len + 11; // extra space for index.[php/html]
    char* const file = malloc(index_len);
    if (file == NULL)    return NULL;
    memset(file, '\0', index_len);
    strncpy(file, path, path_len);
    strncpy(file + path_len, "index.php", 9);
    if (! access(file, F_OK)) {
        // index.php exists
        return file;
    }
    strncpy(file + path_len, "index.html", 10);
    if (! access(file, F_OK)) {
        // index.html exists
        return file;
    }
    //default return
    free(file);
    return NULL;
}
