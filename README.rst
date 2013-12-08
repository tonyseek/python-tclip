|Build Status| |Coverage Status|

Python Tclip
============

The Python binding of tclip.

Usage
-----

Install this package into your site-packages::

    $ pip install git+git://github.com/tonyseek/python-tclip.git

And use it to process pictures

.. code-block:: python

    from tclip import TClip

    clip = TClip(width=150, height=150)
    clip.process_file("/path/to/input.jpg", "/path/to/output.jpg")


.. |Build Status| image:: https://travis-ci.org/tonyseek/python-tclip.png?branch=master,develop
   :target: https://travis-ci.org/tonyseek/python-tclip
.. |Coverage Status| image:: https://coveralls.io/repos/tonyseek/python-tclip/badge.png?branch=develop
   :target: https://coveralls.io/r/tonyseek/python-tclip
