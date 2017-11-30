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

static const char *app_paths[] = {
        "/usr/share/applications",
};

/**
 * Update the desktop database
 *
 * This simply runs when /usr/share/applications is modified, building the
 * new "mimeinfo.cache" file. Note this is anti-statelesss.
 */
static UscHandlerStatus usc_handler_desktop_files_exec(UscContext *ctx, const char *path)
{
        char *command[] = {
                "/usr/bin/update-desktop-database",
                "-q", /* Quiet */
                NULL, /* /usr/share/applications */
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        command[2] = (char *)path;

        usc_context_emit_task_start(ctx, "Updating desktop database");
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_desktop_files = {
        .name = "desktop-files",
        .description = "Update desktop database",
        .required_bin = "/usr/bin/update-desktop-database",
        .exec = usc_handler_desktop_files_exec,
        .paths = app_paths,
        .n_paths = ARRAY_SIZE(app_paths),
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
