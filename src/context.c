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

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"

/**
 * Opaque implementation allows us to avoid potential issues with methods
 * using our internal state in an invalid fashion, and will also allow us
 * to enforce const usage.
 */
struct UscContext {
        char *prefix; /**< For now just '/', but maybe --root option in future */

        unsigned int flags; /**<A bitwise set of flags specified for the context */
};

UscContext *usc_context_new(const char *prefix)
{
        UscContext *ret = NULL;

        ret = calloc(1, sizeof(UscContext));
        if (!ret) {
                return NULL;
        }
        ret->prefix = realpath(prefix, NULL);
        if (!ret->prefix) {
                fprintf(stderr, "Unable to determine root: %s (%s)\n", prefix, strerror(errno));
                usc_context_free(ret);
                return NULL;
        }
        return ret;
}

void usc_context_free(UscContext *self)
{
        if (!self) {
                return;
        }
        free(self->prefix);
}

const char *usc_context_get_prefix(UscContext *self)
{
        if (!self) {
                return NULL;
        }
        return (const char *)self->prefix;
}

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
