#!/usr/bin/env python

# __configure.bak.py

# Copyright (c) 2012 Minor Gordon
# All rights reserved

# This source file is part of the Yield project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the Yield project nor the
# names of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

assert __name__ == '__main__'

from binascii import crc32
from copy import copy, deepcopy
from optparse import OptionParser
from os import chdir, getcwd, listdir, makedirs, stat, unlink
from os.path import abspath, dirname, exists, isdir, isfile, join as path_join, \
    sep as path_sep, split as path_split, splitext
from yuild import Makefile, TopLevelMakefile, Project, SourceFiles, VCXProj, \
    VCXProjFilters, VCXProjUser, VCXSln
from yuild.autoconf import Checks, HeaderCheck, LibraryCheck
from yuild.constant import C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS, \
    C_CXX_FILE_FNMATCH_PATTERNS, CXX_SOURCE_FILE_FNMATCH_PATTERNS, \
    INCLUDE_FILE_FNMATCH_PATTERNS, INDENT_SPACES, PLATFORMS, PLATFORM_GUARDS, \
    SOURCE_FILE_FNMATCH_PATTERNS
from yutil import copy_file, deduplist, fnmatch, indent, list_subdirectories, \
    mirror_subdirectories, rglob, write_file
import re
import subprocess
import sys
import traceback


# Paths
AUTHOR = 'Minor Gordon'
YIELD_ROOT_DIR_PATH = dirname(__file__)
YIELD_INCLUDE_DIR_PATH = path_join(YIELD_ROOT_DIR_PATH, 'include')
YIELD_SRC_DIR_PATH = path_join(YIELD_ROOT_DIR_PATH, 'src')
YIELD_TEST_DIR_PATH = path_join(YIELD_ROOT_DIR_PATH, 'test')

YUILD_ROOT_DIR_PATH = abspath(path_join(YIELD_ROOT_DIR_PATH, '..', 'yuild'))
YUILD_SRC_DIR_PATH = path_join(YUILD_ROOT_DIR_PATH, 'src')

YUTIL_ROOT_DIR_PATH = abspath(path_join(YIELD_ROOT_DIR_PATH, '..', 'yutil'))
YUTIL_SRC_DIR_PATH = path_join(YUTIL_ROOT_DIR_PATH, 'src')


# Conditional imports
try:
    import yutil
except ImportError:
    sys.path.append(YUTIL_SRC_DIR_PATH)

try:
    import yuild
except ImportError:
    sys.path.append(YUILD_SRC_DIR_PATH)


# Constants
AUTHOR = 'Minor Gordon'


# Parse options
option_parser = OptionParser()
option_parser.add_option('--calculate-type-id')
option_parser.add_option('-f', '--force', action='store_true')
option_parser.add_option('--format-src', action='store_true')
option_parser.add_option('-p', '--project', action='append', dest='project_names')
option_parser.add_option('--generate-dist', action='store_true')
option_parser.add_option('--generate-proj', action='store_true')
option_parser.add_option('--generate-src', action='store_true')
option_parser.add_option('--generate-src-rl', action='store_true')
option_parser.add_option('--generate-test', action='store_true')
option_parser.add_option('--generate-test_py', action='store_true')

options, _ = option_parser.parse_args()

if not options.calculate_type_id and\
   not options.format_src and\
   not options.generate_dist and\
   not options.generate_proj and\
   not options.generate_src and\
   not options.generate_src_rl and\
   not options.generate_test and\
   not options.generate_test_py:
    # No options set
    options.format_src = True
    options.generate_proj = True




PROJECT_REFERENCES = {
    'yield': tuple(),
    'yield.fs': ('yield.i18n',),
    'yield.fs.poll': ('yield.fs', 'yield.queue',),
    'yield.http': ('yield.uri',),
    'yield.http.server': ('yield.fs', 'yield.http', 'yield.sockets.aio', 'yield.stage',),
    'yield.http.server.file': ('yield.fs', 'yield.http.server'),
    'yield.http.server.ygi': ('yield.http.server',),
    'yield.i18n': ('yield',),
    'yield.poll': ('yield.queue',),
    'yield.queue': ('yield.thread',),
    'yield.sockets': ('yield',),
    'yield.sockets.aio': ('yield.poll', 'yield.sockets',),
    'yield.stage': ('yield.thread',),
    'yield.thread': ('yield',),
    'yield.uri': ('yield',),
}

