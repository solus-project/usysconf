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

#include <limits.h>
#include <mntent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "files.h"
#include "util.h"

bool usc_is_chrooted()
{
        struct stat root_stat = { 0 };
        struct stat proc_stat = { 0 };

        if (stat("/", &root_stat) != 0) {
                fprintf(stderr, "Unable to probe '/', assuming chroot\n");
                return true;
        }
        if (stat("/proc/1/root/.", &proc_stat) != 0) {
                fprintf(stderr, "Unable to probe '/proc/1/root/.', assuming chroot\n");
                return true;
        }

        if (root_stat.st_dev != proc_stat.st_dev) {
                return true;
        }
        if (root_stat.st_ino != proc_stat.st_ino) {
                return true;
        }
        return false;
}

bool usc_is_proc_mounted()
{
        struct mntent *ent = NULL;
        struct mntent mnt = { 0 };
        FILE *mnts = NULL;
        char buf[PATH_MAX];
        bool ret = false;

        mnts = setmntent("/proc/self/mounts", "r");
        if (!mnts) {
                return false;
        }

        while ((ent = getmntent_r(mnts, &mnt, buf, sizeof(buf)))) {
                if (mnt.mnt_dir && strcmp("/proc", mnt.mnt_dir) == 0) {
                        ret = true;
                        break;
                }
        }

        endmntent(mnts);
        return ret;
}

bool usc_file_mtime(const char *path, time_t *time)
{
        struct stat st = { 0 };
        if (stat(path, &st) != 0) {
                return false;
        }
        *time = st.st_mtime;
        return true;
}

bool usc_file_exists(const char *path)
{
        __usc_unused__ struct stat st = { 0 };
        if (lstat(path, &st) != 0) {
                return false;
        }
        return true;
}

bool usc_file_is_dir(const char *path)
{
        struct stat st = { 0 };
        if (stat(path, &st) != 0) {
                return false;
        }
        return S_ISDIR(st.st_mode);
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
