import os

directories = [ x[0] for x in os.walk('.')]
directories = [ x for x in directories if x[:3] != './.' ]
directories = [ x for x in directories if x != '.' ]
directories.remove('./bin')

include = [ x.replace('.','Gold') for x in directories ]

env = Environment(CPPPATH = include)
for d in directories:
    env.Object(Glob(d + '/' + '*.cpp'))
