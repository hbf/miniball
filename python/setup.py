from setuptools import setup, Extension


with open("../README.md") as f:
    long_description = f.read()

module = Extension(
    "miniball",
    sources=["miniball_python.cpp"],
)

setup(
    name="miniball",
    version="1.0",
    description="Library to find the smallest enclosing ball of points",
    author="Martin Kutz",
    author_email="kutz@math.fu-berlin.de",
    url="https://github.com/hbf/miniball",
    long_description=long_description,
    install_requires=["numpy"],
    extras_require={"test": "pytest"},
    ext_modules=[module],
    zip_safe=False,
)
