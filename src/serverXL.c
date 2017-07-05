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
// serverXL.c

// include header
#include <serverXL.h>

// global variables

extern char* root;
extern int cfd, sfd;
extern bool signaled;

/**
 * Checks (without blocking) whether a client has connected to server.
 * Returns true iff so.
 */
bool connected(void)
{
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t cli_len = sizeof(cli_addr);
    cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
    if (cfd == -1)    return false;
    return true;
}

/**
 * Responds to client with specified status code.
 */
void error(unsigned short code)
{
    // determine code's reason-phrase
    const char* phrase = reason(code);
    if (phrase == NULL)    return;

    // template for response's content
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s\
                    </h1></body></html>";

    // render template
    char body[(strlen(template) - 2 - ((int) log10(code) + 1) - 2 +\
               strlen(phrase)) * 2 + 1];
    int length = sprintf(body, template, code, phrase, code, phrase);
    if (length < 0) {
        body[0] = '\0';
        length = 0;
    }

    // respond with error
    char* headers = "Content-Type: text/html\r\n";
    respond(code, headers, body, length);
}

/**
 * Frees memory allocated by scandir.
 */
void freedir(struct dirent** namelist, int n)
{
    if (namelist != NULL) {
        for (int i = 0; i < n; i++)    free(namelist[i]);
        free(namelist);
    }
}

/**
 * Handles signals.
 */
void handler(int signal)
{
    // control-c
    if (signal == SIGINT)    signaled = true;
}

/**
 * Escapes string for HTML. Returns dynamically allocated memory for escaped
 * string that must be deallocated by caller.
 */
static bool helperescape(const char* entity, char* target, int* new)
{
    *new += strlen(entity);
    target = realloc(target, *new);
    if (target == NULL)    return false;
    strcat(target, entity);
    return true;
}

char* htmlspecialchars(const char* s)
{
    // ensure s is not NULL
    if (s == NULL)    return NULL;

    // allocate enough space for an unescaped copy of s
    char* t = malloc(strlen(s) + 1);
    if (t == NULL)    return NULL;
    t[0] = '\0';

    // iterate over characters in s, escaping as needed
    for (int i = 0, old = strlen(s), new = old; i < old; i++) {
        if (s[i] == '&') {
            const char* entity = "&amp;";
            helperescape(entity, t, &new);
        } else if (s[i] == '"') {
            const char* entity = "&quot;";
            helperescape(entity, t, &new);
        } else if (s[i] == '\'') {
            const char* entity = "&#039;";
            helperescape(entity, t, &new);
        } else if (s[i] == '<') {
            const char* entity = "&lt;";
            helperescape(entity, t, &new);
        } else if (s[i] == '>') {
            const char* entity = "&gt;";
            helperescape(entity, t, &new);
        } else {
            strncat(t, s + i, 1);
        }

        // memory error
        if (t == NULL)    return NULL;
    }

    // escaped string
    return t;
}

/**
 * Checks, in order, whether index.php or index.html exists inside of path.
 * Returns path to first match if so, else NULL.
 */
char* indexes(const char* path)
{
    char *file = malloc(strlen(path) + 10);
    strcpy(file, path);
    strcat(file, "index.php");

    if (! access(file, F_OK)) {
        return file;
    } else if (strcpy(strstr(file, ".php"), ".html") && ! access(file, F_OK)) {
        return file;
    }
    //default return
    return NULL;
}

/**
 * Interprets PHP file at path using query string.
 */
