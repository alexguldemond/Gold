#!/usr/bin/python

import argparse
import CppHeaderParser
import os
import os.path
import sys

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
 
    if not os.path.isfile(lib_file) :
        print >> sys.stderr, 'Library ' + lib_name + ' not found'
        return 1

    try :
        header = CppHeaderParser.CppHeader(header_file)
    except CppHeaderParser.CppParserError as e:
        print >> sys.stderr, e
        return 1
        
    class_tree = header.classes[cpp_class]

    public_methods = []
    for method in class_tree["methods"]["public"]:
        public_methods.append( {
            'name': method["name"], 
            'parameters' : [ t["type"] for t in method["parameters"]], 
            'return_type' : method["rtnType"],
            'constructor' : method["constructor"],
            'destructor' : method["destructor"],
        })

    py_class = cpp_class.title()
    pyx_file_handle = open(py_class + '.pyx', 'w')
    for container in ['string', 'vector', 'map']:
        pyx_file_handle.write("from libcpp.%s import %s\n" % (container, container))

    pyx_file_handle.write('\ncdef extern "%s" namespace "%s" :\n' % ( header_file, '::'.join(namespace) ) )
    pyx_file_handle.write('\tcdef cppclass %s :\n' % (cpp_class) ) 
    
    for method in public_methods:
        exception_clause = ' except +' if method["constructor"] else ''
        if method["constructor"] or method["destructor"] :
            return_type = ''
        else :
            return_type = method['return_type'] + ' '

        pyx_file_handle.write(
            '\t\t%s%s(%s)%s\n' % ( 
                return_type,
                method['name'],
                ", ".join(method['parameters']), 
                exception_clause 
            )
        )

    pyx_file_handle.write('\n')
    
    pyx_file_handle.write('cdef class %s :\n' % (py_class) )
    pyx_file_handle.write('\tcdef %s* ptr\n' % (py_class) )
    
    for method in public_methods:
        if method["constructor"] :
            name = '__cinit__'
        elif method['destructor'] :
            continue
        else:
            name = method['name']
        parameters = 'self'
        if method['parameters'] : parameters += ', ' + ', '.join(method['parameters'])
        pyx_file_handle.write('\t\t def %s(%s):\n' % (
            method['name'],
            parameters
        ))
                        
if __name__ == "__main__" :
    main()
