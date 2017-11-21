handlers = [
    'ldconfig',
    'clr-boot-manager',
    'depmod',
    'glib2',
    'icon-cache',
    'fonts',
]


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
