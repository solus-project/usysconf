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

#pragma once

#include "context.h"
#include "util.h"

/**
 * A UscStateTracker is an opaque type that is used to track the state
 * of various registered trigger directories on the system
 */
typedef struct UscStateTracker UscStateTracker;

/**
 * Create a new UscStateTracker for the given context
 *
 * @returns A newly allocated UscStateTracker
 */
UscStateTracker *usc_state_tracker_new(UscContext *context);

/**
 * Free a previously allocated UscStateTracker
 */
void usc_state_tracker_free(UscStateTracker *tracker);

/**
 * Attempt to push the path into the tracker with the current mtime
 *
 * Any existing entries will be overwritten by matching their collapsed
 * path first. As such, all entries are required to actually exist on disk.
 */
bool usc_state_tracker_push_path(UscStateTracker *tracker, const char *path);

/**
 * Request that the state tracker actually dumps itself to disk so that it
 * can be reloaded at a later point
 */
bool usc_state_tracker_write(UscStateTracker *tracker);

DEF_AUTOFREE(UscStateTracker, usc_state_tracker_free)

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