void interpret(const char* path, const char* query)
{
    // ensure path is readable
    if (access(path, R_OK) == -1) {
        error(403);
        return;
    }

    // open pipe to PHP interpreter
    char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\
                  \"%s\" php";
    char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
    if (sprintf(command, format, query, path) < 0) {
        error(500);
        return;
    }

    FILE* file = popen(command, "r");
    if (file == NULL) {
        error(500);
        return;
    }

    // load interpreter's content
    char* content;
    size_t length;
    if (load(file, &content, &length) == false) {
        error(500);
        return;
    }

    // close pipe
    pclose(file);

    // subtract php-cgi's headers from content's length to get body's length
    char* haystack = content;
    char* needle = strstr(haystack, "\r\n\r\n");
    if (needle == NULL) {
        free(content);
        error(500);
        return;
    }

    // extract headers
    char headers[needle + 2 - haystack + 1];
    strncpy(headers, content, needle + 2 - haystack);
    headers[needle + 2 - haystack] = '\0';

    // respond with interpreter's content
    respond(200, headers, needle + 4, length - (needle - haystack + 4));

    // free interpreter's content
    free(content);
}

/**
 * Responds to client with directory listing of path.
 */
void list(const char* path)
{
    // ensure path is readable and executable
    if (access(path, R_OK | X_OK) == -1) {
        error(403);
        return;
    }

    // open directory
    DIR* dir = opendir(path);
    if (dir == NULL)    return;

    // buffer for list items
    char* list = malloc(1);
    list[0] = '\0';

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
            error(500);
            return;
        }

        // append list item to buffer
        char* template = "<li><a href=\"%s\">%s</a></li>";
        list = realloc(list, strlen(list) + strlen(template) - 2 + strlen(name)
              - 2 + strlen(name) + 1);
        if (list == NULL) {
            free(name);
            freedir(namelist, n);
            error(500);
            return;
        }
        if (sprintf(list + strlen(list), template, name, name) < 0) {
            free(name);
            freedir(namelist, n);
            free(list);
            error(500);
            return;
        }

        // free escaped name
        free(name);
    }

    // free memory allocated by scandir
    freedir(namelist, n);

    // prepare response
    const char* relative = path + strlen(root);
    char* template = "<html><head><title>%s</title></head><body><h1>%s</h1><ul>\
                    %s</ul></body></html>";
    char body[strlen(template) - 2 + strlen(relative) - 2 + strlen(relative) - 2\
              + strlen(list) + 1];
    int length = sprintf(body, template, relative, relative, list);
    if (length < 0)
    {
        free(list);
        closedir(dir);
        error(500);
        return;
    }

    // free buffer
    free(list);

    // close directory
    closedir(dir);

    // respond with list
    char* headers = "Content-Type: text/html\r\n";
    respond(200, headers, body, length);
}

/**
 * Loads a file into memory dynamically allocated on heap.
 * Stores address thereof in *content and length thereof in *length.
 */
bool load(FILE* file, char** content, size_t* length)
{
    *length = 0;
    *content = NULL;
    if (fseek(file, 0L, SEEK_END) != -1) {
        *length = ftell(file);
        rewind(file);
    }
    if (*length != 0) {
        *content = (char *)malloc(*length * sizeof(char));
        fread(*content, sizeof(char), *length, file);
    } else {
        int c;
        while((c = fgetc(file)) != EOF) {
            char *buffer = (char *)malloc((*length + 1) * sizeof(char));
            memcpy(buffer, *content, *length);
            free(*content);
            *(buffer + *length) = c;
            *content = buffer;
            *length += 1;
        }
    }
    if (ferror(file)) {
        free(*content);
        content = NULL, *length = 0;
        return false;
    }
    return true;
}

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* path)
{
    if (path == NULL)    return NULL;

    char *x =  strrchr(path, '.');
    char s[strlen(x)];
    strcpy(s, x);

    if (! strcasecmp(s, ".html"))        return "text/html";
    else if (! strcasecmp(s, ".css"))    return "text/css";
    else if (! strcasecmp(s, ".js"))     return "text/javascript";
    else if (! strcasecmp(s, ".gif"))    return "image/gif";
    else if (! strcasecmp(s, ".png"))    return "image/png";
    else if (! strcasecmp(s, ".jpg"))    return "image/jpeg";
    else if (! strcasecmp(s, ".ico"))    return "image/x-icon";
    else if (! strcasecmp(s, ".php"))    return "text/x-php";

    return NULL;
}

