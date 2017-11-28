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

static const char *schema_paths[] = {
        "/usr/share/glib-2.0/schemas",
};

/**
 * Compile glib schemas whenever the directory is updated
 */
static UscHandlerStatus usc_handler_glib2_exec(__usc_unused__ UscContext *ctx, const char *path)
{
        autofree(char) *fp = NULL;
        char *command[] = {
                "/usr/bin/glib-compile-schemas",
                NULL, /* /usr/share/glib-2.0/schemas */
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        command[1] = (char *)path,

        usc_context_emit_task_start(ctx, "Compiling glib-schemas");
        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_glib2 = {
        .name = "glib2",
        .description = "Compile glib-schemas",
        .exec = usc_handler_glib2_exec,
        .paths = schema_paths,
        .n_paths = ARRAY_SIZE(schema_paths),
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
