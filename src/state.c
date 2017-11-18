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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "state.h"

/**
 * TODO: Have this in /var and a define option
 */
#define STATE_FILE "state"

DEF_AUTOFREE(FILE, fclose)

/**
 * Each entry is just a list node with a ptr (registered interest) and
 * an mtime for when it was last modified on disk
 */
typedef struct UscStateEntry {
        struct UscStateEntry *next; /**<Next guy in the chain */
        char *ptr;                  /**<Registered interest */
        time_t mtime;               /**<Last modified stamp */
} UscStateEntry;

/**
 * Opaque implementation details.
 */
struct UscStateTracker {
        UscContext *context;
        char *state_file;
        UscStateEntry *entry; /**<Root entry in the list */
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

/**
 * Lookup the path within the internal list and find any matching node for it.
 *
 * This may return NULL.
 */
static UscStateEntry *usc_state_tracker_lookup(UscStateTracker *self, const char *path)
{
        for (UscStateEntry *entry = self->entry; entry; entry = entry->next) {
                if (entry->ptr && strcmp(entry->ptr, path) == 0) {
                        return entry;
                }
        }
        return NULL;
}

bool usc_state_tracker_push_path(UscStateTracker *self, const char *path)
{
        UscStateEntry *entry = NULL;
        autofree(char) *dup = NULL;
        struct stat st = { 0 };

        /* Make sure it actually exists */
        dup = realpath(path, NULL);
        if (!dup) {
                return false;
        }

        if (stat(dup, &st) != 0) {
                return false;
        }

        /* Does this entry already exist */
        entry = usc_state_tracker_lookup(self, dup);
        if (entry) {
                goto store_time;
        }

        /* Must insert a new entry now */
        entry = calloc(1, sizeof(UscStateEntry));
        if (!entry) {
                fputs("OOM\n", stderr);
                return false;
        }
        entry->ptr = strdup(dup);
        if (!entry->ptr) {
                fputs("OOM\n", stderr);
                free(entry);
                return false;
        }

store_time:
        /* Merge list reversed */
        entry->mtime = st.st_mtime;
        entry->next = self->entry;
        self->entry = entry;
        return true;
}

static void usc_state_entry_free(UscStateEntry *entry)
{
        if (!entry) {
                return;
        }
        /* Free next dude in the chain */
        usc_state_entry_free(entry->next);
        fprintf(stderr, "ENTRY: %s (%ld)\n", entry->ptr, entry->mtime);
        free(entry->ptr);
        free(entry);
}

void usc_state_tracker_free(UscStateTracker *self)
{
        if (!self) {
                return;
        }
        usc_state_entry_free(self->entry);
        free(self->state_file);
        free(self);
}

bool usc_state_tracker_write(UscStateTracker *self)
{
        autofree(FILE) *fp = NULL;

        fp = fopen(self->state_file, "w");
        if (!fp) {
                fprintf(stderr, "fopen(): %s %s\n", self->state_file, strerror(errno));
                return false;
        }

        /* Walk nodes */
        for (UscStateEntry *entry = self->entry; entry; entry = entry->next) {
                if (fprintf(fp, "%ld:%s\n", entry->mtime, entry->ptr) < 0) {
                        fprintf(stderr, "fprintf(): %s %s\n", self->state_file, strerror(errno));
                        return false;
                }
        }

        return true;
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