/**
 * Parses a request-line, storing its absolute-path at abs_path
 * and its query string at query, both of which are assumed
 * to be at least of length LimitRequestLine + 1.
 */
bool parse(const char* line, char* abs_path, char* query)
{
    if (cfd == -1 || line == NULL)    return false;
    if (abs_path != NULL) {
        memset(abs_path, '\0', LimitRequestLine + 1);
    }
    if (query != NULL) {
        memset(query, '\0', LimitRequestLine + 1);
    }
    unsigned int length = strlen(line);
    char toParse[length + 1];
    strcpy(toParse, line);
    char *peek = strtok(toParse, " ");
    if (strcmp(peek, "GET")) {
        error(405);
        return false;
    }
    peek = strtok(NULL, " ");
    if (*peek != '/') {
        error(501);
        return false;
    }
    if (strchr(peek, '\"')) {
        error(400);
        return false;
    }
    strcpy(query, "");
    char *q = strchr(peek, '?');
    int n = 0;
    if (q) {
        n = strlen(q);
        strcpy(query, q + 1);
    }
    int m = strlen(peek) - n;
    strncpy(abs_path, peek, m);
    peek = strtok(NULL, " ");
    if (strcmp(peek, "HTTP/1.1\r\n")) {
        error(505);
        return false;
    }
    return true;
}

/**
 * Returns status code's reason phrase.
 *
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6
 * https://tools.ietf.org/html/rfc2324
 */
const char* reason(unsigned short code)
{
    switch (code) {
        case 200: return "OK";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 414: return "Request-URI Too Long";
        case 418: return "I'm a teapot";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        default:  return NULL;
    }
}

/**
 * Redirects client to uri.
 */
void redirect(const char* uri)
{
    char* template = "Location: %s\r\n";
    char headers[strlen(template) - 2 + strlen(uri) + 1];
    if (sprintf(headers, template, uri) < 0) {
        error(500);
        return;
    }
    respond(301, headers, NULL, 0);
}

/**
 * Reads (without blocking) an HTTP request's headers into memory dynamically allocated on heap.
 * Stores address thereof in *message and length thereof in *length.
 */
bool request(char** message, size_t* length)
{
    // ensure socket is open
    if (cfd == -1)    return false;

    // initialize message and its length
    *message = NULL;
    *length = 0;

    // read message
    while (*length < LimitRequestLine + LimitRequestFields *
            LimitRequestFieldSize + 4) {
        // read from socket
        char buffer[BYTES];
        ssize_t bytes = read(cfd, buffer, BYTES);
        if (bytes < 0) {
            if (*message != NULL) {
                free(*message);
                *message = NULL;
            }
            *length = 0;
            break;
        }

        // append bytes to message
        *message = realloc(*message, *length + bytes + 1);
        if (*message == NULL) {
            *length = 0;
            break;
        }
        memcpy(*message + *length, buffer, bytes);
        *length += bytes;

        // null-terminate message thus far
        *(*message + *length) = '\0';

        // search for CRLF CRLF
        int offset = (*length - bytes < 3) ? *length - bytes : 3;
        char* haystack = *message + *length - bytes - offset;
        char* needle = strstr(haystack, "\r\n\r\n");
        if (needle != NULL) {
            // trim to one CRLF and null-terminate
            *length = needle - *message + 2;
            *message = realloc(*message, *length + 1);
            if (*message == NULL)    break;
            *(*message + *length) = '\0';

            // ensure request-line is no longer than LimitRequestLine
            haystack = *message;
            needle = strstr(haystack, "\r\n");
            if (needle == NULL || (needle - haystack + 2) > LimitRequestLine) {
                break;
            }

            // count fields in message
            int fields = 0;
            haystack = needle + 2;
            while (*haystack != '\0') {
                // look for CRLF
                needle = strstr(haystack, "\r\n");
                if (needle == NULL)    break;

                // ensure field is no longer than LimitRequestFieldSize
                if (needle - haystack + 2 > LimitRequestFieldSize) {
                    break;
                }

                // look beyond CRLF
                haystack = needle + 2;
            }

            // if we didn't get to end of message, we must have erred
            if (*haystack != '\0')    break;

            // ensure message has no more than LimitRequestFields
            if (fields > LimitRequestFields)    break;

            // valid
            return true;
        }
    }

    // invalid
    if (*message != NULL)    free(*message);
    *message = NULL;
    *length = 0;
    return false;
}

