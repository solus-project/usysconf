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

#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "files.h"

/**
 * UscHandler is currently just demo stuff and we'll flesh it out in time
 * to support multiple paths. For now we're just interested in getting stuff
 * off the ground.
 */
typedef struct UscHandler {
        const char *path;      /**<Path we register interest for */
        const char *name;      /**<Name for this handler */
        usc_context_func exec; /**<Execution handler */
} UscHandler;

/* DEMO */
static UscHandlerStatus demo1(__usc_unused__ UscContext *ctx, const char *p, const char *fp)
{
        fprintf(stderr, "Checking: %s (%s)\n", p, fp);
        return USC_HANDLER_SUCCESS;
}

static UscHandlerStatus demo2(UscContext *ctx, const char *p, const char *fp)
{
        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED)) {
                fputs("debug: skipping chrooted call\n", stderr);
                return USC_HANDLER_SKIP;
        }
        fprintf(stderr, "Checking: %s (%s) from %s\n", p, fp, usc_context_get_prefix(ctx));
        return USC_HANDLER_SUCCESS;
}

static UscHandler usc_handlers[] = {
        { "/usr/share/icons", "Icon caches", demo1 },
        { "/usr/lib/systemd/system", "Systemd units", demo2 },
};

/**
 * Will be used to actually check if the path is updated, by consulting
 * the mtime of the tree against our known mtime.
 *
 * For now, let's cheat, because live stream.
 */
static bool usc_path_updated(__usc_unused__ const char *path)
{
        time_t sys_updated = 0;
        if (!usc_file_mtime(path, &sys_updated)) {
                return false;
        }
        /* TODO: Something useful with the mtime. */
        fprintf(stderr, "%s mtime: %ld\n", path, sys_updated);
        return true;
}

int main(__usc_unused__ int argc, __usc_unused__ char **argv)
{
        autofree(UscContext) *context = NULL;

        context = usc_context_new("/");
        if (!context) {
                fputs("Cannot continue without valid UscContext\n", stderr);
                return EXIT_FAILURE;
        }

        if (usc_context_has_flag(context, USC_FLAGS_CHROOTED)) {
                fputs("Chrooted!\n", stdout);
        } else {
                fputs("Not chrooted\n", stdout);
        }

        /* Just test the main loop iteration jank for now */
        for (size_t i = 0; i < ARRAY_SIZE(usc_handlers); i++) {
                UscHandlerStatus status = USC_HANDLER_MIN;
                const UscHandler *handler = &usc_handlers[i];
                /* Do we need to handle this dude ? */
                if (!usc_path_updated(handler->path)) {
                        continue;
                }

                status = handler->exec(context, handler->path, handler->path);
                switch (status) {
                case USC_HANDLER_FAIL:
                        /* Update record */
                        fputs("Failed\n", stderr);
                        break;
                case USC_HANDLER_SUCCESS:
                        /* Update record */
                        fputs("Success\n", stderr);
                        break;
                case USC_HANDLER_SKIP:
                        /* Can't run right now, so don't update mtimes */
                        fprintf(stderr, "Skipping: %s\n", handler->name);
                        break;
                default:
                        /* You done goofed */
                        fprintf(stderr, "Invalid return! %s\n", handler->name);
                        break;
                }
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
