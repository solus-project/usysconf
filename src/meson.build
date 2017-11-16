usysconf_sources = [
    'main.c',
]

# Build main qol-assist binary
usysconf = executable(
    'usysconf',
    sources: usysconf_sources,
    install: true,
    install_dir: path_sbindir,
    include_directories: [
        config_h_dir,
    ],
)
