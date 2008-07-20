#!/bin/python
#
################################################################################
# HCR Library (LGPL) - Copyright (C) 2008 - Michael Levin
#
# The HCR library is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your option)
# any later version.
#
# The HCR library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with the HCR library. If not, see <http://www.gnu.org/licenses/>.
################################################################################
#
# While SCons supports Windows, this SConstruct file is for POSIX systems only!

import glob, os, sys

# Package parameters
package = 'cellwriter'
package_name = 'CellWriter'
package_url = 'http://risujin.org/cellwriter'
version = '2.0.0'

# The library requires its own settings
libhcr_name = 'HCR Library'
libhcr_lib = 'libhcr-1'
libhcr_version = '1.0.0'

# Builder for precompiled headers
gch_builder = Builder(action = '$CC $CFLAGS $CCFLAGS $_CCCOMCOM ' +
                               '-x c-header -c $SOURCE -o $TARGET')

# Create a default environment. Have to set environment variables after
# initialization so that SCons doesn't mess them up.
default_env = Environment(ENV = os.environ, BUILDERS = {'GCH' : gch_builder})

# If the file timestamp has not changed, don't run MD5 test
default_env.Decider('MD5-timestamp')

# Tells SCons to be smarter about caching dependencies
default_env.SetOption('implicit_cache', 1)

# Adds an option that may have been set via environment flags
def AddEnvOption(env, opts, opt_name, opt_desc, env_name = None, value = None):
        if env_name == None:
                env_name = opt_name;
        if env_name == opt_name:
                opt_desc += ' (environment variable)'
        else:
                opt_desc += ' (environment variable ' + env_name + ')'
        if env_name in os.environ:
                opts.Add(opt_name, opt_desc, os.environ[env_name])
        elif value == None:
                opts.Add(opt_name, opt_desc, env.get(opt_name))
        else:
                opts.Add(opt_name, opt_desc, value)

# Options are loaded from 'custom.py'. Default values are only provided for
# the variables that do not get set by SCons itself.
config_file = ARGUMENTS.get('config', 'custom.py')
opts = Options(config_file)

# Load options
AddEnvOption(default_env, opts, 'CC', 'Compiler to use')
AddEnvOption(default_env, opts, 'CFLAGS', 'Compiler flags')
AddEnvOption(default_env, opts, 'LINK', 'Linker to use')
AddEnvOption(default_env, opts, 'LINKFLAGS', 'Linker flags', 'LDFLAGS')
AddEnvOption(default_env, opts, 'PREFIX', 'Installation path prefix',
             'PREFIX', '/usr/local/')
AddEnvOption(default_env, opts, 'icon_path', 'Path to icon dir from share dir',
             'icon_path', '/icons/hicolor/scalable/apps/')
opts.Add(BoolOption('pch', 'Use precompiled headers if possible', True))
opts.Add(BoolOption('dump_env', 'Dump SCons Environment contents', False))
opts.Add(BoolOption('dump_path', 'Dump path enviornment variable', False))
opts.Update(default_env)
opts.Save(config_file, default_env)

# Dump Environment
if default_env['dump_env']:
        print default_env.Dump()
if default_env['dump_path']:
        print default_env['ENV']['PATH']

# Setup the help information
Help('\n' + package.title() + ' ' + version +
""" is built using SCons. See the README file for detailed
instructions.

Options are initialized from the environment first and can then be overridden
using either the command-line or the configuration file. Values only need to
be specified on the command line once and are then saved to the configuration
file. The filename of the configuration file can be specified using the
'config' command-line option:

  scons config='custom.py'

The following options are available:
""")
Help(opts.GenerateHelpText(default_env))

################################################################################
#
# scons cellwriter -- Compile CellWriter
#
################################################################################
cellwriter_src = glob.glob('src/*.c') + glob.glob('src/*/*.c')
cellwriter_env = default_env.Clone()
cellwriter_env.Append(CPPPATH = ['.', 'include', 'src/common'])
cellwriter_env.Append(LIBS = ['m', 'Xtst'])
cellwriter_env.ParseConfig('pkg-config gtk+-2.0 --cflags --libs')
cellwriter_obj = cellwriter_env.Object(cellwriter_src)
cellwriter = cellwriter_env.Program(package, cellwriter_obj)
Default(cellwriter)

# Build the precompiled header as a dependency of the main program. We have
# to manually specify the header dependencies because SCons is too stupid to
# scan the header files on its own.
cellwriter_pch = []
def CellWriterPCH(header, deps = []):
        global cellwriter_pch
        pch = cellwriter_env.GCH(header + '.gch', header)
        cellwriter_env.Depends(pch, deps)
        cellwriter_env.Depends(cellwriter_obj, pch)
        cellwriter_pch += pch
