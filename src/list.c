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
// list.c

// include header
#include <serverX.h>

// global variables
extern char* root;

/**
 * Responds to client with directory listing of path.
 */
void list(int cfd, const char* path)
{
    // ensure path is readable and executable
    if (access(path, R_OK | X_OK) == -1) {
        error(cfd,403);
        return;
    }

    // open directory
    DIR* dir = opendir(path);
    if (dir == NULL)    return;

    // buffer for list items
    char* list = malloc(1);
    list[0] = '\0';
    int list_length = 0;

    // iterate over directory entries
    struct dirent** namelist = NULL;
    int n = scandir(path, &namelist, NULL, alphasort);
    for (int i = 0; i < n; i++) {
        // omit . from list
        if (strcmp(namelist[i]->d_name, ".") == 0)    continue;

        // escape entry's name
        char* name = htmlspecialchars(namelist[i]->d_name);
        if (name == NULL) {
            free(list);
            freedir(namelist, n);
            error(cfd, 500);
            return;
        }

        // append list item to buffer
        char* template = "<li><a href=\"%s\">%s</a></li>";
        int list_len = list_length + strlen(template) + 2 * strlen(name) - 3;
        char* newList = malloc(list_len);
        if (newList == NULL) {
            free(name);
            freedir(namelist, n);
            free(list);
            error(cfd, 500);
            return;
        }

        // copy previous content and free
        memcpy(newList, list, list_length);
        free(list);
        list = newList;

        // print after previous value in list
        if (snprintf(list + list_length, list_len, template, name, name) < 0) {
            free(name);
            freedir(namelist, n);
            free(list);
            error(cfd, 500);
            return;
        }

        // update buffer length
        list_length = list_len - 1;

        // free escaped name
        free(name);
    }

    // free memory allocated by scandir
    freedir(namelist, n);

    // prepare response
    const char* relative = path + strlen(root);
    char* template = "<!DOCTYPE HTML><html><head><title>Home - %s</title></head>\
<body><h1>%s</h1><ul>%s</ul></body></html>";
    char body[strlen(template) - 2 + strlen(relative) - 2 + strlen(relative) - 2\
              + strlen(list) + 1];
    int length = sprintf(body, template, relative, relative, list);
    if (length < 0)
    {
        free(list);
        closedir(dir);
        error(cfd, 500);
        return;
    }

    // free buffer
    free(list);

    // close directory
    closedir(dir);

    // respond with list
    char* headers = "Content-Type: text/html\r\n";
    respond(cfd, 200, headers, body, length);
}