if options.generate_dist or options.generate_proj:
    # Do autoconf
    if Checks({
           '*': (HeaderCheck('openssl/ssl.h'),),
           'unix': (LibraryCheck('crypto'), LibraryCheck('ssl'))
       })():
        PROJECT_REFERENCES['yield.sockets.ssl'] = ('yield.sockets',)

PROJECT_NAMES = PROJECT_REFERENCES.keys()

if options.project_names is not None and len(options.project_names) > 0:
    for project_name in options.project_names:
        assert project_name in PROJECT_NAMES, project_name
else:
    options.project_names = PROJECT_NAMES

PROJECT_REFERENCES['yield.poll_test'] = ('yield.poll', 'yield.sockets')
PROJECT_REFERENCES['yield.sockets_test'] = ('yield.fs', 'yield.sockets')
PROJECT_REFERENCES['yield.sockets.aio_test'] = ('yield.fs', 'yield.sockets.aio')

CXXDEFINES = {
   'yield.sockets.ssl': {
        '*': ['YIELD_HAVE_OPENSSL']
    }
}

CXXFLAGS = {
    'unix': [
        '-fno-rtti',
        '-Wall',
        '-Wextra',
        '-Wfloat-equal',
        '-Winit-self',
        '-Winline',
        '-Wold-style-cast',
        # '-Wpadded',
        '-Wunused',
    ],
    'win32': [
      '/EHsc', # Enable exceptions
      '/GR-', # Disable RTTI
      '/MP', # Multiprocess compilation
      '/nologo',
      '/wd\"4100\"',
      '/wd\"4127\"',
      '/wd\"4290\"',
      '/wd\"4355\"',
      '/wd\"4512\"',
      # "/Za" # Disable language extensions
    ]
}
for platform in CXXFLAGS.iterkeys():
    assert platform in PLATFORMS, platform

CXXPATH = {
    '*': [YIELD_INCLUDE_DIR_PATH],
    'freebsd': ['/usr/local/include']
}
for platform in CXXPATH.iterkeys():
    assert platform in PLATFORMS, platform

LDFLAGS = {
    'win32': [
        '/ignore:4006',
        '/ignore:4221'
    ],
}
for platform in LDFLAGS.iterkeys():
    assert platform in PLATFORMS, platform


LIBPATH = {
    '*': [path_join(YIELD_ROOT_DIR_PATH, 'lib', 'yield')],
    'freebsd': ['/usr/local/lib']
}
for platform in LIBPATH.iterkeys():
    assert platform in PLATFORMS, platform

LIBS = {
    'yield': {
        'linux': ['rt', 'stdc++'],
        'sunos': ['m', 'rt', 'stdc++'],
    },

    'yield.i18n': {
        'darwin': ['iconv'],
        'freebsd': ['iconv', 'intl'],
        'sunos': ['iconv'],
    },

    'yield.sockets': {
        'sunos': ['nsl', 'socket'],
    },

    'yield.sockets.ssl': {
        'unix': ['crypto', 'ssl']
    },

    'yield.thread': {
        'freebsd': ['pthread'],
        'linux': ['pthread'],
        'sunos': ['kstat'],
    },

#    'yield.uuid': {
#        'sunos': ['uuid'],
#    },
}
for project_name in PROJECT_NAMES:
    project_test_libs = LIBS.setdefault(project_name + '_test', {})
    project_test_libs['linux'] = ['pthread']
    project_test_libs['win32'] = ['gtestd']
    project_test_libs['unix'] = ['gtest']

OUTPUT_FILE_PATHS = {}
for project_name in PROJECT_NAMES:
    OUTPUT_FILE_PATHS[project_name] = \
        path_join(YIELD_ROOT_DIR_PATH, 'lib', 'yield', '_'.join(project_name.split('.')))
    OUTPUT_FILE_PATHS[project_name + '_test'] = \
        path_join(YIELD_ROOT_DIR_PATH, 'bin', 'yield', '_'.join(project_name.split('.')) + '_test')

PROJECT_DIR_PATHS = {}
for project_name in PROJECT_NAMES:
    project_dir_path = path_join(YIELD_ROOT_DIR_PATH, 'proj', path_sep.join(project_name.split('.')))
    PROJECT_DIR_PATHS[project_name] = project_dir_path
    PROJECT_DIR_PATHS[project_name + '_test'] = project_dir_path

