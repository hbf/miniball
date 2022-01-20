from setuptools import setup, Extension

module = Extension(
    "miniball",
    include_dirs=["/usr/include/python3.8" "/usr/include/python3.8"],
    sources=["miniball_python.cpp"],
)

setup(
    name="miniball",
    version="1.0",
    description="The miniball package",
    author="foo bar",
    author_email="foo@bar",
    url="https://foo.bar",
    long_description="TODO",
    install_requires=["numpy"],
    extras_require={"test": "pytest"},
    ext_modules=[module],
)
