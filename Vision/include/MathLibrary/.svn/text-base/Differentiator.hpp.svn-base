//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Differentiator.hpp
// Interface of the class Differentiator.
//========================================================================  

#ifndef INCLUDED_Differentiator_hpp
#define INCLUDED_Differentiator_hpp

#include "LowpassFilter.hpp"

//====================================================================
// class Differentiator
// -------------------------------------------------------------------
// \brief
// Numerical differentation using backward difference followed by 
// low-pass filtering. 
//
// The Differentiator computes differentiation of the signal using
// backward difference followed by smoothening of the differentiated
// signal by a low pass filter. A derived class from Differentiator can
// override the differenting algorithm provided.
//
// The class \c Differentiator can be used with any data type
// (double, int, RowVector, ColumnVector, Matrix, etc).
// 
// <b>Example Program:</b>
// \include Differentiator.t.cpp
//========================================================================  
 
template<class T = double>
class Differentiator
{
 public:
  inline Differentiator();
   // Default constructor for differentiator.
   // Sampling period set to default of 0.001 seconds.
   // The cut-off frequency is set to half the
   // sampling frequency and damping ratio of the
   // low-pass filter in the differentiator is set to 1. 
 
  inline Differentiator(double samplingPeriod);
   // The Constructor that initializes the sampling period.
   //  samplingPeriod  The sampling period of the 
   //                  differentiator in seconds.
	
  virtual ~Differentiator (){}
   // The default destructor.

  void setSamplingPeriod(double p){ d_samplingPeriod = p; }
   // Sets the sampling period of the differentiator 
   //  p  The sampling period of the differentiator in seconds.
   //     By default the sampling period is set to 0.001 seconds.

  inline void setCutOffFrequencyHz(double cutOffFrequencyHz);
   // Differentiation is followed by a low pass filtering process.
   // This function sets the cut-off frequency of the filter in hertz. 
   //  cutOffFrequencyHz  The cut-off frequency of the
   //                     low pass butterworth filter in hertz.
   //                     By default the cut-off frequency 
   //                     is set to half the sampling frequency
   //                     of the differentiator.

  inline void setCutOffFrequencyRad(double cutOffFrequencyRad);
   // Differentiation is followed by a low pass filtering process.
   // This function sets the cut-off frequency of the filter in Rad/sec. 
   //  cutOffFrequencyRad  The cut-off frequency of the 
   //                      low pass butterworth filter in Rad/sec.
   //                      By default the cut-off frequency 
   //                      is set to half the sampling frequency
   //                      of the differentiator.

  void setDampingRatio(double d) { d_dampingRatio = d; }
   // Sets the damping factor of the butterworth filter.
   //  d  Desired damping ratio. The default value is 1.

  void reset();
   // Resets the differentiator output to zero.

  inline virtual T differentiate(T &currentInput);
   // This function implements the numerical 
   // method for differentiation.
   // Being a virtual method, the user can derive
   // a method of differentiation that suits the desired needs.
   // i.e..override Differentiator<-My Differentiator ( with
   // a different \a Differentiate() method). 
   // The default method also calls a low-pass
   // Butterworth filter to smoothen
   // out the differentiator output. The differentiator
   // output in the first cycle is smoothened to zero.
   // The cut-off frequency of the filter
   // is set by the method \c setCutOffFrequency.
   // By default the cut-off frequency is half
   // the sampling frequency of the Differentiator.
   //  currentInput  The present value of the 
   //                variable being differentiated.
		
 // ========== END OF INTERFACE ==========
 
 private:
  double d_samplingPeriod;
  double d_cutOffFrequencyHz;
  double d_cutOffFrequencyRad;
  double d_dampingRatio;
  T d_previousInput;
  T d_currentDifferential;
  LowpassFilter<T> lowPassFilter;
  int d_initializationFlag;
};



//==============================================================
// End of declarations.
//==============================================================


//======================================================================== 
// Differentiator::Differentiator: Constructor of the Differentiator class
//========================================================================  
template<class T>
Differentiator<T>::Differentiator()
{
 d_samplingPeriod = 0.001;
 d_dampingRatio = 1;
 d_previousInput = 0;
 d_cutOffFrequencyHz = 1/(2*d_samplingPeriod); 
 d_cutOffFrequencyRad = 2 * 3.14159265358979 * d_cutOffFrequencyHz;
 d_initializationFlag = 0;
}


template<class T>
Differentiator<T>::Differentiator(double samplingPeriod)
{
 d_samplingPeriod = samplingPeriod;
 d_dampingRatio = 1;
 d_previousInput = 0;
 d_cutOffFrequencyHz = 1/(2*d_samplingPeriod);
 d_cutOffFrequencyRad = 2 * 3.14159265358979 * d_cutOffFrequencyHz;
 d_initializationFlag = 0;
}


//======================================================================== 
// Differentiator::setCutOffFrequencyHz
//========================================================================  
template<class T>
void Differentiator<T>::setCutOffFrequencyHz(double cutOffFrequencyHz)
{
 d_cutOffFrequencyHz = cutOffFrequencyHz;
 d_cutOffFrequencyRad = 2 * 3.14159265358979 * d_cutOffFrequencyHz;
}


//======================================================================== 
// Differentiator::setCutOffFrequencyRad
//========================================================================  
template<class T>
void Differentiator<T>::setCutOffFrequencyRad(double cutOffFrequencyRad)
{
 d_cutOffFrequencyRad = cutOffFrequencyRad;
 d_cutOffFrequencyHz = 	d_cutOffFrequencyRad/(2 * 3.14159265358979);
}


//======================================================================== 
// Differentiator::reset
//========================================================================  
template<class T>
void Differentiator<T>::reset()
{
 d_initializationFlag = 0;

  // Settings for the low-pass filter.
 lowPassFilter.setCutOffFrequencyHz(d_cutOffFrequencyHz);
 lowPassFilter.setDampingRatio(d_dampingRatio);
 lowPassFilter.setSamplingPeriod(d_samplingPeriod);
}


//======================================================================== 
// Differentiator::differentiate
//========================================================================  
template<class T>
T Differentiator<T>::differentiate(T &currentInput)
{
 // automatic initialization of the first differentiator
 // output to zero.
 if (d_initializationFlag == 0)
 {
  d_previousInput = currentInput;
  d_initializationFlag = 1;
 }
	
 T filteredOutput;

 d_currentDifferential = (currentInput - d_previousInput) * (1/d_samplingPeriod) ;
 d_previousInput = currentInput;

 filteredOutput = lowPassFilter.filter(d_currentDifferential);
 return filteredOutput;
}


#endif
