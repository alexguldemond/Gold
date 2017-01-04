import os
import os.path

directories = [ x[0] for x in os.walk('.')]
directories = [ x for x in directories if x[:3] != './.' ]
directories.remove('./bin')
if './lib' in directories: directories.remove('./lib')
if './html' in directories: directories.remove('./html')
if './latex' in directories: directories.remove('./latex')
directories = [ x.split('/')[1:] for x in directories if x != '.' ]
cpp_directories = [ x for x in directories if 'python' not in x ]
python_extensions = [ x for x in directories if 'python' in x]

lib_roots = list(set([ x[0] for x in cpp_directories ]))
python_paths = [x for x in python_extensions if x[-1] == 'python']

env = Environment(
    CPPPATH = os.environ['GOLD_HOME'],
    CXXFLAGS = ["-std=c++0x", '-Wall', '-Werror']
)

for root in lib_roots:
    local_dirs = [ '/'.join(x) for x in cpp_directories if x[0] == root]
    lib_list = env.SharedLibrary('lib/' + root, [Glob(d + "/*.cpp") for d in local_dirs] )
libs = lib_roots

for path_list in python_paths:
    path = '/'.join(path_list)
    setup_script = path + '/setup.py'
    if not os.path.isfile(setup_script) : continue
    install = env.Command('always.install', 
                          [setup_script],
                          'python setup.py build_ext -i',
                          chdir = path
    )
    Alias('install', install)

for program in Glob('bin/*.cxx'):
    env.Program(program, LIBS=libs, LIBPATH='./lib')

