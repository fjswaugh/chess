# Partially stolen from https://bitbucket.org/mblum/libgp/src/2537ea7329ef/.ycm_extra_conf.py
import os
import ycm_core

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
        '-Wall',
        '-Wextra',
        #'-Werror',
        #'-Wno-long-long',
        #'-Wno-variadic-macros',
        #'-fexceptions',
        '-std=c++1z',
        # The -x option specifies the language that the files to be compiled
        # are written in.
        '-x', 'c++',

        '-I', 'include',
        '-I', 'inc',
        '-I', '.'
]

def genPackageConfig(flags, package_name):
    def not_whitespace(string):
       return not (string == '' or string == '\n')
    stream = os.popen("pkg-config --cflags " + package_name)
    includes = stream.read().split(' ')
    includes = filter(not_whitespace, includes)
    return flags + includes

flags = genPackageConfig(flags, 'gtkmm-3.0')

# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
compilation_database_folder = ''

if compilation_database_folder:
        database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
        database = None


def DirectoryOfThisScript():
        return os.path.dirname( os.path.abspath( __file__ ) )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
        if not working_directory:
                return list( flags )
        new_flags = []
        make_next_absolute = False
        path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]
        for flag in flags:
                new_flag = flag

                if make_next_absolute:
                        make_next_absolute = False
                        if not flag.startswith( '/' ):
                                new_flag = os.path.join( working_directory, flag )

                for path_flag in path_flags:
                        if flag == path_flag:
                                make_next_absolute = True
                                break

                        if flag.startswith( path_flag ):
                                path = flag[ len( path_flag ): ]
                                new_flag = path_flag + os.path.join( working_directory, path )
                                break

                if new_flag:
                        new_flags.append( new_flag )
        return new_flags


def FlagsForFile( filename ):
        if database:
                # Bear in mind that compilation_info.compiler_flags_ does NOT return a
                # python list, but a "list-like" StringVec object
                compilation_info = database.GetCompilationInfoForFile( filename )
                final_flags = MakeRelativePathsInFlagsAbsolute(
                        compilation_info.compiler_flags_,
                        compilation_info.compiler_working_dir_ )
        else:
                relative_to = DirectoryOfThisScript()
                final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )

        return {
                'flags': final_flags,
                'do_cache': True
        }
