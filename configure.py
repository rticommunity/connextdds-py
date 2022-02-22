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

import argparse
import os
import collections
import filecmp
import re
import shutil
import ast
import xml.etree.ElementTree as etree
try:
    import configparser
except ImportError:
    import ConfigParser as configparser


def get_script_dir():
    return os.path.dirname(os.path.realpath(__file__))


def resolve_lib_dir(nddshome, platform):
    lib_dir = os.path.join(nddshome, 'lib', platform)
    if not os.path.isdir(lib_dir):
        raise FileNotFoundError('rti: {} target library directory was not found.'.format(platform))
    return lib_dir


def get_wheel_plugins(args):
    plugins = []
    if args.tcp:
        plugins.append('nddstransporttcp')
    if args.secure:
        plugins.append('nddssecurity')
    if args.monitoring:
        plugins.append('rtimonitoring')
    return plugins


def get_debug_suffix(is_debug):
    return 'd' if is_debug else ''


def dir_type(value):
    if os.path.exists(value) and os.path.isdir(value):
        return value
    else:
        raise FileNotFoundError('rti: {} is not a valid directory'.format(value))


def file_type(value):
    if os.path.exists(value) and os.path.isfile(value):
        return value
    else:
        raise FileNotFoundError('rti: {} is not a valid file'.format(value))


def get_version(nddshome):
    try:
        tree = etree.parse(os.path.join(nddshome, 'rti_versions.xml'))
        root = tree.getroot()
        host = root.find('host')
        assert host is not None
        base_version = host.find('base_version')
        version = base_version.text.split('.')
        major, minor, release = version[0:3]
        return int(major), int(minor), int(release)
    except:
        return None


def get_openssl_lib_set(version, is_win):
    if is_win:
        if version[0] > 6 or (version[0] == 6 and version[1] >= 1):
            return ['libcrypto-1_1-x64']
        elif version [0] == 6 and version [1] == 1:
            return ['libcrypto-1_1-x64', 'libssl-1_1-x64']
        else:
            return ['libeay32', 'libssl32']
    else:
        if version[0] > 6 or (version[0] == 6 and version[1] >= 1):
            return ['crypto']
        else:
            return ['crypto', 'ssl']


def get_openssl_libs(version, arch, arch_dir, openssl_dir, is_debug):
    openssl_libs = []
    if 'Win' in arch:
        lib_ends = rti_platform_ends['Windows']
        openssl_libs = [os.path.join(openssl_dir,
                            lib_ends.prefix +
                            lib_name +
                            get_debug_suffix(is_debug) +
                            lib_ends.suffix)
                            for lib_name in get_openssl_lib_set(version, True)]
    else:
        libs = get_openssl_lib_set(version, False)
        if 'Linux' in arch:
            lib_ends = rti_platform_ends['Linux']
        elif 'Darwin' in arch:
            lib_ends = rti_platform_ends['Darwin']
        else:
            raise ValueError('rti: {} is not currently a supported platform'.format(arch))
        lib_filename = ''.join(
                        [lib_ends.prefix,
                        'nddssecurity',
                        get_debug_suffix(is_debug),
                        lib_ends.suffix])
        secure_lib = os.path.join(arch_dir, lib_filename)
        with open(secure_lib, 'rb') as lib_file:
            content = lib_file.read()
            for lib in libs:
                if version[0] > 6 or (version[0] == 6 and (version[1] >= 1 or version[2] >= 1)):
                    expression = lib_ends.prefix + lib + lib_ends.suffix + r'(\.\d+\.\d+(\.\d+)?)?'
                else:
                    expression = lib_ends.prefix + lib + r'(\.\d+\.\d+(\.\d+)?)?' + lib_ends.suffix
                match = re.search(expression.encode(), content)
                if not match:
                    raise RuntimeError(
                        'rti: could not find versioned openssl library name in security library.')
                lib_location = os.path.join(openssl_dir, match.group(0).decode())
                if not os.path.isfile(lib_location):
                    raise FileNotFoundError('rti: {} not found in {}'.format(
                        os.path.basename(lib_location),
                        openssl_dir))
                openssl_libs.append(lib_location)
    return openssl_libs


def valid_lib_pair(original_lib, dst_lib):
    lib_filename = os.path.basename(original_lib)
    if not (os.path.isfile(dst_lib) and filecmp.cmp(original_lib, dst_lib)):
        return (original_lib, dst_lib)
    else:
        print('INFO: {} already exists in destination, skipping copy...'.format(lib_filename))
        return None


