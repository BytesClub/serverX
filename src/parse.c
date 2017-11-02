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
// parse.c

// include header
#include <serverX.h>

/**
 * Parses a request-line, storing its absolute-path at abs_path
 * and its query string at query, both of which are assumed
 * to be at least of length LimitRequestLine + 1.
 */
bool parse(int cfd, const char* line, char* abs_path, char* query)
{
    if (cfd == -1 || line == NULL) {
        return false;
    } else if (abs_path != NULL) {
        memset(abs_path, '\0', LimitRequestLine + 1);
    } else if (query != NULL) {
        memset(query, '\0', LimitRequestLine + 1);
    }
    unsigned int length = strlen(line);
    char toParse[length + 1];
    strcpy(toParse, line);
    char *peek = strtok(toParse, " ");
    if (strcmp(peek, "GET")) {
        error(cfd, 405);
        return false;
    }
    peek = strtok(NULL, " ");
    if (*peek != '/') {
        error(cfd, 501);
        return false;
    } else if (strchr(peek, '\"')) {
        error(cfd, 400);
        return false;
    }
    strncpy(query, "", 1);
    char *q = strchr(peek, '?');
    int n = 0;
    if (q != NULL) {
        n = strlen(q);
        strncpy(query, q + 1, n - 1);
    }
    int m = strlen(peek) - n;
    strncpy(abs_path, peek, m);
    peek = strtok(NULL, " ");
    if (strcmp(peek, "HTTP/1.1\r\n")) {
        error(cfd, 505);
        return false;
    }
    return true;
}
