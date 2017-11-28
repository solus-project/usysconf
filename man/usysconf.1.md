usysconf(1) -- Universal system configuration
=============================================


## SYNOPSIS

`usysconf run [trigggers]`


## DESCRIPTION

`usysconf` is a system configuration agent designed for integration within the
software update process. The main job for `usysconf` is to run OS configuration
tasks within a well defined centralised location.

`usysconf` will run a variety of triggers to ensure system consistency and
health, detecting changes within the filesystem and taking any appropriate
actions to configure components such as kernels or the dynamic linker cache.

## SUBCOMMANDS

`run [triggers]`

    When called without any arguments, all system triggers will be executed.
    Alternatively, you may provide a list of triggers to run directly.

    The special value "list" will cause all known triggers to be listed
    instead of executing them.

`help`

    Print the supported command set for the `usysconf(1)` binary.

`version`

    Print the version and license information, before quitting.

## FILES

`usysconf` tracks state through some special files, and will recover in their
absence.

`/var/log/usysconf.log`

    This file will contain the output from the last run of usysconf, if the
    tool was run automatically without a working `stdout`. This is typical
    for situations where `usysconf` is masked behind a long running daemon.

`/var/log/usysconf.rewind.log`

    This is a temporary file used by `usysconf` to store the stdout/stderr
    of the currently executing command. Under normal situations, this file
    will be replayed into the main log file if there is an error, and this
    rewind log will be deleted.

`/var/lib/usysconf/status`

    This file is used to track the current state of `usysconf` handlers.
    Upon completion, `usysconf` will write the state file with the mtime
    for each known asset for the triggers. This file is read by `usysconf`
    on startup to allow unmodified assets to be skipped, making `usysconf`
    incremental in nature.

    If this file is removed, `usysconf` will run all triggers again and
    write the state file once more - this is non fatal. In fact, this
    mechanism can be used to reset `usysconf` state to ensure that all
    triggers do run again.
   

## EXIT STATUS

On success, 0 is returned. A non-zero return code signals a failure.


## COPYRIGHT

 * Copyright © 2017 Ikey Doherty, License: CC-BY-SA-3.0


## SEE ALSO

`clr-boot-manager(1)`, `qol-assist(1)`

 * https://github.com/solus-project/usysconf

## NOTES

Creative Commons Attribution-ShareAlike 3.0 Unported

 * http://creativecommons.org/licenses/by-sa/3.0/
