/* File : example.i */
%module orderbook

%include <std_vector.i>
%include <std_string.i>
%include <std_map.i>

%{
#include "orderbook.h"
%}


/* Let's just grab the original header file here */
%template(float_vector) std::vector<float>;
%template(limit_map) std::map<float, Limit>;
%template(idk) std::map<float, Limit, greater<float>, std::allocator<std::pair<float const, Limit>>>;
%include "orderbook.h"
