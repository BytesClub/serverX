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
// start.c

// include header
#include <serverX.h>

// global variables

extern char* root;
extern int sfd, root_len;
#if defined(_WIN32) || defined(__WIN32__)
    extern HANDLE hConsole;
#endif

/**
 * Starts server on specified port rooted at path.
 */
void start(short port, const char* path)
{
    // time counter
    time_t epoch;

    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)    stop();
    root_len = strlen(root);

    // ensure root is executable
    if (access(root, X_OK) == -1)    stop();

    // Process ID
    pid_t pid = getpid(), ppid = getppid();

    ALERT;
    // announce PID
    time(&epoch);
    printf("%sStarting Process: %lli\tParent: %lli\n", ctime(&epoch), (long long)pid, (long long)ppid);
    // announce root
    time(&epoch);
    printf("%sUsing %s for server's root\n", ctime(&epoch), root);
    RESET;

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        DANGER;
        time(&epoch);
        printf("%sFailed to create Socket\n", ctime(&epoch));
        RESET;
        stop();
    }

    // allow reuse of address (to avoid "Address already in use")
    char optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // assign name to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        DANGER;
        time(&epoch);
        printf("%sPort %i already in use\n", ctime(&epoch), port);
        RESET;
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)    stop();

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)    stop();
    SUCCESS;
    time(&epoch);
    printf("%sStarted Process: %lli\n", ctime(&epoch), (long long)pid);
    time(&epoch);
    printf("%sListening on port %i", ctime(&epoch), ntohs(addr.sin_port));
    RESET;

    // flush output log
    fflush(stdout);
}
