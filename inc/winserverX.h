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
// winserverX.h

#ifndef __win32_serverX_h
#define __win32_serverX_h

// feature test macro requirements

#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// Header for dprintf() and vdprintf()

#ifndef __dprintf_h
#define __dprintf_h

#ifdef __STDC__
    #ifndef __stdarg_h
        #include <stdarg.h>
    #endif
#else
    #ifndef __varargs_h
        #include <varargs.h>
    #endif
#endif

int dprintf (int d, const char *format, ...);
int vdprintf (int d, const char *format, va_list ap);

#endif // dprintf.h

// Header for realpath()

#ifndef __realpath_h
#define __realpath_h

char *realpath(const char *path, char resolved_path[MAX_PATH]);

#endif // realpath.h

// Header for scandir()

#ifndef __scandir_h
#define __scandir_h

#ifndef __assert_h
    #include <assert.h>
#endif


int alphasort(const struct dirent **a, const struct dirent **b);

int antialphasort2(const struct dirent **a, const struct dirent **b);

int scandir(const char *path, struct dirent ***e,
        int (*filter)(const struct dirent *),
        int (*compare)(const struct dirent **, const struct dirent **));

int scandir_full_path(const char *path, struct dirent ***e,
        int (*filter)(const char *),
        int (*compare)(const struct dirent **, const struct dirent **));

#endif // scandir.h

// Header for ppid()

#ifndef __getppid_h
#define __getppid_h

#ifndef __tlhelp32_h
    #include <tlhelp32.h>
#endif

DWORD getppid();

#endif // getppid.h
