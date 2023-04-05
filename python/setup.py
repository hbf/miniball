from setuptools import setup, Extension

import numpy

with open("../README.md") as f:
    long_description = f.read()

module = Extension(
    "miniball",
    sources=["miniball_python.cpp"],
    include_dirs=[numpy.get_include()],
)

setup(
    name="miniball",
    version="1.0",
    description="Library to find the smallest enclosing ball of points",
    author="Kaspar Fischer, Bernd Gärtner, and Martin Kutz",
    url="https://github.com/hbf/miniball",
    long_description=long_description,
    install_requires=["numpy"],
    extras_require={"test": "pytest"},
    ext_modules=[module],
    zip_safe=False,
)
