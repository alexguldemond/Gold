import os
import os.path

directories = [ x[0] for x in os.walk('cplusplus/Gold')]
directories = [ x for x in directories if x[:3] != './.' ]
directories = [ x.split('/')[1:] for x in directories if x != '.' ]
cpp_directories = [ x for x in directories if 'python' not in x and len(x) > 1]
python_extensions = [ x for x in directories if 'python' in x]

lib_roots = list(set([ x[1] for x in cpp_directories]))
python_paths = [x for x in python_extensions if x[-1] == 'python']

env = Environment(
    CPPPATH = os.environ['GOLD_HOME'] + '/Gold/cplusplus/',
    CXXFLAGS = ["-std=c++0x", '-Wall', '-Werror']
)

#build c++ libraries
libs = []
for root in lib_roots:
    local_dirs = [ 'cplusplus/'+'/'.join(x) for x in cpp_directories if x[1] == root]
    lib_list = env.SharedLibrary('lib/' + root, [Glob(d + "/*.cpp") for d in local_dirs] )
    libs.extend(lib_list)

#build python extensions to c++ libraries
ext_libs = []
ext_lib_dir = 'python/Gold'
for path_list in python_paths:
    path = 'cplusplus/' + '/'.join(path_list)
    setup_script = path + '/setup.py'
    if not os.path.isfile(setup_script) : continue
    install = env.Command('always.install', 
                          [setup_script],
                          'python setup.py build_ext -i',
                          chdir = path
    )
    Alias('install', install)
    ext_libs.extend(install)
Clean(ext_libs, Glob(ext_lib_dir + '/*.so'))

#build executables
execs = []
for program in Glob('bin/*.cxx'):
    program = env.Program(program, LIBS=lib_roots, LIBPATH='./lib')
    execs.extend(program)


Requires(ext_libs, libs)
Requires(execs, libs)
