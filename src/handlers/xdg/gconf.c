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
#include <fcntl.h>
#include <glob.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "context.h"
#include "files.h"
#include "util.h"

static const char *gconf_paths[] = { "/etc/gconf/schemas", "/usr/share/gconf/schemas" };

#define GCONF_PRIMARY_TREE "/etc/gconf/gconf.xml.defaults"
#define MERGE_PTR "xml:merged:" GCONF_PRIMARY_TREE

/**
 * Helper method to nuke the existing tree quickly
 */
static int rm_dir_kids(const char *dir)
{
        autofree(DIR) *d = NULL;
        struct dirent *ent = NULL;
        int fd = 0;
        int r = 0;

        d = opendir(dir);
        if (!d) {
                return -errno;
        }

        fd = dirfd(d);

        while ((ent = readdir(d)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0) {
                        continue;
                }
                if (strcmp(ent->d_name, "..") == 0) {
                        continue;
                }
                if (!strstr(ent->d_name, ".xml")) {
                        continue;
                }
                if ((r = unlinkat(fd, ent->d_name, 0)) != 0) {
                        return r;
                }
        }
        return 0;
}

/**
 * Rebuild the gconf2 merged database
 */
static UscHandlerStatus usc_handler_gconf_exec_internal(UscContext *ctx, const char *path)
{
        glob_t schemas = { 0 };
        autofree(char) *schema_glob = NULL;
        UscHandlerStatus ret = USC_HANDLER_SKIP;

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        /* Build the glob pattern */
        if (asprintf(&schema_glob, "%s/*.schemas", path) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        schemas.gl_offs = 2;
        if (glob(schema_glob, GLOB_DOOFFS | GLOB_NOSORT, NULL, &schemas) != 0) {
                ret = USC_HANDLER_SKIP;
                goto done;
        }

        /* Set args */
        schemas.gl_pathv[0] = "/usr/bin/gconftool-2";
        schemas.gl_pathv[1] = "--makefile-install-rule";

        setenv("GCONF_CONFIG_SOURCE", MERGE_PTR, 1);
        usc_context_emit_task_start(ctx, "Registering gconf schemas in: %s", path);
        int r = usc_exec_command(schemas.gl_pathv);
        if (r != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                ret = USC_HANDLER_FAIL | USC_HANDLER_BREAK;
        } else {
                usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
                ret = USC_HANDLER_SUCCESS | USC_HANDLER_BREAK;
        }

        unsetenv("GCONF_CONFIG_SOURCE");
done:
        globfree(&schemas);
        return ret;
}

static UscHandlerStatus usc_handler_gconf_exec(UscContext *ctx, const char *path)
{
        UscHandlerStatus ret = 0;

        if (!usc_file_is_dir(path)) {
                return USC_HANDLER_SKIP;
        }

        /* Even if we re-enter, ensure we only nuke the first time. */
        if (!usc_context_should_skip(ctx, GCONF_PRIMARY_TREE)) {
                if (usc_file_is_dir(GCONF_PRIMARY_TREE)) {
                        usc_context_emit_task_start(ctx, "Removing old gconf tree");
                        if (rm_dir_kids(GCONF_PRIMARY_TREE) < 0) {
                                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                                usc_context_printf(ctx,
                                                   "Cannot remove stale gconf tree: %s\n",
                                                   strerror(errno));
                                return USC_HANDLER_FAIL;
                        }
                        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
                }

                if (!usc_context_push_skip(ctx, GCONF_PRIMARY_TREE)) {
                        fputs("OOM\n", stderr);
                        return USC_HANDLER_FAIL;
                }
        }

        if (!usc_file_exists(GCONF_PRIMARY_TREE)) {
                usc_context_emit_task_start(ctx, "Preparing gconf tree");
                if (mkdir(GCONF_PRIMARY_TREE, 00755) != 0) {
                        usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                        fprintf(stderr, "Cannot construct gconf dir: %s\n", strerror(errno));
                        return USC_HANDLER_FAIL;
                }
                usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);
        }

        /* Make sure we update all the trees now */
        for (size_t i = 0; i < ARRAY_SIZE(gconf_paths); i++) {
                UscHandlerStatus lret = usc_handler_gconf_exec_internal(ctx, gconf_paths[i]);
                if ((lret & USC_HANDLER_FAIL) == USC_HANDLER_FAIL) {
                        ret = lret;
                        break;
                }
                if ((ret & USC_HANDLER_SUCCESS) != USC_HANDLER_SUCCESS) {
                        ret = lret;
                }
        }

        return ret;
}

const UscHandler usc_handler_gconf = {
        .name = "gconf",
        .description = "Update GConf schemas",
        .required_bin = "/usr/bin/gconftool-2",
        .exec = usc_handler_gconf_exec,
        .paths = gconf_paths,
        .n_paths = ARRAY_SIZE(gconf_paths),
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
