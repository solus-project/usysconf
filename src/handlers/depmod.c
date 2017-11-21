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

static const char *module_paths[] = {
        /* Glob all module directories and track individually */
        "/lib/modules/*",
};

/**
 * Update the module cache within the kernel directory to ensure new modules
 * are readily available.
 */
static UscHandlerStatus usc_handler_depmod_exec(__usc_unused__ UscContext *ctx, const char *path)
{
        autofree(char) *kern_dir = NULL;
        autofree(char) *base_dup = NULL;
        char *base_nom = NULL;

        /* TODO: Support basedir */
        char *command[] = {
                "/sbin/depmod",
                "-A", /* only probe new fellers */
                NULL, /* The path we're depmodding */
                NULL, /* Terminator */
        };

        if (asprintf(&kern_dir, "%s/kernel", path) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        if (!usc_file_is_dir(kern_dir)) {
                return USC_HANDLER_SKIP;
        }

        /* Grab the base name here so we know what to tell depmod */
        base_dup = strdup(path);
        if (!base_dup) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }
        base_nom = basename(base_dup);

        /* Assign path argument. */
        command[2] = (char *)base_nom;

        fprintf(stderr, "Running depmod on %s\n", base_nom);
        int ret = usc_exec_command(command);
        if (ret != 0) {
                fprintf(stderr, "Ohnoes\n");
                return USC_HANDLER_FAIL;
        }
        /* Need to run for all of our globs */
        return USC_HANDLER_SUCCESS;
}

const UscHandler usc_handler_depmod = {
        .name = "depmod",
        .exec = usc_handler_depmod_exec,
        .paths = module_paths,
        .n_paths = ARRAY_SIZE(module_paths),
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
