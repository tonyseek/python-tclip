from os.path import exists
from subprocess import check_output

import cffi


CONFIG_PATH = [
    "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml",
    "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml",
]


def pkgconfig(package, **kwargs):
    flag_map = {"-I": "include_dirs", "-L": "library_dirs", "-l": "libraries"}
    output = check_output(["pkg-config", "--cflags", "--libs", package])
    for token in output.split():
        if token[:2] in flag_map:
            kwargs.setdefault(flag_map.get(token[:2]), []).append(token[2:])
        else:
            kwargs.setdefault("extra_link_args", []).append(token)
    return kwargs


def setup_ffi():
    ffi = cffi.FFI()
    ffi.cdef("""
    int cffi_tclip(char *source_path, char *dest_path, int dest_width,
                   int dest_height, char *config_path);
    """)
    interface = ffi.verify(sources=["tclip.cpp"], language="c++",
                           **pkgconfig("opencv"))
    return interface


def find_exists_path(paths, default=None):
    gen_paths = (p for p in paths if exists(p))
    return next(gen_paths, default)


class TClip(object):
    def __init__(self, width, height, config_path=None):
        self.config_path = config_path or find_exists_path(CONFIG_PATH)
        self.width = width
        self.height = height
        self.impl = setup_ffi()

    def process_file(self, source_path, dest_path):
        if not self.config_path:
            raise RuntimeError("haarcascade_frontalface_alt.xml not found")
        return self.impl.cffi_tclip(source_path, dest_path, self.width,
                                    self.height, self.config_path)
