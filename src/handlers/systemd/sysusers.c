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

static const char *sysuser_paths[] = {
        "/usr/lib/sysusers.d",
        /* /etc/sysusers.d */
};

/**
 * Create systemd sysusers
 *
 * If an update delivers changes to /usr/lib/sysusers.d, tell
 * systemd-sysusers to go do something with that.
 */
static UscHandlerStatus usc_handler_sysusers_exec(__usc_unused__ UscContext *ctx, const char *path)
{
        const char *command[] = {
                "/usr/bin/systemd-sysusers",
                "--root=/", /* Ensure no tom-foolery with dbus */
                NULL,       /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        fprintf(stderr, "Updating sysusers for %s\n", path);
        int ret = usc_exec_command((char **)command);
        if (ret != 0) {
                fprintf(stderr, "Ohnoes\n");
                return USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        }
        /* Only want to run once for all of our globs */
        return USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
}

const UscHandler usc_handler_sysusers = {
        .name = "sysusers",
        .exec = usc_handler_sysusers_exec,
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
