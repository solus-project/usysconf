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

static const char *apparmor_paths[] = {
        "/etc/apparmor.d",
};

/**
 * Update the apparmor cache within the kernel directory to ensure new apparmors
 * are readily available.
 */
static UscHandlerStatus usc_handler_apparmor_exec(UscContext *ctx, __usc_unused__ const char *path)
{
        char *compile_command[] = {
                "/usr/sbin/aa-lsm-hook-compile", NULL, /* Terminator */
        };
        char *load_command[] = {
                "/usr/sbin/aa-lsm-hook-load", NULL, /* Terminator */
        };

        usc_context_emit_task_start(ctx, "Compiling AppArmor profiles");
        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED)) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_SKIP);
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        /* Compile */
        int ret = usc_exec_command(compile_command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Load */
        usc_context_emit_task_start(ctx, "Reloading AppArmor profiles");
        ret = usc_exec_command(load_command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Only run once */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_apparmor = {
        .name = "apparmor",
        .description = "Compile AppArmor profiles",
        .required_bin = "/usr/sbin/aa-lsm-hook-compile",
        .exec = usc_handler_apparmor_exec,
        .paths = apparmor_paths,
        .n_paths = ARRAY_SIZE(apparmor_paths),
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
