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

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "context.h"
#include "files.h"
#include "util.h"

static const char *font_paths[] = {
        "/usr/share/fonts/*/*",
        "/usr/share/fonts/*",
};

/**
 * Update the font cache on disk.
 *
 * TODO: Track cache invalidations whereby an existing glob that we once
 * tracked has disappeared so we can rebuild the whole cache, then we'll
 * be able to do per directory fc-cache updates to make updates quicker.
 */
static UscHandlerStatus usc_handler_fonts_exec(UscContext *ctx, const char *path)
{
        /* TODO: Maybe try again with '-r' if this fails */
        const char *command[] = {
                "/usr/bin/fc-cache",
                "-f", /* Force */
                "-s", /* System only */
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        usc_context_emit_task_start(ctx, "Rebuilding font cache");
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_fonts = {
        .name = "fonts",
        .description = "Rebuild font cache",
        .exec = usc_handler_fonts_exec,
        .paths = font_paths,
        .n_paths = ARRAY_SIZE(font_paths),
};

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
