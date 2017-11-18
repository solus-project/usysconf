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

#include <glob.h>
#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "files.h"
#include "state.h"

/* Implemented elsewhere in the codebase */
extern UscHandler usc_handler_icon_cache;
extern UscHandler usc_handler_ldconfig;

/* Table of supported handlers */
static const UscHandler *usc_handlers[] = { &usc_handler_icon_cache, &usc_handler_ldconfig };

static void usc_handle_one(const UscHandler *handler, UscContext *context, UscStateTracker *tracker)
{
        UscHandlerStatus status = USC_HANDLER_MIN;
        const char *root = NULL;

        root = usc_context_get_prefix(context);

        for (size_t i = 0; i < handler->n_paths; i++) {
                bool record_remain = false;
                glob_t glo = { 0 };
                const char *path = NULL;
                autofree(char) *full_path = NULL;

                path = handler->paths[i];

                if (asprintf(&full_path, "%s/%s", root, path) < 0) {
                        fputs("OOM\n", stderr);
                        abort();
                }

                if (glob(full_path, GLOB_NOSORT | GLOB_BRACE, NULL, &glo) != 0) {
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

                        status = handler->exec(context, path, resolved);

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
                        if (!usc_state_tracker_push_path(tracker, resolved)) {
                                fprintf(stderr, "Failed to record path %s\n", resolved);
                        }
                }
                globfree(&glo);
        }
}

int main(__usc_unused__ int argc, __usc_unused__ char **argv)
{
        autofree(UscContext) *context = NULL;
        autofree(UscStateTracker) *tracker = NULL;

        context = usc_context_new("/");
        if (!context) {
                fputs("Cannot continue without valid UscContext\n", stderr);
                return EXIT_FAILURE;
        }
        tracker = usc_state_tracker_new(context);
        if (!tracker) {
                fputs("Cannot continue without valid UscStateTracker\n", stderr);
                return EXIT_FAILURE;
        }

        if (usc_context_has_flag(context, USC_FLAGS_CHROOTED)) {
                fputs("Chrooted!\n", stdout);
        } else {
                fputs("Not chrooted\n", stdout);
        }

        /* Just test the main loop iteration jank for now */
        for (size_t i = 0; i < ARRAY_SIZE(usc_handlers); i++) {
                usc_handle_one(usc_handlers[i], context, tracker);
        }

        /* Dump it back to disk */
        if (!usc_state_tracker_write(tracker)) {
                fputs("Failed to write state file!\n", stderr);
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
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
