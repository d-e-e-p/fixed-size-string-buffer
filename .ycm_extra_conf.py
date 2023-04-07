#
# orig template from:
# from https://github.com/ycm-core/ycmd/blob/master/.ycm_extra_conf.py
#
import os
from os.path import abspath, join, dirname, basename
import ycm_core


DIR_OF_THIS_SCRIPT = os.path.abspath(os.path.dirname(__file__))
DIR_OF_THIRD_PARTY = os.path.join(DIR_OF_THIS_SCRIPT, 'third_party')
SOURCE_EXTENSIONS = ['.cpp', '.cxx', '.cc', '.c', '.m', '.mm']

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
    '-Wall', '-Wextra', '-Werror', '-Wno-long-long', '-Wno-variadic-macros',
    # For a C project, you would set this to 'c' instead of 'c++'.
    '-x', 'c++',
    '-std=c++17',
    # OS X only , but extra paths that don't exist are not harmful
    '-isystem', '/System/Library/Frameworks/Python.framework/Headers',
    '-isystem', '/usr/local/include',
    '-isystem', '/usr/local/include/eigen3',
    '-isystem', '/opt/homebrew/include/',
    '-I', 'include',
    '-I', '../include',
    '-I', '../../include',
    '-I', '../src',
    '-I', '.',
]




def IsHeaderFile( filename ):
  extension = os.path.splitext( filename )[ 1 ]
  return extension in [ '.h', '.hxx', '.hpp', '.hh' ]


def FindCorrespondingSourceFile( filename ):
  if IsHeaderFile( filename ):
    basename = os.path.splitext( filename )[ 0 ]
    for extension in SOURCE_EXTENSIONS:
      replacement_file = basename + extension
      if os.path.exists( replacement_file ):
        return replacement_file
  return filename

#
# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# You can get CMake to generate this file for you by adding:
#   set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
# to your CMakeLists.txt file.
# also see ycmd/ycmd/completers/cpp/flags.py
# youcompleteme has to be build with ./install.py --all
#
# assumes dir structure, eg: 
#   source: fixed-size-string-buffer/bench/source/bench_push.cpp
#   db:     fixed-size-string-buffer/build/bench/compile_commands.json  
# 
def FindCorrespondingCompilationDatabaseFolder( filename ):
    type = basename(dirname(dirname(filename)))
    path = abspath(join(dirname(dirname(dirname(filename))), 'build', type))
    json = join(path, 'compile_commands.json')
    if os.path.isfile(json):
        if hasattr(ycm_core, "CompilationDatabase"):
            database = ycm_core.CompilationDatabase( path )
            return database
    return None


def Settings( **kwargs ):
  if kwargs[ 'language' ] == 'cfamily':
    # If the file is a header, try to find the corresponding source file and
    # retrieve its flags from the compilation database if using one. This is
    # necessary since compilation databases don't have entries for header files.
    # In addition, use this source file as the translation unit. This makes it
    # possible to jump from a declaration in the header file to its definition
    # in the corresponding source file.
    filename = FindCorrespondingSourceFile( kwargs[ 'filename' ] )

    database = FindCorrespondingCompilationDatabaseFolder( kwargs[ 'filename' ] )
    if not database:
      return {
        'flags': flags,
        'include_paths_relative_to_dir': DIR_OF_THIS_SCRIPT,
        'override_filename': filename
      }

    compilation_info = database.GetCompilationInfoForFile( filename )
    if not compilation_info.compiler_flags_:
      return {}

    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object.
    final_flags = list( compilation_info.compiler_flags_ )

    # NOTE: This is just for YouCompleteMe; it's highly likely that your project
    # does NOT need to remove the stdlib flag. DO NOT USE THIS IN YOUR
    # ycm_extra_conf IF YOU'RE NOT 100% SURE YOU NEED IT.
    try:
      final_flags.remove( '-stdlib=libc++' )
    except ValueError:
      pass

    return {
      'flags': final_flags,
      'include_paths_relative_to_dir': compilation_info.compiler_working_dir_,
      'override_filename': filename
    }
  return {}


