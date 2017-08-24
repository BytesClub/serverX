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

// Library Header File
// serverX.h

#ifndef __serverX_h
#define __serverX_h

// Version info
#define serverX_VERSION "1.0.0"

// feature test macro requirements

#define _GNU_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// limits on an HTTP request's size, based on Apache's
// http://httpd.apache.org/docs/2.2/mod/core.html

#define LimitRequestFields 50
#define LimitRequestFieldSize 4094
#define LimitRequestLine 8190

// number of bytes for buffers
#define BYTES 512

// header files

#ifndef __getopt_h
    #include <getopt.h>
#endif
#ifndef __dirent_h
    #include <dirent.h>
#endif
#ifndef __errno_h
    #include <errno.h>
#endif
#ifndef __limits_h
    #include <limits.h>
#endif
#ifndef __math_h
    #include <math.h>
#endif
#ifndef __signal_h
    #include <signal.h>
#endif
#ifndef __stdbool_h
    #include <stdbool.h>
#endif
#ifndef __stdio_h
    #include <stdio.h>
#endif
#ifndef __stdlib_h
    #include <stdlib.h>
#endif
#ifndef __string_h
    #include <string.h>
#endif
#ifndef __strings_h
    #include <strings.h>
#endif
#ifndef __sys_stat_h
    #include <sys/stat.h>
#endif
#ifndef __sys_types_h
    #include <sys/types.h>
#endif
#ifndef __unistd_h
    #include <unistd.h>
#endif

// Windows specific files
#if defined(_WIN32) || defined(__WIN32__)
    #define OS_NAME "Windows NT"
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT
    #endif
    #define _WIN32_WINNT 0x0501
    #ifndef __windows_h
        #include <windows.h>
    #endif
    #ifndef __winsock2_h
        #include <winsock2.h>
    #endif
    #ifndef __ws2tcpip_h
        #include <ws2tcpip.h>
    #endif
    #ifndef __win32_serverX_h
        #include <winserverX.h>
    #endif
    #define SIGNAL(SIG, HANDLER) \
        signal((SIG), (HANDLER));
    #if defined(_MSC_VER)
        #define popen(x, y)  _popen(x, y)
        #define pclose(x)    _pclose(x)
    #endif

// Linux specific files
#else
    #define OS_NAME "GNU/Linux"
    #ifndef __arpa_inet_h
        #include <arpa/inet.h>
    #endif
    #ifndef __pthread_h
        #include <pthread.h>
    #endif
    #ifndef __sys_socket_h
        #include <sys/socket.h>
    #endif
    #define SIGNAL(SIG, HANDLER)     \
        struct sigaction act;        \
        act.sa_handler = (HANDLER);  \
        act.sa_flags = 0;            \
        sigemptyset(&act.sa_mask);   \
        sigaction((SIG), &act, NULL);
#endif

// prototypes

/**
 * @ function:       connected
 * @ brief:          Checks (without blocking) whether a client has connected
 *                   to the server.
 * @ returns:        True iff so, else false.
 */
bool connected(void);

/**
 * @ function:       error
 * @ brief:          Responds to client with specified status code.
 * @ param code:     The HTTP error code.
 */
void error(unsigned short code);

/**
 * @ function:       freedir
 * @ brief:          Frees memory allocated by scandir.
 * @ param namelist: List of files in given directory.
 * @ param n:        Number of files in given directory.
 */
void freedir(struct dirent** namelist, int n);

/**
 * @ function:       handler
 * @ brief:          Handles signals (SIGINT).
 * @ param signal:   The signal code.
 */
void handler(int signal);

/**
 * @ function:      htmlspecialchars
 * @ brief:         Escapes string for HTML. Returns dynamically allocated
 *                  memory for escaped string that must be deallocated by caller.
 * @ param s:       String that has to be escaped.
 * @ returns:       Pointer to escaped string.
 */
char* htmlspecialchars(const char* s);

