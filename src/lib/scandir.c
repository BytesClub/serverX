/*
 * Copyright (c) 2015-2016 Luke Lorimer <noryb009@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// include header
#include <serverX.h>

int alphasort(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

int antialphasort2(const struct dirent **a, const struct dirent **b) {
    return alphasort(b, a);
}

#define original_size 2

int scandir_main(const char *path, struct dirent ***e,
        int (*filter)(const void *),
        int (*compare)(const struct dirent **, const struct dirent **),
        int full_path) {
    int path_len = 0;
    if(full_path == 1)
        path_len = strlen(path);
    DIR *d = opendir(path);
    if(d == NULL)
        return -1;

    int n = 0;
    int size = original_size;
    struct dirent **ents = malloc(sizeof(struct dirent *) * size);
    struct dirent *next;

    while(1) {
        next = readdir(d);
        if(next == NULL)
            break;

        if(strcmp(next->d_name, ".") == 0 || strcmp(next->d_name, "..") == 0)
            continue;
        if(filter != NULL) {
            if(full_path == 0 && !filter(next))
                continue;
            else if(full_path == 1) {
                int len = path_len + 1 + strlen(next->d_name) + 1;
                char buf[len];
                strcat(strcat(strcpy(buf, path), "/"), next->d_name);
                if(!filter(buf))
                    continue;
            }
        }

        if(size == n) {
            size *= 2;
            struct dirent **newents = malloc(sizeof(struct dirent *) * size);
            if (newents == NULL) {
                free(ents);
                return -2;
            }
            memncpy(newents, ents, sizeof(ents));
            ents = newents;
        }

        ents[n] = malloc(sizeof(struct dirent));
        ents[n]->d_ino = next->d_ino;
        strcpy(ents[n]->d_name, next->d_name);
        n++;
    }

    if(compare != NULL)
        qsort(ents, n, sizeof(struct dirent *),
                (int (*)(const void *, const void *))compare);

    closedir(d);
    *e = ents;
    assert(n >= 0);
    return n;
}

int scandir(const char *path, struct dirent ***e,
        int (*filter)(const struct dirent *),
        int (*compare)(const struct dirent **, const struct dirent **)) {
    return scandir_main(path, e, (int (*)(const void *))filter, compare, 0);
}

int scandir_full_path(const char *path, struct dirent ***e,
        int (*filter)(const char *),
        int (*compare)(const struct dirent **, const struct dirent **)) {
    return scandir_main(path, e, (int (*)(const void *))filter, compare, 1);
}
