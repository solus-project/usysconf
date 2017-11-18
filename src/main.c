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

#include <stdio.h>
#include <stdlib.h>

#include "context.h"

/* DEMO */
static bool demo1(__usc_unused__ UscContext *ctx, const char *p, const char *fp)
{
        fprintf(stderr, "Checking: %s (%s)\n", p, fp);
        return true;
}

static bool demo2(UscContext *ctx, const char *p, const char *fp)
{
        fprintf(stderr, "Checking: %s (%s) from %s\n", p, fp, usc_context_get_prefix(ctx));
        return true;
}

static usc_context_func handlers[] = {
        demo1,
        demo2,
};

int main(__usc_unused__ int argc, __usc_unused__ char **argv)
{
        autofree(UscContext) *context = NULL;

        context = usc_context_new("/");
        if (!context) {
                fputs("Cannot continue without valid UscContext\n", stderr);
                return EXIT_FAILURE;
        }

        if (usc_context_has_flag(context, USC_FLAGS_CHROOTED)) {
                fputs("Chrooted!\n", stdout);
        } else {
                fputs("Not chrooted\n", stdout);
        }

        /* Just test the main loop iteration jank for now */
        for (size_t i = 0; i < ARRAY_SIZE(handlers); i++) {
                if (!handlers[i](context, "/usr", "/root/usr")) {
                        fprintf(stderr, "Fail\n");
                } else {
                        fprintf(stderr, "Success\n");
                }
        }

        return EXIT_SUCCESS;
}

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
