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
// redirect.c

// include header
#include <serverX.h>


/**
 * Redirects client to uri.
 */
void redirect(int cfd, const char* uri)
{
    char* template = "Location: %s\r\n";
    int headers_len = strlen(template) + strlen(uri) - 1;
    char headers[headers_len];
    if (snprintf(headers, headers_len, template, uri) < 0) {
        error(cfd, 500);
        return;
    }
    respond(cfd, 301, headers, NULL, 0);
}