def check_lib(name, arch, lib_ends, src_dir, dst_dir, is_debug):
    lib_filename = lib_ends.prefix + name + get_debug_suffix(is_debug) + lib_ends.suffix
    original_lib = os.path.join(src_dir, lib_filename)
    dst_lib = os.path.join(dst_dir, lib_filename)
    if not os.path.isfile(original_lib):
        raise FileNotFoundError('rti: could not find {} for {}.'.format(lib_filename, arch))
    return valid_lib_pair(original_lib, dst_lib)


def copy_arch_libs(arch, required_libs, plugins, platform_lib_dir, user_plugins, openssl_lib_dir, version, is_debug):
    if 'Linux' in arch:
        lib_ends = rti_platform_ends['Linux']
    elif 'Win' in arch:
        lib_ends = rti_platform_ends['Windows']
    elif 'Darwin' in arch:
        lib_ends = rti_platform_ends['Darwin']
    else:
        raise ValueError('rti: {} is not currently a supported platform'.format(arch))
    copy_libs = []

    dst_dir = os.path.join(get_script_dir(), 'platform', arch, 'lib')
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)

    # Add required libraries
    required_map = collections.defaultdict(lambda: [])
    for package, module_libs in required_libs.items():
        for lib in module_libs:
            required_map[package].append(
                lib_ends.prefix + lib + get_debug_suffix(is_debug) + lib_ends.suffix)
            copy_libs.append(check_lib(lib, arch, lib_ends, platform_lib_dir, dst_dir, is_debug))
            if 'Win' in arch:
                copy_libs.append(
                    check_lib(
                        lib,
                        arch,
                        PlatformLibEnds('', '.lib'),
                        platform_lib_dir,
                        dst_dir,
                        is_debug))

    # Add standard RTI plugins as specified in command args
    plugin_list = []
    if 'Win' in arch:
        import glob
        msvc_lib = glob.glob(os.path.join(platform_lib_dir, 'msvc*.dll'))
        if msvc_lib:
            assert len(msvc_lib) == 1
            basename = os.path.basename(msvc_lib[0])
            plugin_list.append(basename)
            copy_libs.append(
                check_lib(os.path.splitext(basename)[0],
                arch,
                lib_ends,
                platform_lib_dir,
                dst_dir,
                False))
    for plugin in plugins:
        plugin_list.append(lib_ends.prefix + plugin + get_debug_suffix(is_debug) + lib_ends.suffix)
        if plugin == 'nddssecurity':
            # needs to also get openssl libs
            if not openssl_lib_dir:
                openssl_lib_dir = platform_lib_dir
            openssl_libs = get_openssl_libs(version, arch, platform_lib_dir, openssl_lib_dir, is_debug)
            for lib in openssl_libs:
                plugin_list.append(os.path.basename(lib))
                copy_libs.append(valid_lib_pair(lib, os.path.join(dst_dir, os.path.basename(lib))))

        copy_libs.append(check_lib(plugin, arch, lib_ends, platform_lib_dir, dst_dir, is_debug))

    # Add non-standard plugins
    for plugin in user_plugins:
        original_lib = plugin
        dst_lib = os.path.join(dst_dir, os.path.basename(plugin))
        plugin_list.append(os.path.basename(plugin))
        copy_libs.append(valid_lib_pair(original_lib, dst_lib))

    copy_libs = [lib_pair for lib_pair in copy_libs if lib_pair is not None]
    for src, dst in copy_libs:
        print('Copying {}...'.format(os.path.basename(src)))
        shutil.copyfile(src, dst)
    return required_map, list(set(plugin_list))


