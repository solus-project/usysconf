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

#include "config.h"
#include "context.h"
#include "files.h"
#include "util.h"

static const char *hwdb_paths[] = {
        "/usr/lib/udev/hwdb.d",
        "/etc/udev/hwdb.d",
};

/**
 * Update the hwdb caches
 *
 * This generates the .bin cache file from the hwdb.d directories
 * and effectively compiles to a single cache
 */
static UscHandlerStatus usc_handler_hwdb_exec(UscContext *ctx, const char *path)
{
        const char *command[] = {
#ifdef HAVE_SYSTEMD
                "/usr/bin/systemd-hwdb",
                "update", /* Update hwdb cache */
#else
                "/usr/bin/udevadm",
                "hwdb",
                "--update", /* Legacy invocation */
#endif
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        usc_context_emit_task_start(ctx, "Updating hwdb");
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_hwdb = {
        .name = "hwdb",
        .description = "Update hardware database",
#ifdef HAVE_SYSTEMD
        .required_bin = "/usr/bin/systemd-hwdb",
#else
        .required_bin = "/usr/bin/udevadm",
#endif
        .exec = usc_handler_hwdb_exec,
        .paths = hwdb_paths,
        .n_paths = ARRAY_SIZE(hwdb_paths),
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
