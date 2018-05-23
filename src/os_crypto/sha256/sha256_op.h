/*
 * Contributed by Arshad Khan (@arshad01)
 * Maintained by Wazuh Inc.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#ifndef __SHA256_OP_H
#define __SHA256_OP_H

#include <sys/types.h>

typedef char os_sha256[65];

int OS_SHA256_File(const char *fname, os_sha256 output, int mode) __attribute((nonnull));

#endif
