//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: HighpassFilter.hpp
// Interface of the class HighpassFilter.
//========================================================================  
 

#ifndef INCLUDED_HighpassFilter_hpp
#define INCLUDED_HighpassFilter_hpp

// ----- Standard Libaries -----
#include <iostream>
#include <math.h>

using namespace std;

//======================================================================== 
// class HighpassFilter
// ----------------------------------------------------------------------
// \brief
// A high-pass second order butterworth filter.
// 
// <b>Example Program:</b>
//
// \include HighpassFilter.t.cpp
//========================================================================  
 
template<class T = double>
class HighpassFilter
{
 public:
  inline HighpassFilter();
   // The default Constructor. Sets
   // cut-off frequency to 1 Hertz
   // and sampling period to 0.001 sec.

  inline HighpassFilter(double cutOffFrequencyHz, double samplingPeriod );
   // This constructor for the filter initializes the parameters
   // of the filter.
   //  cutOffFrequencyHz  The cut-off frequency of the 
   //			  high pass butterworth filter in \a hertz.
   //  samplingPeriod 	  The sampling period of the 
   //			  filter in seconds.
  
  ~HighpassFilter(){}
   // The default destructor.
	
  inline void setCutOffFrequencyHz(double cutOffFrequencyHz);
   // This function sets the cut-off frequency of the filter. 
   //  cutOffFrequencyHz  The cut-off frequency of the 
   //                     high pass butterworth filter in \a hertz.

  inline void setCutOffFrequencyRad(double cutOffFrequencyRad);
   // This function sets the cut-off frequency of the filter. 
   //  cutOffFrequencyRad  The cut-off frequency of the 
   //                      high pass butterworth filter in \a rad/sec.

  inline double getCutOffFrequencyHz();
   //  return  The cut-off frequency of the filter in \a hertz.

  inline double getCutOffFrequencyRad();
   //  return  The cut-off frequency of the filter in rad/sec.

  inline void setSamplingPeriod(double samplingPeriod);
   // Sets the sampling period of the filter. 
   //  samplingPeriod  The sampling period of the in seconds.

  inline double getSamplingPeriod();
   //  return  The sampling period in seconds.

  inline void setAutoInit();
   // Automatic initialization of the filter.

  inline void initializeFilter (T initInput, T initOutput);
   // Initializes the initial value of input and output.
   //  initInput   Initial value of the input to the filter.
   //  initOutput  Initial output of the filter.

  inline T filter(T input);
   // The filter.
	
 protected:
  double d_samplingPeriod;
  double d_cutOffFrequencyHz;
  double d_cutOffFrequencyRad;
  double d_numeratorParameter[3]; 
  double d_denumParameter[3];
  int d_initFlag ;
  T d_previousInputX[3];  
  T d_previousOutputY[3];
  T d_filteredOut;
  
  inline void calculateInternalParameters();
   // Calculates the internel parameters
   // based on cut-off frequency and 
   // sampling period.
};

// ========== END OF INTERFACE ==========

//==============================================
// The implementation of the filter begins here.
//==============================================


//======================================================================== 
// HighpassFilter::HighpassFilter: Constructor of the HighpassFilter class
//========================================================================  
template<class T>
HighpassFilter<T>::HighpassFilter ()
{
 // set class parameters
 d_cutOffFrequencyHz = 1;
 d_samplingPeriod = 0.001;
 d_cutOffFrequencyRad = 2 * 3.14159265358979 * d_cutOffFrequencyHz;

 //calculate the filter parameters
 calculateInternalParameters();
 d_initFlag = 1;
}

template<class T>
HighpassFilter<T>::HighpassFilter (double cutOffFrequencyHz, double samplingPeriod)     
{
 // set class parameters
 d_cutOffFrequencyHz = cutOffFrequencyHz;
 d_samplingPeriod = samplingPeriod;
 d_cutOffFrequencyRad = 2 * 3.14159265358979 * d_cutOffFrequencyHz;

 //calculate the filter parameters
 calculateInternalParameters();
 d_initFlag = 1;
}


