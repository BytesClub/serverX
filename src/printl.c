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

// Library Source File
// printl.c

// include header
#include <serverX.h>

extern bool logger;
#if defined(_WIN32) || defined(__WIN32__)
    extern HANDLE hConsole;
#endif

/**
 * Prints log information into stdout/logfile.
 */
int printl(const char* module, const char* message, short status) {
    // initialize logging screen
    #if defined(_WIN32) || defined(__WIN32__)
        switch (status) {
            case 1:  SetConsoleTextAttribute(hConsole, 4); break;
            case 2:  SetConsoleTextAttribute(hConsole, 6); break;
            case 3:  SetConsoleTextAttribute(hConsole, 2); break;
            default: SetConsoleTextAttribute(hConsole, 1); break;
        }
    #else
        switch (status) {
            case 1:  if (! logger) printf("\033[31m"); break;
            case 2:  if (! logger) printf("\033[33m"); break;
            case 3:  if (! logger) printf("\033[32m"); break;
            default: if (! logger) printf("\033[34m"); break;
        }
    #endif
    
    // print message
    time_t epoch;
    time(&epoch);
    int ret = printf("%s%s: %s\n", ctime(&epoch), module, message);
    
    // reset to default
    #if defined(_WIN32) || defined(__WIN32__)
        SetConsoleTextAttribute(hConsole, 7);
    #else
        if (! logger) printf("\033[39m\n");
    #endif
    
    // flush the output
    fflush(stdout);
    return ret;
}