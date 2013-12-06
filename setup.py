from subprocess import check_output
from setuptools import setup, Extension


def pkgconfig(package, **kwargs):
    flag_map = {"-I": "include_dirs", "-L": "library_dirs", "-l": "libraries"}
    output = check_output(["pkg-config", "--cflags", "--libs", package])
    for token in output.split():
        if token[:2] in flag_map:
            kwargs.setdefault(flag_map.get(token[:2]), []).append(token[2:])
        else:
            kwargs.setdefault("extra_link_args", []).append(token)
    return kwargs


tclip_ext = Extension(
    name="_tclip",
    sources=["tclip.cpp"],
    language="c++",
    **pkgconfig("opencv")
)


setup(
    name="python-tclip",
    author="Jiangge Zhang",
    author_email="tonyseek@gmail.com",
    version="0.1.0",
    zip_safe=False,
    url="https://github.com/tonyseek/python-tclip",
    py_modules=["tclip"],
    ext_modules=[tclip_ext],
)
