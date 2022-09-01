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
%template(pair_thing) std::pair<float, Limit>;
%template(idk) std::vector< std::pair< float,Limit >,std::allocator< std::pair< float,Limit > > >;
%include "orderbook.h"
