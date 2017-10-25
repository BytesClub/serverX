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
// load.c

// include header
#include <serverX.h>


/**
 * Loads a file into memory dynamically allocated on heap.
 * Stores address thereof in *content and length thereof in *length.
 */
bool load(FILE* file, char** content, size_t* length)
{
    *length = 0;
    *content = NULL;
    if (fseek(file, 0L, SEEK_END) != -1) {
        *length = ftell(file);
        rewind(file);
    }
    if (*length != 0) {
        *content = malloc(*length * sizeof(char));
        size_t bytes_read = fread(*content, sizeof(char), *length, file);
        if (bytes_read && bytes_read < *length) {
            free(*content);
            content = NULL, *length = 0;
            return false;
        }
    } else {
        int c;
        while((c = fgetc(file)) != EOF) {
            char* buffer = malloc((*length + 1) * sizeof(char));
            memcpy(buffer, *content, *length);
            free(*content);
            *(buffer + *length) = c;
            *content = buffer;
            *length += 1;
        }
    }
    if (ferror(file)) {
        free(*content);
        content = NULL, *length = 0;
        return false;
    }
    return true;
}
