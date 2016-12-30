import os

directories = [ x[0] for x in os.walk('.')]
directories = [ x for x in directories if x[:3] != './.' ]
directories.remove('./bin')
directories.remove('./lib')
directories = [ x.split('/')[1:] for x in directories if x != '.' ]

lib_roots = list(set([ x[0] for x in directories ]))

env = Environment(
    CPPPATH = os.environ['GOLD_HOME'],
    CXXFLAGS = ["-std=c++0x", '-Wall', '-Werror']
)

for root in lib_roots:
    local_dirs = [ '/'.join(x) for x in directories if x[0] == root]
    lib_list = env.SharedLibrary('lib/' + root, [Glob(d + "/*.cpp") for d in local_dirs] )
libs = lib_roots

for program in Glob('bin/*.cxx'):
    env.Program(program, LIBS=libs, LIBPATH='./lib')
