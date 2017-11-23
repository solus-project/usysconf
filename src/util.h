/*
 * This file is part of usysconf.
 *
 * Copyright © 2017 Solus Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#define _GNU_SOURCE

#include <stdlib.h>
#include <sys/types.h>

/**
 * Fork and execute a command, waiting for it to complete.
 *
 * @returns Status code of the command, if it executed.
 */
int usc_exec_command(char **command);

/**
 * Grab the (0-based) indexed path component from the string
 * and return a copy of it
 *
 * @note Will return NULL if the component is out of range
 *
 * @returns Duplicated string with the indexed path component
 */
char *usc_get_strn_component(const char *inp_path, ssize_t whence);

/**
 * Taken out of libnica and various other Solus projects
 */
#define DEF_AUTOFREE(N, C)                                                                         \
        static inline void _autofree_func_##N(void *p)                                             \
        {                                                                                          \
                if (p && *(N **)p) {                                                               \
                        C(*(N **)p);                                                               \
                        (*(void **)p) = NULL;                                                      \
                }                                                                                  \
        }

#define autofree(N) __attribute__((cleanup(_autofree_func_##N))) N

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

#define __usc_unused__ __attribute__((unused))

DEF_AUTOFREE(char, free)

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 expandtab:
 * :indentSize=8:tabSize=8:noTabs=true:
 */