for project_name in PROJECT_NAMES:
    PROJECT_REFERENCES.setdefault(project_name + '_test', [project_name])

# Expand project references so that the list has dependencies going left -> right, e.g.
# if project A depends on project B depends on project C, the order will be [A, B, C]
def __expand_project_references(project_name):
    project_references = []
    for project_reference in PROJECT_REFERENCES[project_name]:
        project_references.append(project_reference)
        for project_reference in __expand_project_references(project_reference):
            try:
                del project_references[project_references.index(project_reference)]
            except ValueError:
                pass
            project_references.append(project_reference)
    return project_references
PROJECT_REFERENCES_EXPANDED = {}
for project_name in PROJECT_REFERENCES.iterkeys():
    PROJECT_REFERENCES_EXPANDED[project_name] = \
        deduplist(__expand_project_references(project_name))


# Construct LIBS to get the right link order on Unix, namely requirements flow
# left to right (if A depends on B, the link order should be A B)
LIBS_EXPANDED = {}
for project_name, project_references in PROJECT_REFERENCES_EXPANDED.iteritems():
    LIBS_EXPANDED[project_name] = {}

    try:
        for platform, platform_libs in LIBS[project_name].iteritems():
            LIBS_EXPANDED[project_name].setdefault(platform, []).extend(platform_libs)
    except KeyError:
        pass

    # Add the project references' outputs to the project's libs
    for project_reference in project_references:
        if project_reference != 'yield.queue':
            project_reference_lib = path_split(OUTPUT_FILE_PATHS[project_reference])[-1]
            LIBS_EXPANDED[project_name].setdefault('*', [])
            assert project_reference_lib not in LIBS_EXPANDED[project_name]['*'], LIBS_EXPANDED[project_name]['*']
            LIBS_EXPANDED[project_name]['*'].append(project_reference_lib)

    # Add all of the project references' libs to the project's libs
    for project_reference in project_references:
        try:
            for platform, platform_libs in LIBS[project_reference].iteritems():
                LIBS_EXPANDED[project_name].setdefault(platform, []).extend(platform_libs)
        except KeyError:
            pass

    # dedup
    for platform in LIBS_EXPANDED[project_name].iterkeys():
        LIBS_EXPANDED[project_name][platform] = deduplist(LIBS_EXPANDED[project_name][platform])
LIBS = LIBS_EXPANDED


INCLUDE_FILE_PATHS = {}
SOURCE_DIR_PATHS = {}
SOURCE_FILE_PATHS = {}

for project_name in PROJECT_NAMES:
    project_include_dir_path = path_join(YIELD_INCLUDE_DIR_PATH, path_sep.join(project_name.split('.')))
    INCLUDE_FILE_PATHS[project_name] = []
    for platform in list(PLATFORMS) + ['']:
        if exists(path_join(project_include_dir_path, platform)):
            for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS:
                INCLUDE_FILE_PATHS[project_name].extend(
                    list(rglob(path_join(project_include_dir_path, platform, fnmatch_pattern)))
                )
    INCLUDE_FILE_PATHS[project_name].sort()
    INCLUDE_FILE_PATHS[project_name + '_test'] = []

    for project_name_, project_source_dir_path in (
            (project_name, path_join(YIELD_SRC_DIR_PATH, path_sep.join(project_name.split('.')))),
            (project_name + '_test', path_join(YIELD_TEST_DIR_PATH, path_sep.join(project_name.split('.'))))
        ):
        SOURCE_DIR_PATHS[project_name_] = project_source_dir_path
        SOURCE_FILE_PATHS[project_name_] = []
        for platform in list(PLATFORMS) + ['']:
            if exists(path_join(project_source_dir_path, platform)):
                for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS + SOURCE_FILE_FNMATCH_PATTERNS:
                    SOURCE_FILE_PATHS[project_name_].extend(
                        list(rglob(path_join(project_source_dir_path, platform, fnmatch_pattern)))
                    )
        SOURCE_FILE_PATHS[project_name_].sort()


INCLUDE_FILE_PATHS['yield'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'ygi.h'),
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'config.hpp'), # before types
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'types.hpp'), # before atomic
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'atomic.hpp'), # before object
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'object.hpp'), # before event
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'time.hpp'), # before event_queue
] + \
INCLUDE_FILE_PATHS['yield']