/**
 * @ function:      indexes
 * @ brief:         Checks, in order, whether index.php or index.html exists
 *                  inside of path.
 * @ param path:    Requested path from URL.
 * @ returns:       Path to first match if so, else NULL.
 */
char* indexes(const char* path);

/**
 * @ function:      interpret
 * @ brief:         Interprets PHP file at path using query string.
 * @ param path:    Path to PHP script.
 * @ param query:   Query found with URL (optional).
 */
void interpret(const char* path, const char* query);

/**
 * @ function:      list
 * @ brief:         Responds to client with directory listing of path.
 * @ param path:    Requested path from URL.
 */
void list(const char* path);

/**
 * @ function:      load
 * @ brief:         Loads a file into memory dynamically allocated on heap.
 *                  Stores address in *content and length in *length.
 * @ param file:    File to be read into memory.
 * @ param content: Pointer to string holds address of the content.
 * @ param length:  Pointer to integer storing size of the content.
 * @ returns:       True iff action successful, false otherwise.
 */
bool load(FILE* file, char** content, size_t* length);

/**
 * @ function:      lookup
 * @ brief:         Finds MIME type for supported extensions, else NULL.
 * @ param path:    Path of the delivering file.
 * @ returns:       Type of file as string.
 */
const char* lookup(const char* path);

/**
 * @ function:      parse
 * @ brief:         Parses a request-line, storing its absolute-path at abs_path
 *                  and its query string at query, both of which are assumed
 *                  to be at least of length LimitRequestLine + 1.
 * @ param line:    HTTP request with URL.
 * @ param path:    Path extracted from URL.
 * @ param query:   Query extracted from URL.
 * @ returns:       True iff action successful, false otherwise.
 */
bool parse(const char* line, char* path, char* query);

/**
 * @ function:      process
 * @ brief:         Contains implementations for request handling.
 * @ param args:    Any argument sent as void pointer.
 * @ returns:       Return value is stored in memory, and address is returned.
 */
void* process(void* args);

/**
 * @ function:      reason
 * @ brief:         Returns status code's reason phrase.
 * @ param code:    Status code for error.
 * @ returns:       String against status code give.
 * @ ref:           http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6
 *                  https://tools.ietf.org/html/rfc2324
 */
const char* reason(unsigned short code);

/**
 * @ function:      redirect
 * @ brief:         Redirects client to uri.
 * @ param uri:     URL that it has to redirect.
 */
void redirect(const char* uri);

/**
 * @ function:      request
 * @ brief:         Reads (without blocking) an HTTP request's headers into
 *                  memory dynamically allocated on heap.
 *                  Stores address in *message and length in *length.
 * @ param message: Pointer to the buffer.
 * @ param length:  Pointer to the integer holding the size of buffer.
 * @ returns:       True iff action successful, false otherwise.
 */
bool request(char** message, size_t* length);

/**
 * @ function:      respond
 * @ brief:         Responds to a client with status code, headers, and body
 *                  of specified length.
 * @ param code:    HTTP status code.
 * @ param headers: String for HTTP headers.
 * @ param body:    Response body from buffer.
 * @ param length:  Size of buffer.
 */
void respond(int code, const char* headers, const char* body, size_t length);

/**
 * @ function:      start
 * @ brief:         Starts server on specified port rooted at path.
 * @ param port:    TCP Port configured for connection.
 * @ param path:    Root path for communication.
 */
void start(short port, const char* path);

/**
 * @ function:      stop
 * @ brief:         Stop server, deallocating any resources.
 */
void stop(void);

/**
 * @ function:      transfer
 * @ brief:         Transfers file at path with specified type to client.
 * @ param path:    Path of the file source.
 * @ param type:    MIME-type of the file.
 */
void transfer(const char* path, const char* type);

/**
 * @ function:      urldecode
 * @ brief:         URL-decodes string, returning dynamically allocated memory
 *                  for decoded string that must be deallocated by caller.
 * @ param s:       URL request string.
 * @ returns:       Decoded request string.
 */
char* urldecode(const char* s);

#endif // Ends serverX.h
