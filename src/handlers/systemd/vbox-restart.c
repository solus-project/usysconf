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

#include "config.h"
#include "context.h"
#include "files.h"
#include "util.h"

static const char *unit_paths[] = {
        SYSTEMD_UNIT_DIR "/vboxdrv.service", /* Main systemd unit */
        "/usr/bin/VirtualBox",               /* VBox.sh */
};

/**
 * Reload systemd if the units change on disk
 *
 */
static UscHandlerStatus usc_handler_vbox_restart_exec(UscContext *ctx,
                                                      __usc_unused__ const char *path)
{
        const char *command[] = {
                "/usr/bin/systemctl", "restart", "vboxdrv.service", NULL, /* Terminator */
        };

        usc_context_emit_task_start(ctx, "Restarting VirtualBox services");
        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED) ||
            usc_context_has_flag(ctx, USC_FLAGS_LIVE_MEDIUM)) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_SKIP);
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }

        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_vbox_restart = {
        .name = "vbox-restart",
        .description = "Restart VirtualBox services",
        .required_bin = "/usr/bin/VBox.sh",
        .exec = usc_handler_vbox_restart_exec,
        .paths = unit_paths,
        .n_paths = ARRAY_SIZE(unit_paths),
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