INCLUDE_FILE_PATHS['yield.fs'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'fs', 'path.hpp'), # before directory
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'fs', 'stat.hpp'), # before directory
] + \
INCLUDE_FILE_PATHS['yield.fs']

INCLUDE_FILE_PATHS['yield.http'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'http', 'http_message.hpp'), # before http_response
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'http', 'http_response.hpp'), # before http_request_parser
] + \
INCLUDE_FILE_PATHS['yield.http']

INCLUDE_FILE_PATHS['yield.queue'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'queue', 'synchronized_queue.hpp'), # before synchronized_event_queue
] + \
INCLUDE_FILE_PATHS['yield.queue']

INCLUDE_FILE_PATHS['yield.sockets'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'sockets', 'socket_address.hpp'), # before socket
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'sockets', 'socket.hpp'), # before datagram_socket
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'sockets', 'socket_pair.hpp'), # before datagram_socket_pair
] + \
INCLUDE_FILE_PATHS['yield.sockets']

INCLUDE_FILE_PATHS['yield.sockets.aio'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'sockets', 'aio', 'aiocb.hpp'), # before accept_aiocb
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'sockets', 'aio', 'nbio_queue.hpp'), # before aio_queue
] + \
INCLUDE_FILE_PATHS['yield.sockets.aio']

if INCLUDE_FILE_PATHS.has_key('yield.sockets.ssl'):
    INCLUDE_FILE_PATHS['yield.sockets.ssl'] = [
        path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'sockets', 'ssl', 'ssl_version.hpp'), # before ssl_context
    ] + \
    INCLUDE_FILE_PATHS['yield.sockets.ssl']

INCLUDE_FILE_PATHS['yield.stage'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'stage', 'stage_scheduler.hpp'), # before *_stage_scheduler
] + \
INCLUDE_FILE_PATHS['yield.stage']

INCLUDE_FILE_PATHS['yield.thread'] = [
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'thread', 'mutex.hpp'), # before condition_variable
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'thread', 'lightweight_mutex.hpp'), # before condition_variable
    path_join(YIELD_INCLUDE_DIR_PATH, 'yield', 'thread', 'semaphore.hpp'), # before condition_variable
] + \
INCLUDE_FILE_PATHS['yield.thread']

SOURCE_FILE_PATHS['yield.fs.poll'] = [
    path_join(YIELD_SRC_DIR_PATH, 'yield', 'fs', 'poll', 'watch.hpp'), # before scanning_watch
    path_join(YIELD_SRC_DIR_PATH, 'yield', 'fs', 'poll', 'scanning_watch.hpp'), # before scanning_directory_watch
    path_join(YIELD_SRC_DIR_PATH, 'yield', 'fs', 'poll', 'bsd', 'watch.hpp'), # before directory_watch
    path_join(YIELD_SRC_DIR_PATH, 'yield', 'fs', 'poll', 'win32', 'watch.hpp'), # before directory_watch
] + \
SOURCE_FILE_PATHS['yield.fs.poll']

for project_name in INCLUDE_FILE_PATHS.iterkeys():
    for file_paths in (INCLUDE_FILE_PATHS, SOURCE_FILE_PATHS):
        file_paths[project_name] = deduplist(file_paths[project_name])
        for file_path in file_paths[project_name]:
            if not exists(file_path):
                raise ValueError, file_path + ' does not exist'

THIRD_PARTY_SOURCE_FNMATCH_PATTERNS = (
    'charProps.c',
    'genx.*',
    'pyparsing.py',
    'rapidxml.hpp',
    'SimpleOpt.h',
    'yajl.*',
)




if options.calculate_type_id:
    type_name = options.calculate_type_id
    print str(crc32(type_name) & 0xffffffff)


