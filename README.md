# usysconf

[![License](https://img.shields.io/badge/License-GPL%202.0-blue.svg)](https://opensource.org/licenses/GPL-2.0)

Universal system configuration interface for operating systems (i.e. Solus)

`usysconf` provides a centralised configuration system to replace "package hokos" and post-installation triggers with a reentrant binary, suitable for use in recovery situations. It requires a certain development approach, by throwing away traditional postinstalls, and centralising/minimising the amount of system delta generated during these steps.

The binary replaces the legacy [COMAR system](https://solus-project.com/2017/11/12/this-week-in-solus-install-48/) in Solus with a single point for all configuration to take place. A simple state file tracks the `mtime` for interesting file paths, and when those are invalidated triggers are run in response. This allows `usysconf` to be the final execution step in any package/update operation, and incremently apply trigger steps to the system.

Lastly, a CLI interface is provided allowing users to forcibly run well known triggers by name (or all) to assist in recovery. This allows users to not worry about the inaccessible triggers of the past, and instead request usysconf run all triggers that need running. Optionally, all triggers can be run, bypassing update time checks:

```bash
$ sudo usysconf run -f
```

`usysconf` is a [Solus project](https://solus-project.com/)

![logo](https://build.solus-project.com/logo.png)

## Supported triggers

| Trigger        | Description                             | Notes                                         |
|----------------|-----------------------------------------|-----------------------------------------------|
| ldconfig       | Update dynamic library cache            |                                               |
| boot           | Update boot configuration + kernels     | Uses `clr-boot-manager`                       |
| qol-assist     | Register QoL migration                  | Uses `qol-assist`                             |
| depmod         | Run depmod for each kernel              |                                               |
| hwdb           | Update hardware database                |                                               |
| drivers        | Update graphical driver configuration   | Uses `linux-driver-management`                |
| sysusers       | Update systemd sysusers                 |                                               |
| tmpfiles       | Update systemd tmpfiles                 |                                               |
| systemd-reload | Reload systemd configuration            |                                               |
| systemd-reexec | Re-execute systemd                      |                                               |
| vbox-restart   | Restart VirtualBox services             | Will be replaced with generic service handler |
| apparmor       | Compile AppArmor profiles               | Uses `aa-lsm-hook`                            |
| glib2          | Compile glib-schemas                    |                                               |
| fonts          | Rebuild font cache                      |                                               |
| mime           | Update mimetype database                |                                               |
| icon-caches    | Update icon theme caches                |                                               |
| desktop-files  | Update desktop database                 |                                               |
| gconf          | Update GConf schemas                    |                                               |
| dconf          | Update dconf database                   |                                               |
| gtk2-immodules | Update GTK2 input module cache          |                                               |
| gtk3-immodules | Update GTK3 input module cache          |                                               |
| mandb          | Updating manpages database              |                                               |
| ssl-certs      | Update SSL certificate configuration    | Only uses `c_rehash` right now                |
| mono-certs     | Populate Mono certificates              | Uses `cert-sync`                              |
| openssh        | Create OpenSSH host key                 |                                               |
| udev-rules     | Reload and apply new udev rules         | Always last to facilitate userspace responses |

### Special note

The two cert handlers will eventually merge to use a full system trust store that serves normal applications and Mono.


## Authors

Copyright © 2017-2018 Solus Project

`usysconf` is available under the terms of the `GPL-2.0` license.
