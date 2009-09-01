from distutils.core import setup, Extension

setup(name="ketama", version="0.1",
      ext_modules=[Extension("ketama", ["ketamamodule.c"],
                             library_dirs=["/usr/local/lib"],
                             include_dirs=["/usr/local/include"],
                             libraries=["ketama"])])
