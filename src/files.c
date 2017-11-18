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

#include <stdio.h>
#include <sys/stat.h>

#include "files.h"

bool usc_is_chrooted()
{
        struct stat st = { 0 };
        /* Don't do dangerous system ops within a chroot like cbm updates */
        if (stat("/", &st) != 0) {
                fprintf(stderr, "Unable to probe '/', assuming chroot\n");
                return true;
        }
        return st.st_ino != 2;
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
