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

#include "context.h"
#include "files.h"
#include "util.h"

static const char *sshd_paths[] = {
        "/usr/sbin/sshd",
        "/sbin/sshd",
};

#define RSA_HOST_KEY "/etc/ssh/ssh_host_rsa_key"
#define DSA_HOST_KEY "/etc/ssh/ssh_host_dsa_key"

/**
 * Handle correct bootstrap of sshd
 */
static UscHandlerStatus usc_handler_sshd_exec(UscContext *ctx, const char *path)
{
        const char *command[] = {
                "/usr/bin/ssh-keygen",
                "-q",
                "-t",
                "rsa",
                "-f",
                RSA_HOST_KEY, /* Build the RSA host key. */
                "-C",
                "",
                "-N",
                "",
                NULL, /* Terminator */
        };

        /* We expect to be looking at the executable sshd here. */
        if (access(path, X_OK) != 0) {
                return USC_HANDLER_SKIP;
        }

        /* Host key is configured, let's bail. */
        if (usc_file_exists(RSA_HOST_KEY) || usc_file_exists(DSA_HOST_KEY)) {
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        usc_context_emit_task_start(ctx, "Creating OpenSSH host key");
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_sshd = {
        .name = "openssh",
        .description = "Create OpenSSH host key",
        .required_bin = "/usr/bin/ssh-keygen",
        .exec = usc_handler_sshd_exec,
        .paths = sshd_paths,
        .n_paths = ARRAY_SIZE(sshd_paths),
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
