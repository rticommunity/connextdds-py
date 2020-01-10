import os
import re
import sys
import platform
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def get_nddshome(self):
        if 'NDDSHOME' in os.environ:
            return os.environ['NDDSHOME']
        raise EnvironmentError('NDDSHOME not set.')

    def get_arch(self):
        if 'CONNEXTDDS_ARCH' in os.environ:
            return os.environ['CONNEXTDDS_ARCH']
        raise EnvironmentError('CONNEXTDDS_ARCH not set.')

    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        nddshome = self.get_nddshome()
        arch = self.get_arch()
        
        cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
        if cmake_version < '3.15.0':
            raise RuntimeError("CMake >= 3.15.0 is required")

        for ext in self.extensions:
            self.build_extension(ext, nddshome, arch)

    def build_extension(self, ext, nddshome, arch):

        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DBUILD_SHARED_LIBS=ON',
                      '-DCONNEXTDDS_DIR=' + nddshome,
                      '-DCONNEXTDDS_ARCH=' + arch]

        cfg = 'Debug' if self.debug else 'MinSizeRel'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j8']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

setup(
    name='rti',
    version='0.0.1',
    author='Marc Chiesa',
    author_email='marc@rti.com',
    description='A full Python binding for RTI Connext DDS',
    long_description='',
    ext_modules=[CMakeExtension('rti.connextdds')],
    install_requires=['pybind11>=2.4'],
    setup_requires=['pybind11>=2.4'],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)