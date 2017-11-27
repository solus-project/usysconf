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
        SYSTEMD_UTIL_DIR "/systemd" /* /usr/lib/systemd/systemd */
};

/**
 * Ask systemd to reexec when the binary has been updated
 *
 */
static UscHandlerStatus usc_handler_systemd_reexec_exec(UscContext *ctx, const char *path)
{
        const char *command[] = {
                "/usr/bin/systemctl", "daemon-reexec", NULL, /* Terminator */
        };

        if (access(path, X_OK) != 0) {
                return USC_HANDLER_SKIP;
        }

        if (usc_context_has_flag(ctx, USC_FLAGS_CHROOTED)) {
                fputs("Skipping reexec due to chroot environment\n", stderr);
                return USC_HANDLER_SKIP | USC_HANDLER_BREAK;
        }

        fprintf(stderr, "Re-executing systemd due to change on: %s\n", path);
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                fprintf(stderr, "Ohnoes\n");
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_systemd_reexec = {
        .name = "systemd_reexec",
        .exec = usc_handler_systemd_reexec_exec,
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
