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
// find.c

// include header
#include <serverX.h>

// global variables

extern client_t* cfdlist;
#if defined(_WIN32) || defined(__WIN32__)
    extern HANDLE hConsole;
#endif

/**
 * Creates new client info block and returns pointer to it's cfd
 */
static int* create(int cfd, time_t tstamp)
{
    client_t* cli = malloc(sizeof(client_t));
    if (cli == NULL)    return NULL;
    cli->cfd = cfd, cli->nreq = 1, cli->tstamp = tstamp;
    cli->next = cfdlist;
    cfdlist = cli;
    STATUS;
    printf("Created connection:  Client ID: %d  Last Used On: %s",
    cli->cfd, ctime(&cli->tstamp));
    RESET;
    fflush(stdout);
    return &(cli->cfd);
}

/**
 * Finds memory location for particular client.
 * Returns pointer to cfd integer in client info block.
 */
int* find(int cfd, time_t tstamp)
{
    // invalid connection
    if (cfd == -1) {
        return NULL;
    }

    // zero connection
    if (cfdlist == NULL) {
        return create(cfd, tstamp);
    }

    // general case
    for (client_t* cur = cfdlist; cur != NULL; cur = cur->next) {
        // found client id, already exists
        if (cur->cfd == cfd) {
            cur->nreq++;
            cur->tstamp = tstamp;
            return &(cur->cfd);
        }
    }
    return create(cfd, tstamp);
}
