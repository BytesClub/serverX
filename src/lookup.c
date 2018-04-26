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
// lookup.c

// include header
#include <serverX.h>

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* path)
{
    if (path == NULL)    return NULL;

    char *x =  strrchr(path, '.');
    char s[strlen(x)];
    strcpy(s, x);

    if (! strcasecmp(s, ".html"))        return "text/html";
    else if (! strcasecmp(s, ".css"))    return "text/css";
    else if (! strcasecmp(s, ".js"))     return "text/javascript";
    else if (! strcasecmp(s, ".json"))   return "application/json";
    else if (! strcasecmp(s, ".gif"))    return "image/gif";
    else if (! strcasecmp(s, ".png"))    return "image/png";
    else if (! strcasecmp(s, ".jpg"))    return "image/jpeg";
    else if (! strcasecmp(s, ".ico"))    return "image/x-icon";
    else if (! strcasecmp(s, ".php"))    return "text/x-php";

    return NULL;
}
