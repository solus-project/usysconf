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
#include "files.h"

/**
 * Opaque implementation allows us to avoid potential issues with methods
 * using our internal state in an invalid fashion, and will also allow us
 * to enforce const usage.
 */
struct UscContext {
        unsigned int flags; /**<A bitwise set of flags specified for the context */
};

UscContext *usc_context_new()
{
        UscContext *ret = NULL;

        ret = calloc(1, sizeof(UscContext));
        if (!ret) {
                return NULL;
        }

        if (usc_is_chrooted()) {
                ret->flags |= USC_FLAGS_CHROOTED;
        }

        return ret;
}

void usc_context_free(UscContext *self)
{
        if (!self) {
                return;
        }
        free(self);
}

bool usc_context_has_flag(UscContext *self, unsigned int flag)
{
        if (!self) {
                return false;
        }
        if ((self->flags & flag) == flag) {
                return true;
        }
        return false;
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
