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
===============================================================================*/

// Main Source File
// serverX.c

// include header
#include <serverX.h>

// Help function
static void help(void);

// server's root
char* root = NULL;
int root_len = 0;

// file descriptor for sockets

int sfd = -1;
int* cfd;

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    errno = 0;
    setprogname(argv[0]);

    // Thread ID and Attributes
    pthread_attr_t tattr;

    // default to port 8080
    int port = 8080;

    // parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "hpl:")) != -1) {
        switch (opt) {
            // -p port
            case 'p':
                port = atoi(optarg);
                break;

            // -l logfile
            case 'l':
                if (freopen(optarg, "a+", stdout) == NULL) {
                    fprintf(stderr, "Failed to open logfile %s\n", optarg);
                }
                break;

            // -h help option
            case 'h':
            case '?':
            default:
                help();
        }
    }

    // ensure port is a non-negative short and path to server's root is specified
    if (port < 0 || port > SHRT_MAX || argv[optind] == NULL ||
        strlen(argv[optind]) == 0) {
        // announce usage
        help();
    }

    // Check if we have permission to use the port
    if (strcmp(KERNEL, "GNU/Linux") == 0 && port < 1024 && getuid()) {
        fprintf(stderr, "To bind ports lower than 1024 you must be root.\n");
        return 2;
    }

    // start server
    start(port, argv[optind]);

    // listen for SIGINT (aka control-c)
    SIGNAL(SIGINT, handler);

    // accept connections one at a time
    while (true) {
        // Client socket descriptor
        cfd = malloc(sizeof(int));

        // check whether client has connected
        if (cfd != NULL) {
            if ((*cfd = connected()) == -1) {
                free(cfd), cfd = NULL;
                continue;
            }
            pthread_t tid;
            pthread_attr_init(&tattr);
            if (pthread_create(&tid, NULL, process, cfd)) {
                int errsv = errno;
                time_t epoch = time(NULL);
                printf("\033[31m");
                printf("%sFollowing error has occured while creating thread\n%s",
                ctime(&epoch), strerror(errsv));
                printf("\033[39m\n");
                error(*cfd, 500);
                free(cfd), cfd = NULL;
                continue;
            }
            pthread_detach(tid);
            cfd = NULL;
        }
    }

    // default return
    return 0;
}

// Help Function: usage string
static void help(void)
{
    fprintf(stderr, "Usage: %s [-p port] [-l logfile] /path/to/root\n\
    Copyright (c) 2017 serverX Authors, Bytes Club Apache 2.0 Licensed\n\
    Build Version: %s  Build Platform: %s  Build Time: %s\n",
    getprogname(), serverX_VERSION, PLATFORM, BUILD_TIME);
    exit(EXIT_FAILURE);
    /* NONREACHABLE */
}
