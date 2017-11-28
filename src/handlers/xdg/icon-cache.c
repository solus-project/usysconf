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
        "/usr/share/icons/hicolor/*/*", /* per app */
        "/usr/share/icons/gnome/*/*",   /* per app */
        "/usr/share/icons/*",
};

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
static UscHandlerStatus usc_handler_icon_cache_exec(UscContext *ctx, const char *path)
{
        autofree(char) *theme_name = NULL;
        autofree(char) *theme_index = NULL;
        autofree(char) *theme_dir = NULL;
        char *command[] = {
                "/usr/bin/gtk-update-icon-cache",
                "-ftq",
                NULL, /* Path */
                NULL, /* Terminator */
        };

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        /* We're not interested in subdirs in our execution call */
        theme_name = usc_get_strn_component(path, 3);
        if (!theme_name) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        /* Grab the theme dir now */
        if (asprintf(&theme_dir, "/usr/share/icons/%s", theme_name) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        /* Index theme too */
        if (asprintf(&theme_index, "%s/index.theme", theme_dir) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        /* We'll only record our skip for valid paths */
        if (usc_context_should_skip(ctx, theme_dir)) {
                return USC_HANDLER_SUCCESS;
        }

        if (!usc_context_push_skip(ctx, theme_dir)) {
                return USC_HANDLER_FAIL;
        }

        /* Is this a stray icon theme dir? */
        if (is_orphan_icon_dir(theme_dir)) {
                usc_context_emit_task_start(ctx, "Removing empty icon directory: %s", theme_dir);
                if (rmdir_and(theme_dir, "icon-theme.cache") != 0) {
                        usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                        usc_context_printf(ctx,
                                           "Failed to remove '%s': %s\n",
                                           theme_dir,
                                           strerror(errno));
                        return USC_HANDLER_FAIL;
                }
                usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
                return USC_HANDLER_SUCCESS | USC_HANDLER_DROP;
        }

        /* Require an index theme. */
        if (!usc_file_exists(theme_index)) {
                return USC_HANDLER_SKIP;
        }

        command[2] = (char *)theme_dir;
        usc_context_emit_task_start(ctx, "Updating icon theme cache: %s", theme_name);
        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        return USC_HANDLER_SUCCESS;
}

const UscHandler usc_handler_icon_cache = {
        .name = "icon-caches",
        .description = "Update icon theme caches",
        .required_bin = "/usr/bin/gtk-update-icon-cache",
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
