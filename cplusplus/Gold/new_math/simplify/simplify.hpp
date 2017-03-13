#ifndef GOLD_MATH_SIMPLIFY
#define GOLD_MATH_SIMPLIFY

#include "Gold/new_math/utils/utils.hpp"

namespace Gold {
    namespace math {
	namespace simplify {

	    /*****************************************************************************************//**
	    * Simplifies a fraction. Throws an exception if it attempts to divide by zero.
	    *                                                                                             
	    * root   => The root of the tree.
	    * 
	    * Return value => Nothing, transformations are done in place. 
	    *********************************************************************************************/
	    void simplify_rational_number(utils::node_ptr root);
	    
	    /*****************************************************************************************//**
	    * Simplifies the tree as much as possible, assuming the tree doesn't contain any functions
	    * or variables. Example: 1+3 becomes 4. Throws an exception if it tries to perform an undefined
	    * operation or if while simplifying it finds a variable. Assumes it was build with load_tree and
	    * change_inverse_method.
	    *                                                                                             
	    * root   => The root of the tree.
	    * 
	    * Return value => The simplified.
	    *********************************************************************************************/
	    void simplify_rational_tree(utils::node_ptr root);

	    void _simplify_add(utils::node_ptr root);

	    void _simplify_subtract(utils::node_ptr root);

	    void _simplify_multiply(utils::node_ptr root);

	    void _simplify_divide(utils::node_ptr root);

	    void _simplify_power(utils::node_ptr root);
	}
    }
}

#endif