if cellwriter_env['pch'] == 'yes':
        CellWriterPCH('src/common/c_shared.h')

# Generate a config.h with definitions
def WriteConfigH(target, source, env):
        global version, libhcr_version

        # Get subversion revision and add it to the version
        svn_revision = ''
        try:
                in_stream, out_stream = os.popen2('svn info')
                result = out_stream.read()
                in_stream.close()
                out_stream.close()
                start = result.find('Revision: ')
                if start >= 0:
                        end = result.find('\n', start)
                        svn_revision = 'r' + result[start + 10:end]
        except:
                pass
        version += svn_revision
        libhcr_version += svn_revision

        # Write config
        config = open('config.h', 'w')
        config.write('\n/* Package parameters */\n' +
                     '#define PACKAGE "' + package + '"\n' +
                     '#define PACKAGE_NAME "' + package_name + '"\n' +
                     '#define PACKAGE_STRING "' + package_name +
                                                  ' ' + version + '"\n' +
                     '\n/* Configured paths */\n' +
                     '#define DATADIR "' + install_share + '"\n' +
                     '#define PKGDATADIR "' + install_data + '"\n' +
                     '\n/* CellWriter */\n' +
                     '#define CELLWRITER_URL "' + package_url + '"\n' +
                     '#define ICON_PATH "' + cellwriter_env['icon_path'] +
                                             '"\n' +
                     '\n/* HCR Library */\n' +
                     '#define LIBHCR_NAME "' + libhcr_name + '"\n' +
                     '#define LIBHCR_LIB "' + libhcr_lib + '"\n' +
                     '#define LIBHCR_VERSION "' + libhcr_version + '"\n')
        config.close()

cellwriter_config = cellwriter_env.Command('config.h', '', WriteConfigH)
cellwriter_env.Depends(cellwriter_config, 'SConstruct')

cellwriter_env.Depends(cellwriter_obj + cellwriter_pch, cellwriter_config)
cellwriter_env.Depends(cellwriter_config, config_file)

################################################################################
#
# scons install -- Install CellWriter
#
################################################################################
install_prefix = os.path.join(os.getcwd(), cellwriter_env['PREFIX'])
install_share = os.path.join(install_prefix, 'share')
install_data = os.path.join(install_share, 'cellwriter')
default_env.Alias('install', install_prefix)
default_env.Depends('install', cellwriter)

def InstallRecursive(dest, src, exclude = []):
        bad_exts = ['o', 'gch']
        files = os.listdir(src)
        for f in files:
                ext = f.rsplit('.')
                ext = ext[len(ext) - 1]
                if (f[0] == '.' or ext in bad_exts):
                        continue
                src_path = os.path.join(src, f)
                if src_path in exclude:
                        continue
                if (os.path.isdir(src_path)):
                        InstallRecursive(os.path.join(dest, f), src_path)
                        continue
                default_env.Install(dest, src_path)

# Files that get installed
default_env.Install(os.path.join(install_prefix, 'bin'), cellwriter)
default_env.Install(install_data, ['AUTHORS', 'ChangeLog', 'GPL', 'README'])
InstallRecursive(install_data, 'share/cellwriter')
InstallRecursive(os.path.join(install_share, 'icons'), 'share/icons')
InstallRecursive(os.path.join(install_share, 'pixmaps'), 'share/pixmaps')

################################################################################
#
# scons dist -- Distributing the source package tarball
#
################################################################################

# The compression command depends on the target system
compress_cmd = 'tar -czf '
compress_suffix = '.tar.gz'

dist_name = package + '-' + version
dist_tarball = dist_name + compress_suffix
default_env.Command(dist_tarball, dist_name,
                    [compress_cmd + dist_tarball + ' ' + dist_name])
default_env.Alias('dist', dist_tarball)
default_env.AddPostAction(dist_tarball, Delete(dist_name))

# Files that get distributed in a source package
default_env.Install(dist_name, ['AUTHORS', 'ChangeLog', 'GPL', 'LGPL',
                                'README', 'SConstruct', 'todo.sh',
                                'Makefile', 'cellwriter.1',
                                'cellwriter.desktop'])
InstallRecursive(os.path.join(dist_name, 'libhcr'), 'libhcr')
InstallRecursive(os.path.join(dist_name, 'share'), 'share')
InstallRecursive(os.path.join(dist_name, 'src'), 'src')

