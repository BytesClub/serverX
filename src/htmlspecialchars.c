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
// serverXL.c

// include header
#include <serverXL.h>

// global variables

extern char* root;
extern int cfd, sfd;
extern bool signaled;

char* htmlspecialchars(const char* s)
{
    // ensure s is not NULL
    if (s == NULL)    return NULL;

    // allocate enough space for an unescaped copy of s
    char* t = malloc(strlen(s) + 1);
    if (t == NULL)    return NULL;
    t[0] = '\0';

    // iterate over characters in s, escaping as needed
    for (int i = 0, old = strlen(s), new = old; i < old; i++) {
        if (s[i] == '&') {
            const char* entity = "&amp;";
            helperescape(entity, t, &new);
        } else if (s[i] == '"') {
            const char* entity = "&quot;";
            helperescape(entity, t, &new);
        } else if (s[i] == '\'') {
            const char* entity = "&#039;";
            helperescape(entity, t, &new);
        } else if (s[i] == '<') {
            const char* entity = "&lt;";
            helperescape(entity, t, &new);
        } else if (s[i] == '>') {
            const char* entity = "&gt;";
            helperescape(entity, t, &new);
        } else {
            strncat(t, s + i, 1);
        }

        // memory error
        if (t == NULL)    return NULL;
    }

    // escaped string
    return t;
}