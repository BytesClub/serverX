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
// htmlspecialchars.c

// include header
#include <serverX.h>


/**
 * Escapes string for HTML. Returns dynamically allocated memory for escaped
 * string that must be deallocated by caller.
 */
static bool helperescape(const char* entity, char* target, int* new)
{
    *new += strlen(entity);
    target = realloc(target, *new);
    if (target == NULL)    return false;
    strcat(target, entity);
    return true;
}

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
            helperescape("&amp;", t, &new);
        } else if (s[i] == '"') {
            helperescape("&quot;", t, &new);
        } else if (s[i] == '\'') {
            helperescape("&#039;", t, &new);
        } else if (s[i] == '<') {
            helperescape("&lt;", t, &new);
        } else if (s[i] == '>') {
            helperescape("&gt;", t, &new);
        } else {
            strncat(t, s + i, 1);
        }

        // memory error
        if (t == NULL)    return NULL;
    }

    // escaped string
    return t;
}
