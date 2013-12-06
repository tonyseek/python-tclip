python-tclip
============

The Python binding of tclip.

Usage
-----

Install this package into your site-packages::

    $ pip install git+git://github.com/tonyseek/python-tclip.git

And use it to process pictures::

    from tclip import TClip

    clip = TClip(width=150, height=150)
    clip.process_file("/path/to/input.jpg", "/path/to/output.jpg")
