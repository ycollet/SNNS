#!/usr/bin/env python3

import os
from setuptools import setup, Extension

# Directory containing libkernel.a/libfunc.a, produced by the project's
# CMake build (CMAKE_ARCHIVE_OUTPUT_DIRECTORY = <build dir>/bin).
# Override with the SNNS_BUILD_DIR environment variable if your build
# directory isn't the conventional "../build" next to this file.
snns_build_dir = os.environ.get('SNNS_BUILD_DIR', '../build')
snns_lib_dir = os.path.join(snns_build_dir, 'bin')

module1 = Extension('krui',
                    sources = ['snns/krui.c'],
                    include_dirs = ['../kernel/sources'],
                    library_dirs = [snns_lib_dir],
                    libraries = ['kernel','func'])

setup (name = 'snns-kernel',
       ext_package='snns',
       version = '0.1',
       author='Patrick Kursawe',
       author_email='Patrick.Kursawe@web.de',
       description = 'SNNS kernel functions',
       packages = ['snns'],
       ext_modules = [module1],
       py_modules = ['snns.util'])
