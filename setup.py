#!/usr/bin/env python

import os
import sys

from setuptools import setup, find_packages, Extension

def GetVersion():
  src = open('gsf/__init__.py')
  for line in src:
    if line.startswith('__version__'):
      return line.split('\'')[1]


setup(name='gsf',
      version=GetVersion(),
      description='Generic Sensor Format',
      author='Kurt Schwehr',
      author_email='schwehr@google.com',
      url='https://github.com/schwehr/generic_sensor_format',
      license='Apache 2.0',
      packages=find_packages(),
      classifiers=[
           'License :: OSI Approved :: Apache Software License',
           'Development Status :: 4 - Beta',
           'Intended Audience :: Science/Research',
           'Topic :: Communications',
           'Topic :: Scientific/Engineering :: Information Analysis',
           'Topic :: Scientific/Engineering :: '
           'Interface Engine/Protocol Translator',
           'Topic :: Scientific/Engineering :: GIS',
           ],
      # scripts=[""],
      # test_suite="test"
      test_suite = "test_py",
      entry_points={
        'console_scripts': [
            'gsf-info = gsf.gsf_info:main',
            'gsf-dump-hex = gsf.dump_hex:main',
        ],
      },
      )
