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
// urldecode.c

// include header
#include <serverX.h>

/**
 * Escapes string for HTML. Returns dynamically allocated memory for escaped
 * string that must be deallocated by caller.
 */
static bool helperescape(const char* entity, char* target, int* new)
{
    *new += strlen(entity);
	char* newTarget;
	newTarget = (char*)malloc(*new);
    void *memcpy(newTarget, target, *new);
	free(target);
    if (newTarget == NULL)    return false;
    strcat(newTarget, entity);
    return true;
}