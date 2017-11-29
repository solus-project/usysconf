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

#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Determine if the system is operating within a chroot or not.
 *
 * @returns True if we've detected a chroot environment
 */
bool usc_is_chrooted(void);

/**
 * Sanity test, determine if the /proc filesystem is mounted and available.
 * To be useful and function correctly, usysconf absolutely requires that
 * the procfs is mounted.
 *
 * @returns True if proc was successfully detected as mounted.
 */
bool usc_is_proc_mounted(void);

/**
 * Grab the mtime for a given path
 *
 * @param path Path that exists..
 * @param time Pointer to store the time in
 *
 * @return True if we were able to grab the mtime
 */
bool usc_file_mtime(const char *path, time_t *time);

/**
 * Determine if the given path actually exists
 *
 * @param path Path to test
 * @returns True if it exists
 */
bool usc_file_exists(const char *path);

/**
 * Determine if the given path is a directory
 *
 * @param path The path to test
 * @returns True if the given path is a directory
 */
bool usc_file_is_dir(const char *path);

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
