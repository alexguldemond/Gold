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
    except CppHeaderParser.CppParserError as e:
        print >> sys.stderr, e
        return 1
        
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

def printImports(handle) :
    for container in ['string', 'vector', 'map']:
        handle.write("from libcpp.%s import %s\n" % (container, container))
        
def printCppClass(cpp_class, py_class, public_methods, header_file, namespace, handle) :
    handle.write('\ncdef extern from "%s" namespace "%s" :\n' % ( header_file, '::'.join(namespace) ) )
    handle.write('\tcdef cppclass %s :\n' % (cpp_class) ) 

    for method in public_methods:
        exception_clause = ' except +' if method["constructor"] else ''
        if method["constructor"] or method["destructor"] :
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

    for method in public_methods:
        if method['destructor'] or method['constructor']:
            continue
        else:
            name = method['name']
        
        parameters = 'self'
        cpp_parameters = ''
        if method['parameters'] : 
            cpp_parameters = getParamString(method['parameters'])
        if cpp_parameters: 
            parameters += ', ' + cpp_parameters
            cpp_parameters = getArgString(method['parameters'])
            
        handle.write('\tdef %s(%s):\n' % (
            name,
            parameters
        ))
        handle.write('\t\treturn self.ptr.%s(%s)\n' % (
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
    header_file = gold_home + '/'.join(namespace) + '/' + cpp_class + '/' + cpp_class + '.hpp'
    py_class = cpp_class.title()
    public_methods = getPublicMethods(cpp_class, header_file)
 
    if not os.path.isfile(lib_file) :
        print >> sys.stderr, 'Library ' + lib_name + ' not found'
        return 1

    pyx_file_handle = open(py_class + '.pyx', 'w')
    printImports(pyx_file_handle)
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

if __name__ == "__main__" :
    main()
