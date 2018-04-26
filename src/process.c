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
extern int root_len;

/**
 * Process for extacting client request and respond
 * To be used by each thread independently
 * Reference: http://man7.org/linux/man-pages/man7/pthreads.7.html
 */
void process(int cfd)
{
    // a message and its length
    char* message = NULL;
    size_t length = 0;

    // path requested
    char* path = NULL;

    // check for request
    if (request(cfd, &message, &length)) {
        // extract message's request-line
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
        const char* haystack = message;
        const char* needle = strstr(haystack, "\r\n");
        if (needle == NULL) {
            error(cfd, 500);
            free(message);
            message = NULL;
            close(cfd);
            exit(EXIT_FAILURE);
        }
        char line[needle - haystack + 2 + 1];
        strncpy(line, haystack, needle - haystack + 2);
        line[needle - haystack + 2] = '\0';

        // log request-line
        char log_message[BYTES];
        sprintf(log_message, "Client ID: %i  Subprocess ID: %lli\n%s",
            cfd, (long long)getpid(), line);
        printl("request", log_message, 0);

        // parse request-line
        char abs_path[LimitRequestLine + 1] = { 0 };
        char query[LimitRequestLine + 1] = { 0 };
        if (parse(cfd, line, abs_path, query)) {
            // URL-decode absolute-path
            char* p = urldecode(abs_path);
            if (p == NULL) {
                error(cfd, 500);
                free(message);
                message = NULL;
                close(cfd);
                exit(EXIT_FAILURE);
            }

            // resolve absolute-path to local path
            int p_len = strlen(p);
            path = malloc(root_len + p_len + 1);
            if (path == NULL) {
                error(cfd, 500);
                free(p);
                free(message);
                message = NULL;
                close(cfd);
                exit(EXIT_FAILURE);
            }
            memset(path, 0, root_len + p_len + 1);
            strncpy(path, root, root_len);
            strncat(path, p, p_len);
            free(p);

            // ensure path exists
            if (access(path, F_OK) == -1) {
                error(cfd, 404);
                free(path);
                path = NULL;
                free(message);
                message = NULL;
                close(cfd);
                exit(EXIT_FAILURE);
            }

            // if path to directory
            struct stat sb;
            if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                // redirect from absolute-path to absolute-path/
                int abs_path_len = strlen(abs_path);
                if (abs_path[abs_path_len - 1] != '/') {
                    char uri[abs_path_len + 2];
                    strncpy(uri, abs_path, abs_path_len);
                    strncat(uri, "/", 1);
                    redirect(cfd, uri);
                    free(path);
                    path = NULL;
                    free(message);
                    message = NULL;
                    close(cfd);
                    exit(EXIT_FAILURE);
                }

                // use path/index.php or path/index.html, if present, instead of directory's path
                char* index = indexes(path);
                if (index != NULL) {
                    free(path);
                    path = index;
                    free(message);
                    message = NULL;
                } else {
                    // list contents of directory
                    list(cfd, path);
                    free(path);
                    path = NULL;
                    free(message);
                    message = NULL;
                    close(cfd);
                    exit(EXIT_SUCCESS);
                }
            }

            // look up MIME type for file at path
            const char* type = lookup(path);
            if (type == NULL) {
                // invalid file type
                error(cfd, 501);
                free(path);
                path = NULL;
                free(message);
                message = NULL;
                close(cfd);
                exit(EXIT_FAILURE);
            }

            // interpret PHP script at path
            if (strcasecmp("text/x-php", type) == 0) {
                interpret(cfd, "PHP", path, query);
                free(path);
                path = NULL;
                free(message);
                message = NULL;
                close(cfd);
                exit(EXIT_SUCCESS);
            } else {
                // transfer file at path
                transfer(cfd, path, type);
                free(path);
                path = NULL;
                free(message);
                message = NULL;
                close(cfd);
                exit(EXIT_SUCCESS);
            }
        } else {
            error(cfd, 400);
            free(path);
            path = NULL;
            free(message);
            message = NULL;
            close(cfd);
            exit(EXIT_FAILURE);
        }
    }

    /* NONREACHABLE*/
    exit(EXIT_FAILURE);
}
