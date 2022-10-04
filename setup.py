 #
 # (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 #
 # RTI grants Licensee a license to use, modify, compile, and create derivative
 # works of the Software solely for use with RTI products.  The Software is
 # provided "as is", with no warranty of any type, including any warranty for
 # fitness for any purpose. RTI is under no obligation to maintain or support
 # the Software.  RTI shall not be liable for any incidental or consequential
 # damages arising out of the use or inability to use the software.
 #

import os
import sys
import subprocess
import shutil
import cmake
import pybind11
try:
    import configparser
except ImportError:
    import ConfigParser as configparser

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.bdist_rpm import bdist_rpm
from distutils import sysconfig


PACKAGE_CONFIG_FILENAME = 'package.cfg'


def get_script_dir():
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


def get_cmake_toolchain():
    if package_cfg.has_option('package', 'cmake-toolchain'):
        return package_cfg.get('package', 'cmake-toolchain')
    return None


def get_python_root():
    if package_cfg.has_option('package', 'python-root'):
        return package_cfg.get('package', 'python-root')
    return None


def get_cpu(arch):
    if 'x64' in arch or 'AMD64' in arch:
        cpu = 'x64'
    elif 'i86' in arch:
        cpu = 'Win32'
    elif 'arm64' in arch:
        cpu = 'arm64'
    else:
        raise RuntimeError(f'Unsupported CPU in arch: {arch}')
    return cpu


def find_libs(lib_list, platform):
    lib_locations = []
    for root, _, filenames in os.walk(os.path.join(get_script_dir(), 'platform', platform, 'lib')):
        for filename in filenames:
            if filename in lib_list:
                lib_locations.append(os.path.join(root, filename))
    assert(len(lib_locations) == len(lib_list))
    return lib_locations


def process_darwin_libs(libs):
    from delocate import tools
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


class ConnextPyRpm(bdist_rpm, object):
    def _make_spec_file(self):
        import site
        spec = super(ConnextPyRpm, self)._make_spec_file()
        defines = [r'%define debug_package %{nil}',
                   r'%define __requires_exclude ^lib(ndds|rti).*.so']
        idx = [idx for idx, line in enumerate(spec) if '%files' in line][0]
        spec[idx] = '%files'
        sitelib = sysconfig.get_python_lib(plat_specific=1)
        files = [
            sitelib
        ]
        retval = defines + spec[:idx + 2] + files + spec[idx + 2:]
        return retval


class CMakeExtension(Extension):
    def __init__(self, name, dependencies):
        Extension.__init__(self, name, sources=[])
        self.dependencies = dependencies


class CMakeBuild(build_ext):
    def run(self):
        nddshome = get_nddshome()
        arch = get_arch()
        lib_dir = os.path.join(get_script_dir(), 'platform', arch)
        
        # CMake will find same Python major version as the one used for setup
        major_version = sys.version_info.major
        minor_version = sys.version_info.minor

        cfg = 'Debug' if (self.debug or get_debug()) else 'Release'
        cmake_args = ['-DBUILD_SHARED_LIBS=ON',
                      '-DCONNEXTDDS_DIR=' + nddshome,
                      '-DCONNEXTDDS_ARCH=' + arch,
                      '-DCMAKE_BUILD_TYPE=' + cfg,
                      '-Dpybind11_DIR=' + pybind11.get_cmake_dir(),
                      '-DRTI_PYTHON_MAJOR_VERSION=' + str(major_version),
                      '-DRTI_PYTHON_MINOR_VERSION=' + str(minor_version),
                      '-DRTI_PLATFORM_DIR=' + lib_dir]

        cmake_toolchain = get_cmake_toolchain()
        python_root = get_python_root()

        if cmake_toolchain:
            cmake_args += ['-DCMAKE_TOOLCHAIN_FILE=' + cmake_toolchain]
        else:
            cmake_args += ['-DCMAKE_PREFIX_PATH=' + lib_dir]

        if python_root:
            cmake_args += ['-DPython_ROOT_DIR=' + python_root]

        for ext in self.extensions:
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            cmake_args += ['-D' + ext.name.replace('.', '_').upper() + '_LIBRARY_OUTPUT_DIRECTORY=' + extdir]
            self.copy_extension_libs(ext, extdir, arch)

        build_args = ['--config', cfg]

        if 'Win' in arch:
            cmake_args += ['-A', get_cpu(arch)]
            build_args += ['--', '/p:CL_MPcount={}'.format(get_job_count())]
        else:
            cmake_args += ['-DRTI_LINK_OPTIMIZATIONS_ON=1']
            build_args += ['--parallel', str(get_job_count())]

        # handle possible ABI issues when targeting gcc 4.x platforms
        if 'Linux' in arch and 'gcc4' in arch:
            abi_flag = '-D_GLIBCXX_USE_CXX11_ABI=0'
        else:
            abi_flag = ''

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} {} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              abi_flag,
                                                              self.distribution.get_version())

        module_build_dir = os.path.join(self.build_temp, 'connext-py')
        if not os.path.exists(module_build_dir):
            os.makedirs(module_build_dir)
        cmake_cmd = os.path.join(cmake.CMAKE_BIN_DIR, 'cmake')
        subprocess.check_call([cmake_cmd, os.path.abspath(os.path.join(get_script_dir(), 'modules'))] + cmake_args, cwd=module_build_dir, env=env)
        subprocess.check_call([cmake_cmd, '--build', '.'] + build_args, cwd=module_build_dir, env=env)

        # attempt to create interface files for type hinting; failure is not a fatal error
        python_cmd = sys.executable
        stubgen = os.path.join(get_script_dir(), 'resources', 'scripts', 'stubgen.py')
        stubgen_args = ['--split-overload-docs', '--no-setup-py', '--root-module-suffix', '']

        # TODO PY-17: remove this workaround when CSampleWrapper signatures
        # are fixed.
        stubgen_args += ['--ignore-invalid', 'signature']
        extdirs = set()
        for ext in self.extensions:
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            extdirs.add(extdir)
            pkg_components = ext.name.split('.')[:-1]
            builddir = os.path.abspath(os.path.join(extdir, os.sep.join(['..'] * len(pkg_components))))
            stubs_env = os.environ.copy()
            stubs_env['PYTHONPATH'] = builddir
            try:
                with open(os.devnull, 'w') as devnull:
                    subprocess.check_call([python_cmd, stubgen] + stubgen_args + ['-o', extdir, ext.name], env=stubs_env, stderr=devnull)
            except Exception:
                print('Could not generate stub file for {}'.format(ext.name))

        for extdir in extdirs:
            if os.path.isdir(os.path.join(extdir, '__pycache__')):
                shutil.rmtree(os.path.join(extdir, '__pycache__'))

    def copy_extension_libs(self, ext, extdir, arch):
        for lib in ext.dependencies:
            shutil.copyfile(os.path.join(get_script_dir(), 'platform', arch, 'lib', lib), os.path.join(extdir, lib))


package_cfg = process_config()

setup(
    ext_modules=[
            CMakeExtension('rti.connextdds', get_package_libs('rti')),
            CMakeExtension('rti.logging.distlog', get_package_libs('rti.logging')),
            CMakeExtension('rti.request._util_native', get_package_libs('rti.request'))
        ],
    cmdclass=dict(build_ext=CMakeBuild, bdist_rpm=ConnextPyRpm)
)
