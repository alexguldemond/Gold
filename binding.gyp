{
    'targets': [
	{
	    'target_name': 'gtest',
	    'type': 'static_library',
	    'include_dirs': [ 
		'src/cplusplus/vendor/googletest/googletest/include',
		'src/cplusplus/vendor/googletest/googletest',
	    ],
	    'sources': [ 'src/cplusplus/vendor/googletest/googletest/src/gtest-all.cc' ],
	    'ldflags': [
		'-pthread',
	    ],
	},
	{
	    'target_name': 'math_test',
	    'type': 'executable',
	    'include_dirs': [
		'src/cplusplus/vendor/googletest/googletest/include',
		'include',
	    ],
	    'sources': [ 
		'src/cplusplus/math/node/node.cpp',
		'src/cplusplus/math/expression/expression.cpp',
		'src/cplusplus/math/expression/variable.cpp',
		'src/cplusplus/math/function/function.cpp',
		'src/cplusplus/math/utils/utils.cpp',
		'src/cplusplus/math/utils/utils_test.cpp',
		'src/cplusplus/math/node/node_test.cpp',
		'src/cplusplus/math/expression/expression_test.cpp',
		'src/cplusplus/math/function/function_test.cpp',
		'src/cplusplus/math/test/all_test.cxx',
	    ],
	    'dependencies': [ 'gtest' ],
	    'cflags_cc': [
		'-Wall', 
		'-Werror',
		'-g',
		'-O0',
		'--coverage',
		'-fprofile-arcs',
		'-ftest-coverage',
		'--std=c++1y',
	    ],
	    'cflags!': [ '-fno-exceptions' ],
	    'cflags_cc!': [ '-fno-exceptions' ],
	    'ldflags': [
		'-pthread',
		'-fprofile-arcs',
	    ],
	},
    ]
}
	    
    
