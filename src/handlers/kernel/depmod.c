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

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "context.h"
#include "files.h"
#include "util.h"

static const char *module_paths[] = {
        /* Glob all module directories and track individually */
        KERNEL_MODULES_DIR "/*",
        KERNEL_MODULES_DIR "/*/*",                /* ./extra/ */
        KERNEL_MODULES_DIR "/*/kernel/drivers/*", /* i.e. nvidia in video dir */
};

/**
 * Update the module cache within the kernel directory to ensure new modules
 * are readily available.
 */
static UscHandlerStatus usc_handler_depmod_exec(UscContext *ctx, const char *path)
{
        autofree(char) *kern_dir = NULL;
        autofree(char) *kernel_nom = NULL;
        char *command[] = {
                "/sbin/depmod",
                "-a", /* probe all fellers */
                NULL, /* The path we're depmodding */
                NULL, /* Terminator */
        };

        /* Grab the kernel name for this set */
        kernel_nom = usc_get_strn_component(path, 2);
        if (!kernel_nom) {
                return USC_HANDLER_FAIL;
        }

        if (asprintf(&kern_dir, "/lib/modules/%s/kernel", kernel_nom) < 0) {
                fputs("OOM\n", stderr);
                return USC_HANDLER_FAIL;
        }

        if (!usc_file_is_dir(kern_dir)) {
                return USC_HANDLER_SKIP;
        }

        /* We'll only record our skip for valid paths */
        if (usc_context_should_skip(ctx, kern_dir)) {
                return USC_HANDLER_SUCCESS;
        }

        /* Assign path argument. */
        command[2] = (char *)kernel_nom;

        if (!usc_context_push_skip(ctx, kern_dir)) {
                return USC_HANDLER_FAIL;
        }

        usc_context_emit_task_start(ctx, "Running depmod on kernel %s", kernel_nom);
        int ret = usc_exec_command(command);
        if (ret != 0) {
                usc_context_emit_task_finish(ctx, USC_HANDLER_FAIL);
                return USC_HANDLER_FAIL;
        }
        usc_context_emit_task_finish(ctx, USC_HANDLER_SUCCESS);

        /* Need to run for all of our globs */
        return USC_HANDLER_SUCCESS;
}

const UscHandler usc_handler_depmod = {
        .name = "depmod",
        .description = "Run depmod for each kernel",
        .required_bin = "/sbin/depmod",
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
