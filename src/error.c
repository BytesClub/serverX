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
// error.c

// include header
#include <serverX.h>

/**
 * Responds to client with specified status code.
 */
void error(int cfd, unsigned short code)
{
    // determine code's reason-phrase
    const char* phrase = reason(code);
    if (phrase == NULL)    return;

    // template for response's content
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s\
</h1></body></html>";

    // render template
    int body_len = (strlen(template) - ((int) log10(code) + 1) - 4 +\
               strlen(phrase)) * 2 + 1;
    char body[body_len];
    int length = snprintf(body, body_len, template, code, phrase, code, phrase);
    if (length < 0) {
        body[0] = '\0';
        length = 0;
    }

    // respond with error
    char* headers = "Content-Type: text/html\r\n";
    respond(cfd, code, headers, body, length);
}
