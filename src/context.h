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

#include "util.h"

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
 * A UscContext is an opaque object passed around to our trigger functions
 * to organise state.
 */
typedef struct UscContext UscContext;

/**
 * Each execution handler can return one of 3 return codes only.
 */
typedef enum {
        USC_HANDLER_MIN = 0,
        USC_HANDLER_SUCCESS, /**<We successfully updated a thing */
        USC_HANDLER_FAIL,    /**<We failed to update a thing */
        USC_HANDLER_SKIP,    /**<Skipped execution, don't update records */
        USC_HANDLER_BREAK,   /**<Do not process further paths */
        USC_HANDLER_MAX,
} UscHandlerStatus;

/**
 * Standard context execution function
 *
 * @param context Pointer to a valid UscContext instance
 * @param path Relative path for analysis without the root
 * @param full_path The complete path to the root
 *
 * @returns True if the execution handler succeeded
 */
typedef UscHandlerStatus (*usc_context_func)(UscContext *context, const char *path,
                                             const char *full_path);

/**
 * UscHandler is currently just demo stuff and we'll flesh it out in time
 * to support multiple paths. For now we're just interested in getting stuff
 * off the ground.
 */
typedef struct UscHandler {
        const char **paths;    /**<Path we register interest for */
        const char *name;      /**<Name for this handler */
        usc_context_func exec; /**<Execution handler */
        size_t n_paths;        /**< Number of known paths */
} UscHandler;

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

/**
 * Return the prefix for this UscContext.
 *
 * @note This string is owned by the context and should not be freed by callers.
 *
 * @param context Pointer to an allocated UscContext instance
 * @returns Pointer to the prefix
 */
const char *usc_context_get_prefix(UscContext *context);

/**
 * Determine if the context has a given flag set
 *
 * @param context Pointer to an allocated UscContext instance
 * @param flag Flag to check for membership
 *
 * @returns True if the flag is set
 */
bool usc_context_has_flag(UscContext *context, unsigned int flag);

DEF_AUTOFREE(UscContext, usc_context_free)

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
