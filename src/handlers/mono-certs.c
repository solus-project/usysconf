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

static const char *mono_certs_paths[] = {
        "/etc/ssl/certs/ca-certificates.crt",
};

/**
 * Populate the Mono certificates systemwide so Mono applications are ready to
 * be used out of the box.
 */
static UscHandlerStatus usc_handler_mono_certs_exec(UscContext *ctx,
                                                    __usc_unused__ const char *path)
{
        char *command[] = {
                "/usr/bin/cert-sync",
                "--quiet", /* keep it quiet */
                "/etc/ssl/certs/ca-certificates.crt",
                NULL, /* Terminator */
        };

        usc_context_emit_task_start(ctx, "Populating Mono certificates");
        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED)) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_SKIP);
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Only run once */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_mono_certs = {
        .name = "mono-certs",
        .description = "Populate Mono certificates",
        .required_bin = "/usr/bin/cert-sync",
        .exec = usc_handler_mono_certs_exec,
        .paths = mono_certs_paths,
        .n_paths = ARRAY_SIZE(mono_certs_paths),
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
