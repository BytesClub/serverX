#!/bin/bash

# Copyright 2017 The serverX Authors. All Rights Reserved.
# Maintainer: Bytes Club (https://bytesclub.github.io)<bytes-club@googlegroups.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

# Build support file
# build.sh

# Get Platform details
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    PLATFORM="Linux"
    if [ -f /etc/os-release ]; then
        # freedesktop.org and systemd
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        # linuxbase.org
        OS=$(lsb_release -si)
        VER=$(lsb_release -sr)
    elif [ -f /etc/lsb-release ]; then
        # For some versions of Debian/Ubuntu without lsb_release command
        . /etc/lsb-release
        OS=$DISTRIB_ID
        VER=$DISTRIB_RELEASE
    elif [ -f /etc/debian_version ]; then
        # Older Debian/Ubuntu/etc.
        OS="Debian"
        VER=$(cat /etc/debian_version)
    elif [ -f /etc/SuSe-release ]; then
        # Older SuSE/etc.
        . /etc/SuSe-release
        OS="SuSe Linux"
        VER=$VERSION
    elif [ -f /etc/redhat-release ]; then
        # Older Red Hat, CentOS, etc.
        arr=( $(cat /etc/redhat-release) )
        for i in ${arr[@]}; do
            if [ $i == "release" ]; then
                OS=$LP
                LP=""
                continue
            fi
            LP="$LP $i"
        done
        VER=$LP
    else
        # Fall back to uname, e.g. "Linux <version>"
        OS=$(uname -s)
        VER=$(uname -r)
    fi

elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="Mac OS"
    OS=$(sw_vers -productName)
    VER=$(sw_vers -productVersion)

elif [[ "$OSTYPE" == "cygwin" ]]; then
    # POSIX compatibility layer and Linux environment emulation for Windows
    PLATFORM="Windows NT"
    if type cmd /c ver >/dev/null 2>&1; then
        OS=$(cmd /c ver)
    else
        # Fall back to uname
        OS=$(uname -s)
        VER=$(uname -r)
    fi

elif [[ "$OSTYPE" == "msys" ]]; then
    # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
    PLATFORM="Windows NT"
    OS=$(systeminfo | sed -n 's/^OS Name:[[:blank:]]*//p')
    VER=$(systeminfo | sed -n 's/^OS Version:[[:blank:]]*//p')

elif [[ "$OSTYPE" == "freebsd"* ]]; then
    # BSD Unix
    PLATFORM="FreeBSD"
    OS=$(uname -s)
    VER=$(uname -r)
else
    # Unrecognized platform, fall back to uname if exists
    PLATFORM="Unrecognized"
    if type uname >/dev/null 2>&1; then
        OS=$(uname -s)
        VER=$(uname -r)
    fi
fi

# Fetch template
INC="inc"
TMP="build.inc.h"
OUT="build.h"
tmp=$(cat $INC/$TMP)

# Output result
echo "$tmp

#define PLATFORM \"$PLATFORM/$OS/$VER\"
#define BUILD_TIME \"$(date)\"

#endif" > $INC/$OUT
