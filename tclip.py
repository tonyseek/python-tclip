from os.path import dirname, realpath, join, exists

import cffi


ffi = cffi.FFI()
ffi.cdef("""
int cffi_tclip(char *source_path, char *dest_path, int dest_width,
               int dest_height, char *config_path);
""")
C = ffi.dlopen(join(dirname(realpath(__file__)), "_tclip.so"))
CONFIG_PATH = [
    "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml",
    "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml",
]


class TClip(object):
    def __init__(self, width, height, config_path=None):
        if config_path is None:
            try:
                config_path = next(p for p in CONFIG_PATH if exists(p))
            except StopIteration:
                raise RuntimeError("haarcascade_frontalface_alt.xml not found")
        self.config_path = config_path
        self.width = width
        self.height = height

    def process_file(self, source_path, dest_path):
        return C.cffi_tclip(source_path, dest_path, self.width, self.height,
                            self.config_path)
