//======================================================================== 
// Package			: The Math Library - Ex
// Authors			: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler			: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Math-Utility.cpp
// Implementation of the class ColumnVector.
// See ColumnVector.hpp for more details.
//========================================================================  
 
// ----- Project Includes -----

#include "Math-Utility.hpp"

int eeps(int i, int j, int k)
{
  if((i==0 && j==1 && k==2) || (i==1 && j==2 && k==0) ||
     (i==2 && j==0 && k==1))
    return 1;
  else if((i==0 && j==2 && k==1) || (i==1 && j==0 && k==2) ||
	  (i==2 && j==1 && k==0))
    return -1;
  else
    return 0;
}

 
 
