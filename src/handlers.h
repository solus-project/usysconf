/*
 * This file is part of usysconf.
 *
 * Copyright © 2017-2018 Solus Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "config.h"
#include "context.h"

/* Implemented elsewhere in the codebase */
extern UscHandler usc_handler_ldconfig;

#ifdef HAVE_CBM
extern UscHandler usc_handler_cbm;
#endif

#ifdef HAVE_QOL_ASSIST
extern UscHandler usc_handler_qol_assist;
#endif

extern UscHandler usc_handler_depmod;

extern UscHandler usc_handler_hwdb;

#ifdef HAVE_LDM
extern UscHandler usc_handler_ldm;
#endif

#ifdef HAVE_SYSTEMD
extern UscHandler usc_handler_sysusers;
extern UscHandler usc_handler_tmpfiles;
extern UscHandler usc_handler_systemd_reload;
#ifdef HAVE_SYSTEMD_REEXEC
extern UscHandler usc_handler_systemd_reexec;
#endif
#ifdef HAVE_VBOX_RESTART
extern UscHandler usc_handler_vbox_restart;
#endif
#endif

#ifdef HAVE_APPARMOR
extern UscHandler usc_handler_apparmor;
#endif

#ifdef HAVE_MONO_CERTS
extern UscHandler usc_handler_mono_certs;
#endif

extern UscHandler usc_handler_glib2;
extern UscHandler usc_handler_fonts;
extern UscHandler usc_handler_mime;
extern UscHandler usc_handler_icon_cache;
extern UscHandler usc_handler_desktop_files;
extern UscHandler usc_handler_gconf;
extern UscHandler usc_handler_dconf;

extern UscHandler usc_handler_gtk2_immodules;
extern UscHandler usc_handler_gtk3_immodules;

extern UscHandler usc_handler_mandb;
extern UscHandler usc_handler_ssl_certs;
extern UscHandler usc_handler_sshd;

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
