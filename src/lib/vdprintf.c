/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

// include header
#include <serverX.h>

#define BUF_SIZE 512

int vdprintf(int d, const char *format, va_list ap)
{
	char buf[BUF_SIZE];
	int len;

	len = vsnprintf(buf, BUF_SIZE, format, ap);
	return write(d, buf, len);
}
