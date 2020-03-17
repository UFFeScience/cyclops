//
//  main.cpp
//
//  Generic multi-dimensional Matrix
// 
//  Follows design provided in Bjarne Stroustrup "The C++ Programming Language", Chapter 29
//   
//  Created by Alex Ershov on 12/08/2018.
//
#include <iostream>
#include <type_traits>
#include "Matrix.h"

using namespace std;

int main(int argc, const char * argv[]) {
   
    cout<<"Welcome to the Matrix!\n";
    
    Matrix<double ,0> m0 {1};
    
    return 0;
}
