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
#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include "context.h"
#include "files.h"
#include "util.h"

static const char *icon_cache_paths[] = {
        "/usr/share/icons",
};

DEF_AUTOFREE(DIR, closedir)

/**
 * Update the icon cache on disk for every icon them found in the given directory
 */
static UscHandlerStatus usc_handler_icon_cache_exec(UscContext *ctx, const char *path,
                                                    const char *full_path)
{
        autofree(DIR) *dir = NULL;
        autofree(char) *gtk_bin = NULL;
        const char *prefix = NULL;
        struct dirent *ent = NULL;
        char *command[] = {
                NULL, /* /usr/bin/gtk-update-icon-cache */
                "-ft",
                NULL, /* Path */
                NULL, /* Terminator */
        };

        prefix = usc_context_get_prefix(ctx);
        if (asprintf(&gtk_bin, "%s/usr/bin/gtk-update-icon-cache", prefix) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        command[0] = gtk_bin;

        dir = opendir(full_path);
        if (!dir) {
                fprintf(stderr, "Failed to open dir: %s (%s)\n", path, strerror(errno));
                return USC_HANDLER_FAIL;
        }

        while ((ent = readdir(dir)) != NULL) {
                autofree(char) *fp = NULL;
                char *dirn = NULL;

                /* Skip '.' and '..' entries */
                if (strncmp(ent->d_name, ".", 1) == 0 || strncasecmp(ent->d_name, "..", 2) == 0) {
                        continue;
                }

                if (asprintf(&fp, "%s/%s/index.theme", full_path, ent->d_name) < 0) {
                        fputs("OOM\n", stderr);
                        return USC_HANDLER_FAIL;
                }

                /* Require an index theme. */
                if (!usc_file_exists(fp)) {
                        continue;
                }

                /* Grab the dirname again now we know the index theme exists */
                dirn = dirname(fp);
                if (!dirn) {
                        continue;
                }

                command[2] = dirn;
                fprintf(stderr, "Checking %s\n", dirn);
                int ret = usc_exec_command(command);
                if (ret != 0) {
                        fprintf(stderr, "Ohnoes\n");
                }
        }

        /* We do nothing *yet* */
        return USC_HANDLER_SUCCESS;
}

const UscHandler usc_handler_icon_cache = {
        .name = "icon caches",
        .exec = usc_handler_icon_cache_exec,
        .paths = icon_cache_paths,
        .n_paths = ARRAY_SIZE(icon_cache_paths),
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
