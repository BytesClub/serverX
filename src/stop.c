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
// stop.c

// include header
#include <serverX.h>

// global variables

extern char* root;
extern int sfd;
extern client_t* cfdlist;
#if defined(_WIN32) || defined(__WIN32__)
    extern HANDLE hConsole;
#endif

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // time counter
    time_t epoch = time(NULL);

    ALERT;
    // announce stop
    printf("\r%sStopping server\n", ctime(&epoch));
    RESET;

    // free root, which was allocated by realpath
    if (root != NULL)    free(root);

    // clear client socket allocated by find
    if (cfdlist != NULL)    checkcfds(true, epoch);

    // close server socket
    if (sfd != -1)    close(sfd);

    // flush output log
    fflush(stdout);

    // Windows compatible log message [close]
    #if defined(_WIN32) || defined(__WIN32__)
        CloseHandle(hConsole);
    #endif

    // stop server
    pthread_exit(EXIT_SUCCESS);
}
