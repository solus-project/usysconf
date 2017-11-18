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

#include <stdbool.h>

/**
 * Bitwise flags for tracking the supported operations and state of a UscContext
 */
typedef enum {
        USC_FLAGS_MIN = 1 << 0,
        USC_FLAGS_CHROOTED = 1 << 1,
        USC_FLAGS_FORCE_CHROOT = 1 << 2,
        USC_FLAGS_MAX,
} UscFlags;

/**
 * A UscContext is a transparent object passed around to our trigger functions
 * to organise state
 */
typedef struct UscContext {
        char *prefix; /**< For now just '/', but maybe --root option in future */

        unsigned int flags; /**<A bitwise set of flags specified for the context */
} UscContext;

/**
 * Construct a newly allocated UscContext for the given prefix
 *
 * @param prefix Valid path for all root prefix operations (i.e. '/')
 *
 * @returns Newly allocated UscContext. Must be freed with usc_context_free
 */
UscContext *usc_context_new(const char *prefix);

/**
 * Free an existing UscContext any associated resources
 *
 * @param context Pointer to an allocated UscContext instance
 */
void usc_context_free(UscContext *context);

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
