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
#if defined(_WIN32) || defined(__WIN32__)
    extern HANDLE hConsole;
#endif

/**
 * Checks if a connection needs to be closed.
 */
void checkcfds(bool status, time_t tstamp)
{
    // zero connection
    if (cfdlist == NULL) {
        return;
    }

    // default case
    client_t* cur = cfdlist;
    client_t* next;
    while (cur != NULL) {
        next = cur->next;

        // if connection is timed out or maximum limit reached
        if (status || (tstamp - cur->tstamp) > KeepAliveTimeout || cur->nreq ==
        KeepAliveMaximum) {
            // respond(cur->cfd, 200, "Connection: Close\r\n", NULL, 0);
            STATUS;
            printf("Closing connection:  Client ID: %d  Requests: %d  \
Last Used On: %s", cur->cfd, cur->nreq, ctime(&(cur->tstamp)));
            RESET;
            close(cur->cfd);
            free(cur);
            fflush(stdout);
        }

        cur = next;
    }
}