if options.generate_dist:
    PROJECT_BUILD_ORDER = []
    def __fill_project_build_order(project_name):
        for project_reference in PROJECT_REFERENCES[project_name]:
            __fill_project_build_order(project_reference)

        if project_name not in PROJECT_BUILD_ORDER:
            PROJECT_BUILD_ORDER.append(project_name)
    for project_name in PROJECT_NAMES:
        __fill_project_build_order(project_name)

    ypy_dir_path = path_join(YIELD_ROOT_DIR_PATH, '..', 'ypy')
    if not exists(ypy_dir_path):
        ypy_dir_path = None

    for project_name in ('yield',):#options.project_names:
        hpp = []
        cpp = ['#include "%s.hpp"\n\n\n' % '_'.join(project_name.split('.'))]

        if project_name == 'yield':
            project_references = PROJECT_BUILD_ORDER
        else:
            project_references = PROJECT_BUILD_ORDER[:PROJECT_BUILD_ORDER.index(project_name) + 1]

        for project_reference in project_references:
            include_files = SourceFiles(INCLUDE_FILE_PATHS[project_reference])
            include_files = include_files.filter(C_CXX_INCLUDE_FILE_FNMATCH_PATTERNS)
            hpp.append(include_files.get_combined_repr())
            source_files = SourceFiles(SOURCE_FILE_PATHS[project_reference])
            source_files = source_files.filter(C_CXX_FILE_FNMATCH_PATTERNS)
            source_files = source_files.exclude('test_*')
            source_files = source_files.exclude('*_test*')
            source_files = source_files.exclude('ygi.h')
            cpp.append(source_files.get_combined_repr())

        project_dist_dir_path = \
            path_join(YIELD_ROOT_DIR_PATH, 'dist')#, path_sep.join(project_name.split('.')))
        if not exists(project_dist_dir_path):
            makedirs(project_dist_dir_path)

        for file_ext in ('cpp', 'hpp'):
            file_path = \
                path_join(
                    project_dist_dir_path,
                    '_'.join(project_name.split('.')) + '.' + file_ext
                )

            write_file(file_path, locals()[file_ext], force=options.force)

            if ypy_dir_path is not None:
                ypy_file_path = \
                    path_join(
                        ypy_dir_path,
                        'share',
                        'yield',
                        'dist',
                        #path_sep.join(project_name.split('.')),
                        path_split(file_path)[1]
                    )

                if exists(ypy_file_path):
                    copy_file(file_path, ypy_file_path)

    if ypy_dir_path is not None:
        copy_file(
            path_join(YIELD_INCLUDE_DIR_PATH, 'ygi.h'),
            path_join(
                ypy_dir_path,
                'share',
                'yield',
                'dist',
                'ygi.h'
            )
        )

if options.format_src:
    source_paths = {
        'Yield': (
            path_join(YIELD_ROOT_DIR_PATH, 'include'),
            path_join(YIELD_ROOT_DIR_PATH, 'src', 'yield'),
            path_join(YIELD_ROOT_DIR_PATH, 'test', 'yield')
        ),
        'yutil': (
            path_join(YUTIL_ROOT_DIR_PATH, 'src', 'yutil.py'),
            path_join(YUTIL_ROOT_DIR_PATH, 'test', 'yutil_test.py'),
        ),
        'yuild': (
            path_join(YUILD_ROOT_DIR_PATH, 'src', 'yuild'),
            path_join(YUILD_ROOT_DIR_PATH, 'test', 'yuild_test'),
        )
    }

    for project, source_paths in source_paths.iteritems():
        source_files = SourceFiles()
        for source_path in source_paths:
            if isdir(source_path):
                for fnmatch_pattern in INCLUDE_FILE_FNMATCH_PATTERNS + \
                                       SOURCE_FILE_FNMATCH_PATTERNS:
                    for source_file_path in rglob(path_join(source_path, '**', fnmatch_pattern)):
                        source_files.append(source_file_path)
            else:
                assert isfile(source_path), source_path
                source_files.append(source_path)

        source_files = source_files.exclude(THIRD_PARTY_SOURCE_FNMATCH_PATTERNS)

        source_files.format(
            author=AUTHOR,
            force=options.force,
            project=project,
        )


