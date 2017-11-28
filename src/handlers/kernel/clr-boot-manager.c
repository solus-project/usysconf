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

/**
 * Paths that trigger a clr-boot-manager update
 */
static const char *boot_paths[] = {
        KERNEL_DIR,
        "/usr/lib/goofiboot",
        "/usr/lib/systemd/boot/efi",
};

/**
 * Trigger `clr-boot-manager update` when one of the boot-relevant components
 * has been altered within the upstream delivery mechanism.
 */
static UscHandlerStatus usc_handler_cbm_exec(UscContext *ctx, const char *path)
{
        char *command[] = {
                "/usr/bin/clr-boot-manager", "update", NULL, /* Terminator */
        };

        /* TODO: Support --path updates */
        if (!usc_file_is_dir(path) && access(path, X_OK) != 0) {
                return USC_HANDLER_SKIP;
        }

        usc_context_emit_task_start(ctx, "Updating clr-boot-manager");
        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_cbm = {
        .name = "boot",
        .description = "Update boot configuration + kernels",
        .required_bin = "/usr/bin/clr-boot-manager",
        .exec = usc_handler_cbm_exec,
        .paths = boot_paths,
        .n_paths = ARRAY_SIZE(boot_paths),
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