/**
 * Responds to a client with status code, headers, and body of specified length.
 */
void respond(int code, const char* headers, const char* body, size_t length)
{
    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = reason(code);
    if (phrase == NULL)    return;

    // respond with Status-Line
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0)    return;

    // respond with headers
    if (dprintf(cfd, "%s", headers) < 0)    return;

    // respond with CRLF
    if (dprintf(cfd, "\r\n") < 0)    return;

    // respond with body
    if (write(cfd, body, length) == -1)    return;

    // log response line
    if (code == 200) {
        // green
        printf("\033[32m");
    } else {
        // red
        printf("\033[33m");
    }
    printf("HTTP/1.1 %i %s", code, phrase);
    printf("\033[39m\n");
}

/**
 * Starts server on specified port rooted at path.
 */
void start(short port, const char* path)
{
    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)    stop();

    // ensure root is executable
    if (access(root, X_OK) == -1)    stop();

    // announce root
    printf("\033[33m");
    printf("Using %s for server's root", root);
    printf("\033[39m\n");

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)    stop();

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
        printf("\033[33m");
        printf("Port %i already in use", port);
        printf("\033[39m\n");
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)    stop();

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)    stop();
    printf("\033[33m");
    printf("Listening on port %i", ntohs(addr.sin_port));
    printf("\033[39m\n");
}

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // announce stop
    printf("\033[33m");
    printf("Stopping server\n");
    printf("\033[39m");

    // free root, which was allocated by realpath
    if (root != NULL)    free(root);

    // close server socket
    if (sfd != -1)    close(sfd);

    // stop server
    exit(errsv);
}

/**
 * Transfers file at path with specified type to client.
 */
void transfer(const char* path, const char* type)
{
    // ensure path is readable
    if (access(path, R_OK) == -1) {
        error(403);
        return;
    }

    // open file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        error(500);
        return;
    }

    // load file's content
    char* content;
    size_t length;
    if (load(file, &content, &length) == false) {
        error(500);
        return;
    }

    // close file
    fclose(file);

    // prepare response
    char* template = "Content-Type: %s\r\n";
    char headers[strlen(template) - 2 + strlen(type) + 1];
    if (sprintf(headers, template, type) < 0) {
        error(500);
        return;
    }

    // respond with file's content
    respond(200, headers, content, length);

    // free file's content
    free(content);
}

/**
 * URL-decodes string, returning dynamically allocated memory for decoded string
 * that must be deallocated by caller.
 */
char* urldecode(const char* s)
{
    // check whether s is NULL
    if (s == NULL)    return NULL;

    // allocate enough (zeroed) memory for an undecoded copy of s
    char* t = calloc(strlen(s) + 1, 1);
    if (t == NULL)    return NULL;

    // iterate over characters in s, decoding percent-encoded octets, per
    // https://www.ietf.org/rfc/rfc3986.txt
    for (int i = 0, j = 0, n = strlen(s); i < n; i++, j++) {
        if (i < n - 2 && s[i] == '%') {
            char octet[3];
            octet[0] = s[i + 1];
            octet[1] = s[i + 2];
            octet[2] = '\0';
            t[j] = (char) strtol(octet, NULL, 16);
            i += 2;
        } else if (s[i] == '+') {
            t[j] = ' ';
        } else {
            t[j] = s[i];
        }
    }

    // escaped string
    return t;
}
