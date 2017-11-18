usysconf_sources = [
    'context.c',
    'files.c',
    'main.c',
]

usysconf_link_args = []

if with_static_binary == true
    usysconf_link_args += '-static'
endif

# Build main qol-assist binary
usysconf = executable(
    'usysconf',
    sources: usysconf_sources,
    install: true,
    install_dir: path_sbindir,
    link_args: usysconf_link_args,
    include_directories: [
        config_h_dir,
    ],
)
