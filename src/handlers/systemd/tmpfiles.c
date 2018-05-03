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

#include "config.h"
#include "context.h"
#include "files.h"
#include "util.h"

static const char *sysuser_paths[] = {
        SYSTEMD_TMPFILES_DIR,
};

/**
 * Create systemd tmpfiles
 *
 * If an update delivers changes to /usr/lib/tmpfiles.d, tell
 * systemd-tmpfiles to go do something with that.
 */
static UscHandlerStatus usc_handler_tmpfiles_exec(UscContext *ctx, const char *path)
{
        const char *command[] = {
                "/usr/bin/systemd-tmpfiles",
                "--root=/", /* Ensure no tom-foolery with dbus */
                "--create", /* Create tmpfiles */
                NULL,       /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        usc_context_emit_task_start(ctx, "Updating systemd tmpfiles");
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_tmpfiles = {
        .name = "tmpfiles",
        .description = "Update systemd tmpfiles",
        .required_bin = "/usr/bin/systemd-tmpfiles",
        .exec = usc_handler_tmpfiles_exec,
        .paths = sysuser_paths,
        .n_paths = ARRAY_SIZE(sysuser_paths),
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
