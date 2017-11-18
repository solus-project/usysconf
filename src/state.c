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

#include "state.h"

/**
 * TODO: Have this in /var and a define option
 */
#define STATE_FILE "state"

struct UscStateTracker {
        UscContext *context;
        char *state_file;
};

UscStateTracker *usc_state_tracker_new(UscContext *context)
{
        UscStateTracker *ret = NULL;

        if (!context) {
                return NULL;
        }
        ret = calloc(1, sizeof(UscStateTracker));
        if (!ret) {
                return NULL;
        }
        /* TODO: Use prefix, i just need local testing for now. */
        if (asprintf(&ret->state_file, "%s/%s", ".", STATE_FILE) < 0) {
                usc_state_tracker_free(ret);
                return NULL;
        }
        return ret;
}

void usc_state_tracker_free(UscStateTracker *self)
{
        if (!self) {
                return;
        }
        free(self->state_file);
        free(self);
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
