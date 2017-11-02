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
extern int* cfd;

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // time counter
    time_t epoch = time(NULL);

    // announce stop
    printf("\033[33m");
    printf("%sStopping server\n", ctime(&epoch));
    printf("\033[39m");

    // free root, which was allocated by realpath
    if (root != NULL)    free(root);

    // clear client socket allocated by main
    if (cfd != NULL)    free(cfd);

    // close server socket
    if (sfd != -1)    close(sfd);

    // flush output log
    fflush(stdout);

    // stop server
    pthread_exit(EXIT_SUCCESS);
}
