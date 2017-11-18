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
// checkcfds.c

// include header
#include <serverX.h>

// global variables
extern client_t* cfdlist;

/**
 * Checks if a connection needs to be closed.
 */
void checkcfds(bool status, time_t tstamp)
{
    // zero connection
    if (cfdlist == NULL) {
        return;
    }

    // single connection
    if (cfdlist->next == NULL) {
        // if connection is timed out or maximum limit reached
        if (status || (tstamp - cfdlist->tstamp) > KeepAliveTimeout ||
        cfdlist->nreq == KeepAliveMaximum) {
            // respond(cfdlist->cfd, 200, "Connection: Close\r\n", NULL, 0);
            printf("\033[33mClosing connection:  Client ID: %d  Requests: %d  \
Last Used On: %zu\033[39m\n", cfdlist->cfd, cfdlist->nreq,
            (long unsigned) cfdlist->tstamp);
            close(cfdlist->cfd);
            free(cfdlist);
            cfdlist = NULL;
            printf("\033[32mConnection Closed!\033[39m\n");
            fflush(stdout);
            return;
        }
    }

    // general case
    client_t* prev = cfdlist;
    client_t* cur = cfdlist->next;
    while (cur != NULL) {
        // if connection is timed out or maximum limit reached
        if (status || (tstamp - cur->tstamp) > KeepAliveTimeout || cur->nreq ==
        KeepAliveMaximum) {
            prev->next = cur->next;
            // respond(cur->cfd, 200, "Connection: Close\r\n", NULL, 0);
            printf("\033[33mClosing connection:  Client ID: %d  Requests: %d  \
Last Used On: %zu\033[39m\n", cur->cfd, cur->nreq, (long unsigned) cur->tstamp);
            close(cur->cfd);
            free(cur);
            cur = prev->next;
            printf("\033[32mConnection Closed!\033[39m\n");
            fflush(stdout);
            continue;
        }

        prev = cur;
        cur = cur->next;
    }
}