//======================================================================== 
// HighpassFilter::setCutOffFrequencyHz
//========================================================================  
template<class T>
void HighpassFilter<T>::setCutOffFrequencyHz(double cutOffFrequencyHz)
{
 d_cutOffFrequencyHz = cutOffFrequencyHz;
 d_cutOffFrequencyRad = 2 * 3.14159265358979 * d_cutOffFrequencyHz;
 calculateInternalParameters();
}


//======================================================================== 
// HighpassFilter::setCutOffFrequencyRad
//========================================================================  
template<class T>
void HighpassFilter<T>::setCutOffFrequencyRad(double cutOffFrequencyRad)
{
 d_cutOffFrequencyRad = cutOffFrequencyRad;
 d_cutOffFrequencyHz = d_cutOffFrequencyRad / (2 * 3.14159265358979);
 calculateInternalParameters();
}


//======================================================================== 
// HighpassFilter::getCutOffFrequencyHz
//========================================================================  
template<class T>
double HighpassFilter<T>::getCutOffFrequencyHz()
{
 return(d_cutOffFrequencyHz);
}


//======================================================================== 
// HighpassFilter::getCutOffFrequencyRad
//========================================================================  
template<class T>
double HighpassFilter<T>::getCutOffFrequencyRad()
{
 return(d_cutOffFrequencyRad);
}


//======================================================================== 
// HighpassFilter::setSamplingPeriod
//========================================================================  
template<class T>
void HighpassFilter<T>::setSamplingPeriod(double samplingPeriod)
{
 d_samplingPeriod = samplingPeriod;
 calculateInternalParameters();
}


//======================================================================== 
// HighpassFilter::getSamplingPeriod
//========================================================================  
template<class T>
double HighpassFilter<T>::getSamplingPeriod()
{
 return(d_samplingPeriod);
}


//======================================================================== 
// HighpassFilter::calculateInternalParameters
//========================================================================  
template<class T>
void HighpassFilter<T>::calculateInternalParameters()
{
 double pOmega =  tan(3.14159265358979 * d_cutOffFrequencyHz * d_samplingPeriod); 
 // prewarp 
 
 double pOmegaSq = pOmega * pOmega;
 d_denumParameter[0] = pOmegaSq + 1.414213562 * pOmega + 1;
 d_denumParameter[1] = 2 * pOmegaSq - 2;
 d_denumParameter[2] = pOmegaSq - 1.414213562 * pOmega + 1;
 d_numeratorParameter[0] = 1;
 d_numeratorParameter[1] = -2;
 d_numeratorParameter[2] = 1;
}


//======================================================================== 
// HighpassFilter::setAutoInit
//========================================================================  
template<class T>
void HighpassFilter<T>::setAutoInit()
{
 d_initFlag = 1;
}


//======================================================================== 
// HighpassFilter::initializeFilter
//========================================================================  
template<class T>
void HighpassFilter<T>::initializeFilter (T initInput, T initOutput)
{
 for (int i=0; i<3; i++)
 {
  d_previousInputX[i] = initInput;
  d_previousOutputY[i] = initOutput;
 }
 d_initFlag = 0; 
}


//=============================================================================
// HighpassFilter::filter
//============================================================================
template<class T>
T HighpassFilter<T>::filter (T input)
{
 // automatic initialization 	
 if (d_initFlag ==1 )
 {
  for (int i = 0; i < 3; i++)
  {
   d_previousInputX[i] = input;
   d_previousOutputY[i] = input;
  }
  d_initFlag = 0;	
 }
	
 // the filtering process is done by the following equation.
 d_previousInputX[2] = input;
 d_previousOutputY[2] = (d_numeratorParameter[0] * d_previousInputX[2] 
                        + d_numeratorParameter[1] * d_previousInputX[1] 
                        + d_numeratorParameter[2] * d_previousInputX[0] 
                        - d_denumParameter[1] * d_previousOutputY[1] 
                        - d_denumParameter[2] * d_previousOutputY[0])
                        /d_denumParameter[0];         
			
 d_previousOutputY[0] = d_previousOutputY[1];
 d_previousOutputY[1] = d_previousOutputY[2];

 d_previousInputX[0] = d_previousInputX[1];
 d_previousInputX[1] = d_previousInputX[2];

 d_filteredOut = d_previousOutputY[2];
 return d_filteredOut;
}


#endif


