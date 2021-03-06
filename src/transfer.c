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
// transfer.c

// include header
#include <serverX.h>

/**
 * Transfers file at path with specified type to client.
 */
void transfer(int cfd, const char* path, const char* type)
{
    // ensure path is readable
    if (access(path, R_OK) == -1) {
        error(cfd, 403);
        return;
    }

    // open file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        error(cfd, 500);
        return;
    }

    // load file's content
    char* content;
    size_t length;
    if (load(file, &content, &length) == false) {
        error(cfd, 500);
        return;
    }

    // close file
    fclose(file);

    // prepare response
    char* template = "Content-Type: %s\r\nConnection: keep-alive\r\nKeep-Alive:\
timeout=%d, max=%d\r\n";
    int headers_len = strlen(template) + strlen(type) +
    ceil(log10(KeepAliveTimeout)) + ceil(log10(KeepAliveMaximum)) - 1;
    char headers[headers_len];
    if (snprintf(headers, headers_len, template, type, KeepAliveTimeout,
    KeepAliveMaximum) < 0) {
        error(cfd, 500);
        return;
    }

    // respond with file's content
    respond(cfd, 200, headers, content, length);

    // free file's content
    free(content);
}
