import os
import re
import sys
import platform
import subprocess
import collections
import shutil
import filecmp
import cmake

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

PlatformLibEnds = collections.namedtuple('PlatformLibEnds', ['prefix', 'suffix'])

rti_native_libs = {
    'rti': ['nddsc', 'nddscore', 'nddscpp2'],
    'rti.logging': ['rtidlc']
}

rti_platform_ends = {
    'Linux': PlatformLibEnds('lib','.so'),
    'Windows': PlatformLibEnds('', '.dll'),
    'Darwin': PlatformLibEnds('lib', '.dylib')
}


def get_rti_libs(pkg_name, pkg_dir):
    if pkg_name in rti_native_libs and platform.system() in rti_platform_ends:
        lib_bases = rti_native_libs[pkg_name]
        ends = rti_platform_ends[platform.system()]
        libs = [ends.prefix + name + get_debug_str() + ends.suffix
                    for name in lib_bases]
        lib_files = [os.path.join(get_nddshome(), 'lib', get_arch(), filename)
                    for filename in libs]
        for filepath in lib_files:
            dst_filepath = os.path.join(get_script_path(), pkg_dir, os.path.basename(filepath))
            if not (os.path.exists(dst_filepath) and filecmp.cmp(filepath, dst_filepath)):
                print('Copying {} to {}'.format(filepath, dst_filepath))
                shutil.copy(filepath, os.path.join(get_script_path(), pkg_dir))
        return libs
    raise RuntimeError('Error setting up package name "{}" on platform "{}"'.format(
        pkg_name, platform.system()))


def get_script_path():
    return os.path.dirname(os.path.realpath(__file__))


def get_nddshome():
    if 'NDDSHOME' in os.environ:
        return os.environ['NDDSHOME']
    raise EnvironmentError('NDDSHOME not set.')


def get_arch():
    if 'CONNEXTDDS_ARCH' in os.environ:
        return os.environ['CONNEXTDDS_ARCH']
    raise EnvironmentError('CONNEXTDDS_ARCH not set.')


def get_job_count():
    if 'NJOBS' in os.environ:
        return os.environ['NJOBS']
    return '1'


def get_debug_str():
    return 'd' if get_debug_env() else ''


def get_debug_env():
    if 'DEBUG' in os.environ:
        return os.environ['DEBUG'] == '1'
    return False


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        nddshome = get_nddshome()
        arch = get_arch()
        
        cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
        if cmake_version < '3.15.0':
            raise RuntimeError("CMake >= 3.15.0 is required")

        for ext in self.extensions:
            self.build_extension(ext, nddshome, arch)

    def build_extension(self, ext, nddshome, arch):

        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cfg = 'Debug' if (self.debug or get_debug_env()) else 'Release'
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DBUILD_SHARED_LIBS=ON',
                      '-DCONNEXTDDS_DIR=' + nddshome,
                      '-DCONNEXTDDS_ARCH=' + arch,
                      '-DCMAKE_BUILD_TYPE=' + cfg,
                      '-DNUM_JOBS=' + get_job_count()]

        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if ext != 'rti.connextdds':
                cmake_args += ['-DBUILD_TEMP_DIRECTORY=' + os.path.abspath(self.build_temp)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            build_args += ['--', '-j' + get_job_count()]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())

        module_build_dir = os.path.join(self.build_temp, ext.name)
        if not os.path.exists(module_build_dir):
            os.makedirs(module_build_dir)
        cmake_cmd = os.path.join(cmake.CMAKE_BIN_DIR, 'cmake')
        subprocess.check_call([cmake_cmd, ext.sourcedir] + cmake_args, cwd=module_build_dir, env=env)
        subprocess.check_call([cmake_cmd, '--build', '.'] + build_args, cwd=module_build_dir)


setup(
    name='rti',
    version='0.0.1',
    author='Marc Chiesa',
    author_email='marc@rti.com',
    description='A full Python binding for RTI Connext DDS',
    long_description='',
    package_dir={'rti' : 'rti_pkg'},
    packages=['rti', 'rti.logging'],
    package_data={
        '': ['*.pyi'],
        'rti': get_rti_libs('rti', 'rti_pkg'),
        'rti.logging': get_rti_libs('rti.logging', os.path.join('rti_pkg', 'logging'))
    },
    ext_modules=[CMakeExtension('rti.connextdds', 'connextdds'), CMakeExtension('rti.logging.distlog', 'distlog')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
