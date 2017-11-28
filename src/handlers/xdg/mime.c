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
        "/usr/share/mime/*",
};

/**
 * Update the mime database.
 *
 * Due to the way in which we have to match anything under the mime tree,
 * i.e. because other things own the directories, we may end up with a
 * case of trying to re-update the mime due to our own timestamps being
 * slightly off.
 *
 * This is OK however as we pass -n which will bypass unnecessary updates
 * to the cache.
 */
static UscHandlerStatus usc_handler_mime_exec(UscContext *ctx, const char *path)
{
        char *command[] = {
                "/usr/bin/update-mime-database",
                "-n",
                NULL, /* /usr/share/mime */
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        /* Locked for now due to only using our own root */
        command[2] = "/usr/share/mime";

        usc_context_emit_task_start(ctx, "Updating mimetype database");
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_mime = {
        .name = "mime",
        .exec = usc_handler_mime_exec,
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
