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

#include "context.h"
#include "util.h"

static const char *icon_cache_paths[] = {
        "/usr/share/icons",
};

/**
 * Update the icon cache on disk for every icon them found in the given directory
 */
static UscHandlerStatus usc_handler_icon_cache_exec(UscContext *ctx, const char *path,
                                                    const char *full_path)
{
        fprintf(stderr, "Checking %s (%s)\n", path, full_path);
        /* We do nothing *yet* */
        return USC_HANDLER_FAIL;
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
