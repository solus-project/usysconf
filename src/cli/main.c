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
#include <string.h>

#include "cli.h"
#include "util.h"

static UscSubCommand subcommands[] = {
        { "run", "run triggers", usc_cli_run_triggers },
};

static size_t n_subcommands = ARRAY_SIZE(subcommands);

static const char *prog_name = NULL;

static void print_usage(void)
{
        fprintf(stderr, "Usage: %s [command]\n\n", prog_name);
        for (size_t i = 0; i < n_subcommands; i++) {
                UscSubCommand *command = &subcommands[i];
                fprintf(stderr, "%*s - %s\n", 20, command->name, command->summary);
        }
}

int main(int argc, char **argv)
{
        UscSubCommand *command = NULL;
        const char *command_arg = NULL;
        prog_name = argv[0];
        ++argv;
        --argc;

        if (argc < 1) {
                print_usage();
                return EXIT_FAILURE;
        }

        command_arg = argv[0];
        ++argv;
        --argc;

        /* Find the root subcommand from the passed argument */
        for (size_t i = 0; i < n_subcommands; i++) {
                UscSubCommand *c = &subcommands[i];
                if (c->name && strcmp(c->name, command_arg) == 0) {
                        command = c;
                        break;
                }
        }

        if (!command) {
                fprintf(stderr, "Unknown command '%s'\n", command_arg);
                print_usage();
                return EXIT_FAILURE;
        }

        if (!command->exec) {
                fprintf(stderr, "Not yet implemented: '%s'\n", command_arg);
                return EXIT_FAILURE;
        }

        return command->exec(argc, argv);
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
