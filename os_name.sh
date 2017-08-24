#!/bin/sh

# OS_NAME.sh
# To find the distribution name and version for Linux OS

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
        OS=Debian
        VER=$(cat /etc/debian_version)
    elif [ -f /etc/SuSe-release ]; then
        # Older SuSE/etc.
        ...
    elif [ -f /etc/redhat-release ]; then
        # Older Red Hat, CentOS, etc.
        ...
    else
        # Fall back to uname, e.g. "Linux <version>"
        OS=$(uname -s)
        VER=$(uname -r)
    fi

elif [[ "$OSTYPE" == "darwin"* ]]; then
        # Mac OSX
        ...

elif [[ "$OSTYPE" == "cygwin" ]]; then
    PLATFORM="Windows"
        # POSIX compatibility layer and Linux environment emulation for Windows
        if type cmd /c ver >/dev/null 2>&1; then
            OS=$(cmd /c ver)
        else
            # Fall back to uname
            OS=$(uname -s)
            VER=$(uname -r)
        fi

elif [[ "$OSTYPE" == "msys" ]]; then
    PLATFORM="Windows"
        # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
        OS=$(systeminfo | sed -n 's/^OS Name:[[:blank:]]*//p')
        VER=$(systeminfo | sed -n 's/^OS Version:[[:blank:]]*//p')

elif [[ "$OSTYPE" == "freebsd"* ]]; then
    PLATFORM="FreeBSD"
    OS=$(uname -s)
    VER=$(uname -r)
fi

# Echo result
echo "$PLATFORM/$OS/$VER"
