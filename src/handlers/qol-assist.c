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
#include <stdio.h>
#include <string.h>

#include "context.h"
#include "files.h"
#include "util.h"

/**
 * Paths that trigger a qol-assist update
 */
static const char *qol_paths[] = {
        "/usr/sbin/qol-assist",
};

/**
 * Trigger 'qol-assist' to run on the next boot.
 *
 * qol-assist will be systemd started when the trigger file exists. Running
 * the trigger subcommand will enforce creation of the trigger to ensure
 * qol-assist runs.
 *
 * This will follow a versioned migration, and will happily ignore any
 * migrations that already happened.
 */
static UscHandlerStatus usc_handler_qol_assist_exec(UscContext *ctx, const char *path)
{
        char *command[] = {
                "/usr/sbin/qol-assist",
                "trigger", /* Trigger an update on boot */
                NULL,      /* Terminator */
        };

        if (access(path, X_OK) != 0) {
                return USC_HANDLER_SKIP;
        }

        /* QoL migrations only make sense for real live systems */
        usc_context_emit_task_start(ctx, "Registering QoL migration on next boot");
        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED)) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_SKIP);
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_qol_assist = {
        .name = "qol-assist",
        .description = "Register QoL migration",
        .required_bin = "/usr/sbin/qol-assist",
        .exec = usc_handler_qol_assist_exec,
        .paths = qol_paths,
        .n_paths = ARRAY_SIZE(qol_paths),
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