def update_config(nddshome, platform, jobs, debug, lib_dict, plugins, toolchain, python_root):
    pyproject_filename = 'pyproject.toml'
    manifest_filename = "MANIFEST.in"
    packagecfg_filename = 'package.cfg'

    if toolchain:
        if not python_root:
            raise argparse.ArgumentError('Python root directory must be specified for cross compile')
        toolchain = os.path.abspath(toolchain)

    if python_root:
        python_root = os.path.abspath(python_root)

    lib_dict['rti'].extend(plugins)

    # Modify pyproject.toml template
    config = configparser.ConfigParser()
    config.read(os.path.join(get_script_dir(), 'templates',pyproject_filename))
    requires = ast.literal_eval(config.get('build-system', 'requires'))
    if 'Darwin' in platform:
        requires.append('delocate')
    elif 'Linux' in platform:
        requires.append('patchelf-wrapper')
    config.set('build-system', 'requires', str(requires))
    with open(os.path.join(get_script_dir(), pyproject_filename), 'w') as pyproject_file:
        config.write(pyproject_file)

    # Modify MANIFEST.in template
    manifest_template = os.path.join(get_script_dir(), 'templates', manifest_filename)
    manifest_configured = os.path.join(get_script_dir(), manifest_filename)
    shutil.copyfile(manifest_template, manifest_configured)
    with open(manifest_configured, 'a') as manifest:
        manifest.write('graft {}\n'.format(os.path.join('platform', platform)))

    # Create package.cfg file
    config = configparser.ConfigParser()
    config.add_section('package')
    config.set('package', 'nddshome', nddshome)
    config.set('package', 'platform', platform)
    config.set('package', 'packages', ','.join(lib_dict.keys()))
    for key, value in lib_dict.items():
        config.set('package', 'libraries-' + key, ','.join(value))
    config.set('package', 'build-type', 'debug' if debug else 'release')
    config.set('package', 'build-jobs', str(jobs))
    if toolchain:
        config.set('package', 'cmake-toolchain', toolchain)
    if python_root:
        config.set('package', 'python-root', python_root)
    with open(os.path.join(get_script_dir(), packagecfg_filename), 'w') as packagecfg_file:
        config.write(packagecfg_file)


PlatformLibEnds = collections.namedtuple('PlatformLibEnds', ['prefix', 'suffix'])

rti_platform_ends = {
    'Linux': PlatformLibEnds('lib','.so'),
    'Windows': PlatformLibEnds('', '.dll'),
    'Darwin': PlatformLibEnds('lib', '.dylib')
}


rti_required_libs = {
    'rti': ['nddsc', 'nddscore', 'nddscpp2'],
    'rti.logging': ['rtidlc'],
    'rti.request': ['rticonnextmsgcpp2']
}


def main():
    parser = argparse.ArgumentParser(
        description='Configure build of connextdds-py package',
        add_help=True)

    parser.add_argument(
        '-n',
        '--nddshome',
        type=dir_type,
        default=None,
        help='Location of NDDSHOME.')

    parser.add_argument(
        '-j',
        '--jobs',
        type=int,
        default=1,
        help='Number of concurrent build jobs/processes.')

    parser.add_argument(
        '-t',
        '--tcp',
        action='store_true',
        help='Add the TCP plugin to the output wheel')

    parser.add_argument(
        '-m',
        '--monitoring',
        action='store_true',
        help='Add the RTI Monitoring plugin to the output wheel')

    parser.add_argument(
        '-s',
        '--secure',
        action='store_true',
        help='Add the RTI DDS Secure libraries and dependencies to the output wheel.')

    parser.add_argument(
        '-p',
        '--plugin',
        action='append',
        type=file_type,
        help='Add a user-defined plugin to the output wheel. This option can be specified multiple times.')

    parser.add_argument(
        '-o',
        '--openssl',
        type=dir_type,
        default=None,
        help='Location of openssl lib directory. If not set, build will look for dependencies in NDDSHOME target lib directory.')

    parser.add_argument(
        '-d',
        '--debug',
        action="store_true",
        help='Build a debug wheel.')

    parser.add_argument(
        '-r',
        '--python-root',
        type=dir_type,
        default=None,
        help='Location of Python root directory')

    parser.add_argument(
        '-c',
        '--cmake-toolchain',
        type=file_type,
        default=None,
        help='Location of cmake toolchain file for cross compilation')

    parser.add_argument(
        'platform',
        type=str,
        help='Platform libs to use for the build.')


    args = parser.parse_args()

    if args.nddshome:
        nddshome = args.nddshome
    elif not 'NDDSHOME' in os.environ:
        raise EnvironmentError('configure: NDDSHOME must be in environment or specified as a command option.')
    else:
        nddshome = os.environ['NDDSHOME']

    nddshome = os.path.abspath(nddshome)

    debug = args.debug

    platform = args.platform

    lib_dir = resolve_lib_dir(nddshome, platform)

    version = get_version(nddshome)

    plugins = get_wheel_plugins(args)

    user_plugins = args.plugin if args.plugin is not None else []

    required, plugins = copy_arch_libs(
                            args.platform,
                            rti_required_libs,
                            plugins,
                            lib_dir,
                            user_plugins,
                            args.openssl,
                            version,
                            debug)

    update_config(
        nddshome,
        platform,
        args.jobs,
        debug,
        required,
        plugins,
        args.cmake_toolchain,
        args.python_root)

    print('Finished! Run "pip wheel ." to create whl file.')


if __name__ == '__main__':
    main()
