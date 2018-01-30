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

#pragma once

/**
 * Simple helper for defining CLI commands and such
 */
typedef struct UscSubCommand {
        const char *name;    /**<Actual CLI name, i.e. 'help' */
        const char *summary; /**<Brief summary for printing the help */
        int (*exec)(int arv, char **argv);
} UscSubCommand;

/**
 * Main command is to actually run our triggers
 */
int usc_cli_run_triggers(int argc, char **argv);

/**
 * Spam the version the console, because everyone wants to know that.
 */
int usc_cli_version(int argc, char **argv);

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