if options.generate_proj:
    makefiles = []
    vcxprojs = []

    for project_name in options.project_names:
        for project_name_suffix, project_type in (
            ('', Project.TYPE_STATIC_LIBRARY),
            ('_test', Project.TYPE_PROGRAM)
        ):
            project = {
               # the _test build_dir_path has to be a separate directory on Win32 to avoid
               #   overwriting build logs and such (which causes incremental build problems)
               # On other systems build_dir_paths should be the same for a project
               #   in order to get proper coverage results.
               'build_dir_path': {
                   '*': path_join(YIELD_ROOT_DIR_PATH, 'build', path_sep.join(project_name.split('.'))),
                   'win32': path_join(YIELD_ROOT_DIR_PATH, 'build', path_sep.join(project_name.split('.'))) + project_name_suffix,
               },
               'cxxdefines': CXXDEFINES.get(project_name, {}),
               'cxxflags': CXXFLAGS,
               'cxxpath': CXXPATH,
               'include_file_paths': INCLUDE_FILE_PATHS[project_name + project_name_suffix],
               'libpath': LIBPATH,
               'libs': LIBS.get(project_name + project_name_suffix, {}),
               'ldflags': LDFLAGS,
               'name': project_name + project_name_suffix,
               'output_file_path': OUTPUT_FILE_PATHS[project_name + project_name_suffix],
               'project_dir_path': PROJECT_DIR_PATHS[project_name + project_name_suffix],
               'project_references': [
                    path_join(PROJECT_DIR_PATHS[project_reference], project_reference)
                    for project_reference in PROJECT_REFERENCES[project_name + project_name_suffix]
                ],
               'source_dir_path': SOURCE_DIR_PATHS[project_name + project_name_suffix],
               'source_file_paths': SOURCE_FILE_PATHS[project_name + project_name_suffix],
               'type': project_type
            }

            if sys.platform == 'win32':
                try:
                    makedirs(project['build_dir_path']['win32'])
                except:
                    pass
                if project_name != 'yield':
                    mirror_subdirectories(
                        project['source_dir_path'],
                        project['build_dir_path']['win32']
                    )

                try:
                    makedirs(project['project_dir_path'])
                except:
                    pass

            for project_class, file_ext in (
                (Makefile, '.Makefile'),
                (VCXProj, '.vcxproj'),
                (VCXProjFilters, '.vcxproj.filters'),
                (VCXProjUser, '.vcxproj.user'),
            ):
                project_instance = project_class(**project)

                if project_class == Makefile:
                    makefiles.append(project_instance)
                elif project_class == VCXProj:
                    vcxprojs.append(project_instance)

                write_file(
                    path_join(
                        project['project_dir_path'],
                        project['name'] + file_ext
                    ),
                    str(project_instance),
                    force=options.force
                )

    if len(options.project_names) == len(PROJECT_NAMES):
        write_file(
            'yield.sln',
            str(VCXSln(vcxprojs, 'yield.sln')),
            force=options.force,
            newline='\r\n'
        )

        write_file(
            'Makefile',
            str(TopLevelMakefile(makefiles)),
            force=options.force
        )


if options.generate_src or options.generate_src_rl:
    for rl_file_path in rglob(path_join(YIELD_ROOT_DIR_PATH, 'src', 'yield', '**', '*.rl')):
        cpp_file_path = \
            path_join(
                dirname(rl_file_path),
                path_split(rl_file_path)[1][:-2] + 'cpp'
            )

        if exists(cpp_file_path):
            if options.force or stat(rl_file_path).st_mtime > stat(cpp_file_path).st_mtime:
                args = ['ragel']
                args.extend(('-I', YIELD_INCLUDE_DIR_PATH))
                args.append('-L')
                args.extend(('-o', cpp_file_path))
                args.append(rl_file_path)
                # print args
                subprocess.call(args)

                SourceFiles(cpp_file_path).format(
                    author=AUTHOR,
                    force=options.force,
                    project='Yield'
                )


if options.generate_test or options.generate_test_py:
    if not '.' in sys.path:
        sys.path.append('.')

    for test_py_file_path in rglob(path_join(YIELD_TEST_DIR_PATH, 'yield', '**', '*_test.py')):
        try:
            unlink(splitext(test_py_file_path)[0] + '.pyc')
        except:
            pass
        stdoutdata, stderrdata = \
            subprocess.Popen(
                [sys.executable, test_py_file_path],
                stdout=subprocess.PIPE
            ).communicate()
        if len(stdoutdata) > 0:
            test_cpp_file_path = splitext(test_py_file_path)[0] + '.cpp'
            if write_file(
                    test_cpp_file_path,
                    stdoutdata,
                    force=options.force,
                    quiet=True
                ):
                SourceFiles(test_cpp_file_path).format(
                    author=AUTHOR,
                    force=options.force,
                    project='Yield'
                )
