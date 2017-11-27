handlers = [
    'ldconfig',
    'depmod',
    'hwdb',
    'mime',
    'glib2',
    'icon-cache',
    'fonts',
    'desktop-files',

    'gtk2_immodules',
    'gtk3_immodules',

    'mandb',
    'ssl',
    'openssh',
]

if with_cbm == true
    handlers += 'clr-boot-manager'
endif

if with_ldm == true
    handlers += 'linux-driver-management'
endif

# We'll still enable hwdb but alter for udev path if not using systemd
if with_systemd == true
    handlers += [
        'tmpfiles',
        'sysusers',
    ]
endif

libusysconf_sources = [
    'context.c',
    'files.c',
    'state.c',
    'util.c',
]

# Push all known handlers into the build
foreach handler : handlers
    libusysconf_sources += 'handlers/@0@.c'.format(handler)
endforeach

usysconf_link_args = []

if with_static_binary == true
    usysconf_link_args += '-static'
endif

usysconf_sources = [
    'cli/main.c',
    'cli/triggers.c',
    'cli/version.c',
]

libusysconf_includes = [
    config_h_dir,
    include_directories('.'),
]

libusysconf = static_library(
    'usysconf',
    sources: libusysconf_sources,
    install: false,
    c_args: [
        '-fvisibility=default',
    ],
    dependencies: [
        libuf.get_variable('link_libuf'),
    ],
    include_directories: libusysconf_includes,
)

link_libusysconf = declare_dependency(
    link_with: libusysconf,
    include_directories: libusysconf_includes,
)


# Build main qol-assist binary
usysconf = executable(
    'usysconf',
    sources: usysconf_sources,
    install: true,
    install_dir: path_sbindir,
    link_args: usysconf_link_args,
    c_args: [
        '-fvisibility=default',
    ],
    dependencies: [
        link_libusysconf,
    ],
)
