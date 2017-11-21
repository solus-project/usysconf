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
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "files.h"
#include "handlers.h"
#include "state.h"

/* Table of supported handlers */
static const UscHandler *usc_handlers[] = {
        &usc_handler_ldconfig, /**<Get library cache in order first */
        &usc_handler_cbm,      /**<Now CBM can successfully run */
        &usc_handler_depmod,   /**< Run depmod after cbm does its thing */

        /** Middleware */
        &usc_handler_hwdb, /**<Want hwdb updated before calling LDM (PCI) */
        &usc_handler_ldm,  /**<Update drivers/GL-links/etc */

        /* Very likely that LDM caused a cache invalidation for lib dirs */
        &usc_handler_ldconfig,

        &usc_handler_sysusers,
        &usc_handler_tmpfiles,

        /** Enter userspace. */
        &usc_handler_glib2,
        &usc_handler_fonts,
        &usc_handler_mime,
        &usc_handler_icon_cache,
        &usc_handler_desktop_files,

        /* Special cases */
        &usc_handler_mandb,
        &usc_handler_ssl_certs,
};

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

static void usc_handle_one(const UscHandler *handler, UscContext *context, UscStateTracker *tracker)
{
        UscHandlerStatus status = USC_HANDLER_MIN;
        bool record_remain = false;

        for (size_t i = 0; i < handler->n_paths; i++) {
                glob_t glo = { 0 };
                const char *path = NULL;

                path = handler->paths[i];

                if (glob(path, GLOB_NOSORT, NULL, &glo) != 0) {
                        continue;
                }

                for (size_t i = 0; i < glo.gl_pathc; i++) {
                        char *resolved = glo.gl_pathv[i];
                        bool record_path = false;

                        /* Don't try to do anything for the remainder of this glob. */
                        if (record_remain) {
                                goto push_entry;
                        }

                        /* Do we need to handle this dude ? */
                        if (!usc_state_tracker_needs_update(tracker, resolved)) {
                                continue;
                        }

                        status = handler->exec(context, resolved);

                        if ((status & USC_HANDLER_SUCCESS) == USC_HANDLER_SUCCESS) {
                                fprintf(stderr, "Success: %s\n", handler->name);
                                record_path = true;
                        }
                        if ((status & USC_HANDLER_FAIL) == USC_HANDLER_FAIL) {
                                fputs("Failed\n", stderr);
                                continue;
                        }
                        if ((status & USC_HANDLER_SKIP) == USC_HANDLER_SKIP) {
                                fprintf(stderr, "Skipping: %s\n", handler->name);
                                continue;
                        }

                        if ((status & USC_HANDLER_BREAK) == USC_HANDLER_BREAK) {
                                /* TODO: Record all paths as updated */
                                fprintf(stderr, "breaking..\n");
                                record_remain = true;
                        }

                        if (!record_path) {
                                continue;
                        }

                push_entry:
                        /* We won't record the new entry here and we won't write it back out either
                         */
                        if ((status & USC_HANDLER_DROP) == USC_HANDLER_DROP) {
                                continue;
                        }
                        if (!usc_state_tracker_push_path(tracker, resolved)) {
                                fprintf(stderr, "Failed to record path %s\n", resolved);
                        }
                }
                globfree(&glo);
        }
}

bool usc_context_run_triggers(UscContext *context, const char *name)
{
        autofree(UscStateTracker) *tracker = NULL;
        bool ran_trigger = false;

        tracker = usc_state_tracker_new();
        if (!tracker) {
                fputs("Cannot continue without valid UscStateTracker\n", stderr);
                return false;
        }

        /* Crack on regardless. */
        if (!usc_state_tracker_load(tracker)) {
                fputs("Invalid state has been removed\n", stderr);
        }

        if (usc_context_has_flag(context, USC_FLAGS_CHROOTED)) {
                fputs("Chrooted!\n", stdout);
        } else {
                fputs("Not chrooted\n", stdout);
        }

        /* Just test the main loop iteration jank for now */
        for (size_t i = 0; i < ARRAY_SIZE(usc_handlers); i++) {
                if (name && strcmp(usc_handlers[i]->name, name) != 0) {
                        continue;
                }
                usc_handle_one(usc_handlers[i], context, tracker);
                ran_trigger = true;
        }

        if (!ran_trigger) {
                fprintf(stderr, "Unknown trigger '%s'\n", name);
                return false;
        }

        /* Dump it back to disk */
        if (!usc_state_tracker_write(tracker)) {
                fputs("Failed to write state file!\n", stderr);
                return false;
        }

        return true;
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
