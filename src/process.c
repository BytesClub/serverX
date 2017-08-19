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
===============================================================================*/

// Library Source File
// process.c

// include header
#include <serverX.h>

// global variable

extern char* root;


/**
 * Process for extacting client request and respond
 * To be used by each thread independently
 * Reference: http://man7.org/linux/man-pages/man7/pthreads.7.html
 */
void* process(void *args) {
    // NULL argument must be given
    if (args != NULL) {
        error(500);
        pthread_exit((void*)1);
    }

    // a message and its length
    char* message = NULL;
    size_t length = 0;

    // path requested
    char* path = NULL;

    // check for request
    if (request(&message, &length)) {
        // extract message's request-line
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
        const char* haystack = message;
        const char* needle = strstr(haystack, "\r\n");
        if (needle == NULL) {
            error(500);
            pthread_exit((void*)2);
        }
        char line[needle - haystack + 2 + 1];
        strncpy(line, haystack, needle - haystack + 2);
        line[needle - haystack + 2] = '\0';

        // log request-line
        printf("%s", line);

        // parse request-line
        char abs_path[LimitRequestLine + 1];
        char query[LimitRequestLine + 1];
        if (parse(line, abs_path, query)) {
            // URL-decode absolute-path
            char* p = urldecode(abs_path);
            if (p == NULL) {
                error(500);
                pthread_exit((void*)2);
            }

            // resolve absolute-path to local path
            path = malloc(strlen(root) + strlen(p) + 1);
            if (path == NULL) {
                error(500);
                pthread_exit((void*)2);
            }
            strcpy(path, root);
            strcat(path, p);
            free(p);

            // ensure path exists
            if (access(path, F_OK) == -1) {
                error(404);
                pthread_exit((void*)2);
            }

            // if path to directory
            struct stat sb;
            if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                // redirect from absolute-path to absolute-path/
                if (abs_path[strlen(abs_path) - 1] != '/') {
                    char uri[strlen(abs_path) + 1 + 1];
                    strcpy(uri, abs_path);
                    strcat(uri, "/");
                    redirect(uri);
                    pthread_exit((void*)0);
                }

                // use path/index.php or path/index.html, if present, instead of directory's path
                char* index = indexes(path);
                if (index != NULL) {
                    free(path);
                    path = index;
                } else {
                    // list contents of directory
                    list(path);
                    pthread_exit((void*)0);
                }
            }

            // look up MIME type for file at path
            const char* type = lookup(path);
            if (type == NULL) {
                error(501);
                pthread_exit((void*)2);
            }

            // interpret PHP script at path
            if (strcasecmp("text/x-php", type) == 0) {
                interpret(path, query);
            }

            // transfer file at path
            else {
                transfer(path, type);
            }
        }
    }

    // free last path, if any
    if (path != NULL) {
        free(path);
        path = NULL;
    }

    // free last message, if any
    if (message != NULL) {
        free(message);
        message = NULL;
    }

    pthread_exit((void*)0);
}
