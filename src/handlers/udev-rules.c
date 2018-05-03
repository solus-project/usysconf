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

static const char *udev_rules_paths[] = {
        "/usr/lib/udev/rules.d",
        "/lib/udev/rules.d",
        "/etc/udev/rules.d",
};

/**
 * Reload the udev rules and then trigger rebuild of udev devices
 *
 * Some of the udev rules being loaded may be composite in nature with
 * include directories, so we cannot rely on inotify alone. Instead we'll
 * manually apply the rules when one of the source directories change,
 * and then ask udev to rebuild and apply the rules to existing udev devices
 * so that updates can automatically "fix" udev rule issues.
 */
static UscHandlerStatus usc_handler_udev_rules_exec(UscContext *ctx, const char *path)
{
        const char *command_reload[] = {
                "/usr/bin/udevadm",
                "control",
                "--reload-rules", /* Reload rules */
                NULL,             /* Terminator */
        };
        const char *command_trigger[] = {
                "/usr/bin/udevadm",
                "trigger", /* Rebuild rules and apply them */
                NULL,      /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        /* First up, reload the rules */
        usc_context_emit_task_start(ctx, "Reload udev rules");
        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED)) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_SKIP);
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        int ret = usc_exec_command((char **)command_reload);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Now trigger the rules */
        usc_context_emit_task_start(ctx, "Apply udev rules");
        ret = usc_exec_command((char **)command_trigger);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_udev_rules = {
        .name = "udev-rules",
        .description = "Reload udev rules",
        .required_bin = "/usr/bin/udevadm",
        .exec = usc_handler_udev_rules_exec,
        .paths = udev_rules_paths,
        .n_paths = ARRAY_SIZE(udev_rules_paths),
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
