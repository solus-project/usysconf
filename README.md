# usysconf

[![License](https://img.shields.io/badge/License-GPL%202.0-blue.svg)](https://opensource.org/licenses/GPL-2.0)

Universal system configuration interface for operating systems (i.e. Solus)

This is a staging tree to work on a new component for Solus, a configuration interface
that centralises all of the stateful configuration tasks currently handled in many disconnected
package install scripts.

This project will provide a unified approach to configuring the system, and is designed
to be the very last operation that package manager would call in a stack. This also allows
Solus to drop the [COMAR system](https://solus-project.com/2017/11/12/this-week-in-solus-install-48/).

In time this will also expand to incorporate management of stateless configuration
policies. For now we are focused on providing the automated system configuration
components to provide a fail-safe binary that is immune to update issues and can
safely apply system changes (such as cache and system user management).

We will also provide a CLI interface to allow usysconf to be utilised as a recovery
tool, enabling users to apply triggers and bring their system back up to full health.

`usysconf` is a [Solus project](https://solus-project.com/)

![logo](https://build.solus-project.com/logo.png)

### TODO

 - [x] Get initial static binary together
 - [x] Add library functions to help with logging/state/exec/etc
 - [x] Add very basic `mtime` tests for conditionally updating directories
 - [x] Enforce linking against `musl` for tiny binaries. (glibc is huge..)
 - [x] Drop `prefix` notion and restrict to direct host usage (or chroot)
 - [ ] Make sure the following list is fully accounted for by usysconf
 - [ ] Don't run a hook if prerequisites aren't found

#### Affected packages in Solus

Checkmark means work is unblocked for this item.

 - [x] nvidia-glx-driver
 - [x] nvidia-304-glx-driver
 - [x] nvidia-340-glx-driver
 - [x] mesalib
 - [x] xorg-server 
 - [x] openssh (must also be converted to sysusers/tmpfiles)
 - [x] fontconfig
 - [x] ca-certs (will expand to use `trust` in future)
 - [x] gtk2
 - [x] gtk3
 - [x] glib2
 - [x] desktop-file-utils
 - [x] hicolor-icon-theme
 - [x] comar (irony, yes. it just runs an unneeded script)
 - [x] cups (just switch to sysusers/tmpfiles)
 - [x] kernel-glue
 - [ ] baselayout
 - [x] dcron (consider killing with fire, convert to sysusers/tmpfile)
 - [x] gconf
 - [x] mlocate (remove trigger completely, sysusers the mlocale group)
 - [ ] pisi (currently unkillable until we build muxer)


## Authors

Copyright © 2017 Solus Project

`usysconf` is available under the terms of the `GPL-2.0` license.
