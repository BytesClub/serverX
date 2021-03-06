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
// respond.c

// include header
#include <serverX.h>

/**
 * Responds to a client with status code, headers, and body of specified length.
 */
void respond(int cfd, int code, const char* headers, const char* body, size_t length)
{
    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = reason(code);
    if (phrase == NULL)    return;

    // respond with Status-Line
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0)    return;

    // respond with headers
    const char* defaultHeader = "X-Content-Type-Options: nosniff\r\n\
Server: serverX/%s (%s)\r\n%s\r\n";
    int len = dprintf(cfd, defaultHeader, serverX_VERSION, PLATFORM, headers);
    if (len < 0)    return;

    // respond with body
    if (write(cfd, body, length) == -1)    return;

    // log response line
    int status = code == 200 ? 3 : 1;
    char message[1024];
    sprintf(message, "HTTP/1.1 %i %s\n", code, phrase);
    printl(__func__, message, status);
}
