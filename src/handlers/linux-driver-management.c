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

/**
 * Paths that trigger an LDM update
 */
static const char *driver_paths[] = {
        "/usr/lib/glx-provider",   "/usr/lib/glx-provider/*",       "/usr/lib/nvidia",
        "/usr/lib32/glx-provider", "/usr/lib32/glx-provider/*",     "/usr/lib32/nvidia",
        "/usr/lib/nvidia/modules", "/usr/lib/xorg/modules/drivers", "/usr/share/glvnd/egl_vendor.d",
};

/**
 * Trigger `linux-driver-management configure gpu` when one of the driver relevant
 * components has been altered within the upstream delivery mechanism.
 *
 * This allows automatic configuration of newly installed drivers through the
 * linux-driver-management utility.
 *
 * In the absence of usable hardware configuration data and driver packages,
 * LDM will always fall back to configuring Mesa as the default provider.
 * This ensures that even if /dev & /sys are mounted into a chroot, LDM can
 * still safely be executed as long as the NVIDIA drivers aren't also installed
 * into the chroot (which you should never do from a distribution perspective)
 */
static UscHandlerStatus usc_handler_ldm_exec(UscContext *ctx, const char *path)
{
        char *command[] = {
                "/usr/bin/linux-driver-management",
                "configure", /* Want to configure the drivers */
                "gpu",       /* Specifically the GPU drivers */
                NULL,        /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        usc_context_emit_task_start(ctx, "Updating graphical driver configuration");
        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_ldm = {
        .name = "drivers",
        .description = "Update graphical driver configuration",
        .required_bin = "/usr/bin/linux-driver-management",
        .exec = usc_handler_ldm_exec,
        .paths = driver_paths,
        .n_paths = ARRAY_SIZE(driver_paths),
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
