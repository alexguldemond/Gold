#!/usr/bin/python

import argparse
import CppHeaderParser
import os
import os.path
import re
import sys

def getPublicMethods(cpp_class, header_file) :
    try :
        header = CppHeaderParser.CppHeader(header_file)
    except CppHeaderParser.CppParseError as e:
        print >> sys.stderr, e
        sys.exit(1)
        
    class_tree = header.classes[cpp_class]
    public_methods = []
    for method in class_tree["methods"]["public"]:
        params = [ {
            'type': t['type'], 
            'name': t['name'], 
            'default' : t['defaultValue'] if 'defaultValue' in t.keys() else '' 
        } for t in method["parameters"]] 
        public_methods.append( {
            'name': method["name"], 
            'parameters' : params, 
            'return_type' : method["rtnType"],
            'constructor' : method["constructor"],
            'destructor' : method["destructor"],
        })
    return public_methods

def getParamString(params) :
    param_string = ', '.join([ 
            param['type'] + ' ' +
            param['name'] + 
            (( ' = %s' % (param['default']) ) if param['default'] != '' else '') 
            for param in params
    ] )
    return param_string

def getParamStringNoDefaults(params) :
    param_string = ', '.join([ 
            param['type'] + ' ' +
            param['name']
            for param in params
    ] )
    return param_string

def getArgString(params) :
    param_string = ', '.join([ param['name'] for param in params] )
    return param_string

def printImports(cpp_class, lib_home, cpp_class_file, handle) :
    handle.write('# distutils: language = c++\n')
    handle.write('# distutils: libraries = %s\n' % (cpp_class) )
    handle.write('# distutils: library_dirs = %s\n\n' % (lib_home ) )
    for container in ['string', 'vector', 'map']:
        handle.write("from libcpp.%s cimport %s\n" % (container, container))
        
def printCppClass(cpp_class, py_class, public_methods, header_file, namespace, handle) :
    handle.write('\ncdef extern from "%s" namespace "%s" :\n' % ( header_file, '::'.join(namespace) ) )
    handle.write('\tcdef cppclass %s :\n' % (cpp_class) ) 

    for method in public_methods:
        if method["destructor"] :
            continue
        exception_clause = ' except +' if method["constructor"] else ''
        if method["constructor"]  :
            return_type = ''
        else :
            return_type = method['return_type'] + ' '
        
        params = method['parameters']
        param_string = getParamStringNoDefaults(params)
            
        handle.write(
            '\t\t%s%s(%s)%s\n' % ( 
                return_type,
                method['name'],
                param_string, 
                exception_clause 
            )
        )
    handle.write('\n')
    return public_methods   

def printWrapper( cpp_class, py_class, public_methods, handle) :
    handle.write('cdef class %s :\n' % (py_class) )
    handle.write('\tcdef %s* ptr\n' % (cpp_class) )

    constructors = [ m for m in public_methods if m['constructor']]

    for constructor in constructors :
        parameters = 'self'
        cpp_parameters = ''
        if constructor['parameters'] : 
            cpp_parameters = getParamString(constructor['parameters'])
        if cpp_parameters: 
            parameters += ', ' + cpp_parameters
            cpp_parameters = getArgString(constructor['parameters'])
        handle.write('\tdef __cinit__(%s):\n' % (
            parameters
        ))
        handle.write('\t\tself.ptr = new %s(%s)\n' % (
            cpp_class,
            cpp_parameters
        ))

    for method in public_methods:
        if method['destructor'] or method['constructor']:
            continue
        
        parameters = 'self'
        cpp_parameters = ''
        if method['parameters'] : 
            cpp_parameters = getParamString(method['parameters'])
        if cpp_parameters: 
            parameters += ', ' + cpp_parameters
            cpp_parameters = getArgString(method['parameters'])
            
        handle.write('\tdef %s(%s):\n' % (
            method['name'],
            parameters
        ))
        handle.write('\t\t%sself.ptr.%s(%s)\n' % (
            'return ' if method['return_type'] != 'void' else '',
            method["name"],
            cpp_parameters
        ))            

    handle.write('\tdef __dealloc__(self):\n\t\tdel self.ptr\n')
    
def main() :
    parser = argparse.ArgumentParser(description='Expose a c++ class to python')
    parser.add_argument('full_class', metavar='class', type=str, nargs = 1, help="The class to interface")
    args = parser.parse_args()
    
    gold_home = os.environ['GOLD_HOME']
    
    cpp_class = args.full_class[0]
    namespace = cpp_class.split('::')
    cpp_class = namespace.pop()
    lib_name = cpp_class if len(namespace) <= 1 else namespace[1]
    lib_name = 'lib' + lib_name + '.so'
    lib_file = gold_home + '/Gold/lib/' + lib_name
    header_file = gold_home + '/Gold/cplusplus/' + '/'.join(namespace) + '/' + cpp_class + '/' + cpp_class + '.hpp'
    py_class = cpp_class.title()
    public_methods = getPublicMethods(cpp_class, header_file)
 
    directory = gold_home + '/Gold/cplusplus/' + '/' + "/".join(namespace) + '/' + cpp_class +'/python/'
    pyx_file_name = directory + py_class + '.pyx'
    setup_file_name = directory + '/setup.py'
    
    if not os.path.exists(os.path.dirname(pyx_file_name)):
        try:
            os.makedirs(os.path.dirname(pyx_file_name))
        except OSError as exc: # Guard against race condition
            if exc.errno != errno.EEXIST:
                raise
    pyx_file_handle = open(pyx_file_name, 'w')
    setup_file_handle = open(setup_file_name, 'w')
    printImports(
        cpp_class = cpp_class,
        lib_home = gold_home + '/Gold/lib/', 
        cpp_class_file = gold_home + '/Gold/cplusplus/' + '/' + '/'.join(namespace) + '/' + cpp_class + '/' + cpp_class + '.cpp', 
        handle = pyx_file_handle
    )
    printCppClass(
        cpp_class = cpp_class, 
        py_class = py_class, 
        public_methods = public_methods, 
        namespace = namespace, 
        handle = pyx_file_handle,
        header_file = header_file
    )
    printWrapper(
        cpp_class = cpp_class,
        py_class = py_class,
        public_methods = public_methods,
        handle = pyx_file_handle,
    )
    setup_file_handle.write("""#!/usr/bin/python                        

from distutils.core import setup
from Cython.Build import cythonize
import shutil

setup(
    ext_modules = cythonize('%s')
)

shutil.move('%s', '%s')
""" % (
    pyx_file_name,
    directory + '/' + py_class + '.so',
    gold_home + '/Gold/python/Gold'
) )

if __name__ == "__main__" :
    main()
