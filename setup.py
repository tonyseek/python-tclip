from setuptools import setup
from setuptools.dist import Distribution
from importlib import import_module


class CFFIExtension(object):

    def __init__(self, qualname):
        self._qualname = qualname
        self._ffi_cached = None
        # install the cffi
        Distribution({"setup_requires": ["cffi"]})

    @property
    def _ffi(self):
        if not self._ffi_cached:
            modname, ffiname = self._qualname.rsplit(":", 1)
            mod = import_module(modname)
            ffi = getattr(mod, ffiname)
            self._ffi_cached = ffi.verifier.get_extension()
        return self._ffi_cached

    def __getattribute__(self, name):
        if name in ("_ffi", "_ffi_cached", "_qualname"):
            return object.__getattribute__(self, name)
        return getattr(self._ffi, name)

    def __setattr__(self, name, value):
        if name in ("_ffi", "_ffi_cached", "_qualname"):
            return object.__setattr__(self, name, value)
        return setattr(self._ffi, name, value)


setup(
    name="python-tclip",
    author="Jiangge Zhang",
    author_email="tonyseek@gmail.com",
    version="0.1.0",
    zip_safe=False,
    url="https://github.com/tonyseek/python-tclip",
    py_modules=["tclip"],
    ext_modules=[CFFIExtension("tclip:ffi")],
    install_requires=["cffi"],
    package_data={"": ["*.cpp"]},
)
