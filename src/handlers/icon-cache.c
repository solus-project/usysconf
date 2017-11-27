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
        "/usr/share/icons/*",
};

DEF_AUTOFREE(DIR, closedir)

static bool is_orphan_icon_dir(const char *directory)
{
        int n_items = 0;
        autofree(DIR) *dir = NULL;
        struct dirent *ent = NULL;

        dir = opendir(directory);
        if (!dir) {
                return false;
        }
        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }
                ++n_items;
                if (strcmp(ent->d_name, "icon-theme.cache") != 0) {
                        return false;
                }
        }
        /* Must have had exactly one file, the icon-theme.cache */
        return n_items <= 1;
}

/**
 * Helper to nuke the one file and it's parent directory.
 *
 * TODO: Just add a new rm_rf style command.
 */
static int rmdir_and(const char *dir, const char *file)
{
        autofree(char) *fp = NULL;
        int ret = 0;

        ret = asprintf(&fp, "%s/%s", dir, file);
        if (ret < 0) {
                return ret;
        }
        if (usc_file_exists(fp)) {
                ret = unlink(fp);
                if (ret < 0) {
                        return ret;
                }
        }
        return rmdir(dir);
}

/**
 * Update the icon cache on disk for every icon them found in the given directory
 */
static UscHandlerStatus usc_handler_icon_cache_exec(__usc_unused__ UscContext *ctx,
                                                    const char *path)
{
        autofree(char) *fp = NULL;
        char *command[] = {
                "/usr/bin/gtk-update-icon-cache",
                "-ftq",
                NULL, /* Path */
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        if (is_orphan_icon_dir(path)) {
                if (rmdir_and(path, "icon-theme.cache") != 0) {
                        fprintf(stderr, "Failed to remove '%s': %s\n", path, strerror(errno));
                        return USC_HANDLER_FAIL;
                }
                fprintf(stderr, "Removed orphan icon theme directory: %s\n", path);
                return USC_HANDLER_SUCCESS | USC_HANDLER_DROP;
        }

        if (asprintf(&fp, "%s/index.theme", path) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        /* Require an index theme. */
        if (!usc_file_exists(fp)) {
                return USC_HANDLER_SKIP;
        }

        command[2] = (char *)path;
        fprintf(stderr, "Checking %s\n", path);
        int ret = usc_exec_command(command);
        if (ret != 0) {
                fprintf(stderr, "Ohnoes\n");
                return USC_HANDLER_FAIL;
        }
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
