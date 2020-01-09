from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools
import os

__version__ = '0.0.1'


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


def get_connextdds_src_files():
    src_files = []
    for root, dirs, files in os.walk('src', False):
        for filename in files:
            _, ext = os.path.splitext(filename)
            if ext == '.cpp':
                src_files.append(os.path.join(root, filename))

    return src_files

def get_nddshome():
    if 'NDDSHOME' in os.environ:
        return os.environ['NDDSHOME']
    raise EnvironmentError('NDDSHOME not set.')

def get_rti_lib_path():
    if 'RTI_LIBRARY_PATH' in os.environ:
        return os.environ['RTI_LIBRARY_PATH']
    raise EnvironmentError('RTI_LIBRARY_PATH not set.')


ext_modules = [
    Extension(
        'connextdds',
        get_connextdds_src_files(),
        include_dirs=[
            # Path to pybind11 headers
            'include',
            os.path.join(get_nddshome(), 'include'),
            os.path.join(get_nddshome(), 'include', 'ndds'),
            os.path.join(get_nddshome(), 'include', 'ndds', 'hpp'),
            get_pybind_include(),
            get_pybind_include(user=True)
        ],
        libraries=[
            'rtidlc',
            'nddscpp2',
            'nddsc',
            'nddscore',
            'm',
            'pthread',
            'dl'
        ],
        library_dirs=[
            get_rti_lib_path()
        ],
        language='c++'
    ),
]


# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    """Return the -std=c++[11/14/17] compiler flag.
    The newer version is prefered over c++11 (when it is available).
    """
    #flags = ['-std=c++17', '-std=c++14', '-std=c++11']
    flags = ['-std=c++14', '-std=c++11']

    for flag in flags:
        if has_flag(compiler, flag): return flag

    raise RuntimeError('Unsupported compiler -- at least C++11 support '
                       'is needed!')


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': ['-Os', '-fpic'],
    }
    l_opts = {
        'msvc': [],
        'unix': ['-fpic'],
    }

    darwin_opts = ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    if sys.platform == 'darwin':
        darwin_opts = ['-stdlib=libc++', '-mmacosx-version-min=10.7', '-m64']
        c_opts['unix'] += darwin_opts
        c_opts['unix'].extend(['-DRTI_UNIX', '-DRTI_DARWIN', '-DRTI_64BIT', '-Wno-return-type-c-linkage'])
        l_opts['unix'] += darwin_opts

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        link_opts = self.l_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts
        build_ext.build_extensions(self)

setup(
    name='rti',
    version=__version__,
    author='Marc Chiesa',
    author_email='marc@rti.com',
    url='na',
    description='A full Python binding for RTI Connext DDS',
    long_description='',
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.4'],
    setup_requires=['pybind11>=2.4'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)