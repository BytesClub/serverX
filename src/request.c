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
// request.c

// include header
#include <serverX.h>

// global variables

extern int cfd;

/**
 * Reads (without blocking) an HTTP request's headers into memory dynamically allocated on heap.
 * Stores address thereof in *message and length thereof in *length.
 */
bool request(char** message, size_t* length)
{
    // ensure socket is open
    if (cfd == -1)    return false;

    // initialize message and its length
    *message = NULL;
    *length = 0;

    // read message
    while (*length < LimitRequestLine + LimitRequestFields *
            LimitRequestFieldSize + 4) {
        // read from socket
        char buffer[BYTES];
        ssize_t bytes = read(cfd, buffer, BYTES);
        if (bytes < 0) {
            if (*message != NULL) {
                free(*message);
                *message = NULL;
            }
            *length = 0;
            break;
        }

        // append bytes to message
        *message = realloc(*message, *length + bytes + 1);
        if (*message == NULL) {
            *length = 0;
            break;
        }
        memcpy(*message + *length, buffer, bytes);
        *length += bytes;

        // null-terminate message thus far
        *(*message + *length) = '\0';

        // search for CRLF CRLF
        int offset = (*length - bytes < 3) ? *length - bytes : 3;
        char* haystack = *message + *length - bytes - offset;
        char* needle = strstr(haystack, "\r\n\r\n");
        if (needle != NULL) {
            // trim to one CRLF and null-terminate
            *length = needle - *message + 2;
            *message = realloc(*message, *length + 1);
            if (*message == NULL)    break;
            *(*message + *length) = '\0';

            // ensure request-line is no longer than LimitRequestLine
            haystack = *message;
            needle = strstr(haystack, "\r\n");
            if (needle == NULL || (needle - haystack + 2) > LimitRequestLine) {
                break;
            }

            // count fields in message
            int fields = 0;
            haystack = needle + 2;
            while (*haystack != '\0') {
                // look for CRLF
                needle = strstr(haystack, "\r\n");
                if (needle == NULL)    break;

                // ensure field is no longer than LimitRequestFieldSize
                if (needle - haystack + 2 > LimitRequestFieldSize) {
                    break;
                }

                // look beyond CRLF
                haystack = needle + 2;
            }

            // if we didn't get to end of message, we must have erred
            if (*haystack != '\0')    break;

            // ensure message has no more than LimitRequestFields
            if (fields > LimitRequestFields)    break;

            // valid
            return true;
        }
    }

    // invalid
    if (*message != NULL)    free(*message);
    *message = NULL;
    *length = 0;
    return false;
}
