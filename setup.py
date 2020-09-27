import os
import re
import sys
import platform
import subprocess
import collections
import shutil
import filecmp
import cmake
try:
    import configparser
except:
    import ConfigParser as configparser

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

PACKAGE_CONFIG_FILENAME = 'package.cfg'

def get_script_path():
    return os.path.dirname(os.path.realpath(__file__))


def get_nddshome():
    if package_cfg.has_option('package', 'nddshome'):
        return package_cfg.get('package', 'nddshome')
    raise EnvironmentError('NDDSHOME not set.')


def get_arch():
    if package_cfg.has_option('package', 'platform'):
        return package_cfg.get('package', 'platform')
    raise EnvironmentError('CONNEXTDDS_ARCH not set.')


def get_job_count():
    if package_cfg.has_option('package', 'build-jobs'):
        return package_cfg.get('package', 'build-jobs')
    return '1'


def get_package_libs(package):
    if package_cfg.has_option('package', 'libraries-' + package):
        return package_cfg.get('package', 'libraries-' + package).split(',')
    return []


def get_debug():
    if package_cfg.has_option('package', 'build-type'):
        return package_cfg.get('package', 'build-type') == 'debug'
    return False


def find_libs(lib_list, platform):
    lib_locations = []
    for root, directories, filenames in os.walk(os.path.join(get_script_path(), 'lib', platform)):
        for filename in filenames:
            if filename in lib_list:
                lib_locations.append(os.path.join(root, filename))
    assert(len(lib_locations) == len(lib_list))
    return lib_locations


def process_darwin_libs(libs):
    from delocate import tools as tools
    for lib in libs:
        tools.set_install_id(lib, os.path.join('@rpath', os.path.basename(lib)))
        rpath = tools.get_rpaths(lib)
        if '@loader_path' not in rpath:
            tools.add_rpath(lib, '@loader_path')
        basenames = [os.path.basename(filename) for filename in libs]
        names = tools.get_install_names(lib)
        for name in names:
            if os.path.basename(name) in basenames:
                if name != os.path.join('@rpath', os.path.basename(name)):
                    tools.set_install_name(
                        lib,
                        name,
                        os.path.join('@rpath', os.path.basename(name)))


def process_linux_libs(libs):
    # only need to set rpath for Linux
    import subprocess
    for lib in libs:
        subprocess.check_call(
            ['patchelf', '--force-rpath', '--set-rpath', '$ORIGIN', lib]
        )


def process_config():
    config = configparser.ConfigParser()
    config.read(PACKAGE_CONFIG_FILENAME)
    platform = config.get('package', 'platform')
    packages = config.get('package', 'packages').split(',')
    libs = []
    for package in packages:
        libs.extend(config.get('package', 'libraries-' + package).split(','))
    lib_files = find_libs(libs, platform)
    if 'Darwin' in platform:
        process_darwin_libs(lib_files)
    elif 'Linux' in platform:
        process_linux_libs(lib_files)
    return config


class CMakeExtension(Extension):
    def __init__(self, name, dependencies):
        Extension.__init__(self, name, sources=[])
        self.dependencies = dependencies


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

        if 'Win' in arch:
            link_lib_prefix = ''
            link_lib_suffix = '.lib'
        elif 'Darwin' in arch:
            link_lib_prefix = 'lib'
            link_lib_suffix = '.dylib'
        else:
            link_lib_prefix = 'lib'
            link_lib_suffix = '.so'

        cfg = 'Debug' if (self.debug or get_debug()) else 'Release'
        cmake_args = ['-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DBUILD_SHARED_LIBS=ON',
                      '-DCONNEXTDDS_DIR=' + nddshome,
                      '-DCONNEXTDDS_ARCH=' + arch,
                      '-DCMAKE_BUILD_TYPE=' + cfg,
                      '-DRTI_LINK_DIR=' + os.path.join(get_script_path(), 'lib', arch),
                      '-DRTI_ARCH_LINK_PREFIX=' + link_lib_prefix,
                      '-DRTI_ARCH_LINK_SUFFIX=' + link_lib_suffix,
                      '-DNUM_JOBS=' + get_job_count()]

        for ext in self.extensions:
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            cmake_args += ['-D' + ext.name.replace('.', '_').upper() + '_LIBRARY_OUTPUT_DIRECTORY=' + extdir]
            self.copy_extension_libs(ext, extdir, arch)

        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            #cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            #if ext.name != 'rti.connextdds':
            #    cmake_args += ['-DBUILD_TEMP_DIRECTORY=' + os.path.abspath(self.build_temp)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            build_args += ['--', '-j' + get_job_count()]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        env['NDDSHOME'] = get_nddshome()

        module_build_dir = os.path.join(self.build_temp, 'connext-py')
        if not os.path.exists(module_build_dir):
            os.makedirs(module_build_dir)
        cmake_cmd = os.path.join(cmake.CMAKE_BIN_DIR, 'cmake')
        subprocess.check_call([cmake_cmd, os.path.abspath(os.path.join(get_script_path(), 'modules'))] + cmake_args, cwd=module_build_dir, env=env)
        subprocess.check_call([cmake_cmd, '--build', '.'] + build_args, cwd=module_build_dir)

    def copy_extension_libs(self, ext, extdir, arch):
        for lib in ext.dependencies:
            shutil.copyfile(os.path.join(get_script_path(), 'lib', arch, lib), os.path.join(extdir, lib))


package_cfg = process_config()

setup(
    ext_modules=[CMakeExtension('rti.connextdds', get_package_libs('rti')), CMakeExtension('rti.logging.distlog', get_package_libs('rti.logging'))],
    cmdclass=dict(build_ext=CMakeBuild)
)
