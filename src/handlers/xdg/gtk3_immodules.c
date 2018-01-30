/*
 * This file is part of usysconf.
 *
 * Copyright © 2017-2018 Solus Project
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

static const char *module_paths[] = {
        "/usr/lib/gtk-3.0/3.*/immodules",
};

/**
 * Update immodules cache for GTK3
 */
static UscHandlerStatus usc_handler_gtk3_immodules_exec(UscContext *ctx, const char *path)
{
        char *command[] = {
                "/usr/bin/gtk-query-immodules-3.0",
                "--update-cache", /* Update cache directly */
                NULL,             /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        usc_context_emit_task_start(ctx, "Updating GTK3 input module cache");
        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_gtk3_immodules = {
        .name = "gtk3-immodules",
        .description = "Update GTK3 input module cache",
        .required_bin = "/usr/bin/gtk-query-immodules-3.0",
        .exec = usc_handler_gtk3_immodules_exec,
        .paths = module_paths,
        .n_paths = ARRAY_SIZE(module_paths),
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
