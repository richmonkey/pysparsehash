from distutils.core import setup, Extension

module = Extension("sparsehash", ["pysparsehash.cc"])
#      define_macros = [('KEY_INTEGER', '')]
#      include_dirs = [''],

setup(name="sparsehash", version="1.0", ext_modules = [module])

