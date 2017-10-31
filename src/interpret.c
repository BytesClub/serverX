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
// interpret.c

// include header
#include <serverX.h>

/**
 * Interprets PHP file at path using query string.
 */
void interpret(int cfd, const char* path, const char* query)
{
    // ensure path is readable
    if (access(path, R_OK) == -1) {
        error(cfd, 403);
        return;
    }

    // open pipe to PHP interpreter
    char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\
\"%s\" php";
    int command_len = strlen(format) + strlen(path) + strlen(query) - 3;
    char command[command_len];
    if (snprintf(command, command_len, format, query, path) < 0) {
        error(cfd, 500);
        return;
    }

    FILE* file = popen(command, "r");
    if (file == NULL) {
        error(cfd, 500);
        return;
    }

    // load interpreter's content
    char* content;
    size_t length;
    if (load(file, &content, &length) == false) {
        error(cfd, 500);
        return;
    }

    // close pipe
    pclose(file);

    // subtract php-cgi's headers from content's length to get body's length
    char* haystack = content;
    char* needle = strstr(haystack, "\r\n\r\n");
    if (needle == NULL) {
        free(content);
        error(cfd, 500);
        return;
    }

    // extract headers
    char headers[needle + 2 - haystack + 1];
    strncpy(headers, content, needle + 2 - haystack);
    headers[needle + 2 - haystack] = '\0';

    // respond with interpreter's content
    respond(cfd, 200, headers, needle + 4, length - (needle - haystack + 4));

    // free interpreter's content
    free(content);
}
