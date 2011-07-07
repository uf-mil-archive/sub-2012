//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed May 5 11:08:28 GMT 2004
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Adams3Integrator.hpp
// Interface of the class Adams3Integrator.
//========================================================================  
 

#ifndef INCLUDED_Adams3Integrator_hpp
#define INCLUDED_Adams3Integrator_hpp

#include "Integrator.hpp" 
 
//======================================================================== 
// class Adams3Integrator
// ----------------------------------------------------------------------
// \brief
// Numerical integration using Adam's 3'rd order method.
//
// This is an implementation from Chapter 12 of the book "Math Toolkit for
// Real-time Programmers", by Jack W. Crenshaw. This implementation is 3rd 
// order.
//
// <b>Example Program:</b>
//
// \include Adams3Integrator.t.cpp
//========================================================================  

template<class T = double>
class Adams3Integrator : public Integrator<T>
{
 public:
  inline Adams3Integrator ();
   // The default constructor.
   // The sampling period is set to 
   // default of 0.001 seconds (set by parent).
   // The initial value is set to 0.
	
  inline Adams3Integrator(double samplingPeriod, const T &initialValue);
   // The constructor to initialize the sampling period 
   // and initial Value.
   //  samplingPeriod 	The sampling period in seconds.
   //  initialValue	The initial value at the start
   //		        of integration.

  virtual ~Adams3Integrator() {};
   // The default destructor

  virtual void reset(const T &resetToValue);
   // This function resets the output of the Integrator
   // to the value \a resetToValue and further integration
   // restarts from this initial value.
   //  resetToValue  The value to which the integrator
   //		     output is reset to.

  inline virtual T integrate(T &currentInput);
   // This function provides the numerical 
   // method for integration.
   //  currentInput  The current value of the time-varying signal 
   //                to be integrated.
			
 // ========== END OF INTERFACE ==========
 private:
  T d_previousInput;
  T d_currentOutput;
  T d_d1;
  T d_d2;
  T d_d3;	 
};

//======================================================
// End of declarations.
//======================================================



//======================================================================== 
// Adams3Integrator::Adams3Integrator
//========================================================================  
template<class T>
Adams3Integrator<T>::Adams3Integrator()
{
 T resetToValue;
 resetToValue = 0.0;
 this->d_samplingPeriod = 0.001;
 reset(resetToValue);
}


template<class T>
Adams3Integrator<T>::Adams3Integrator(double samplingPeriod, 
                                      const T &initialValue)
{
 this->d_samplingPeriod = samplingPeriod;
 reset(initialValue);
}


//======================================================================== 
// Adams3Integrator::reset
//========================================================================  
template<class T>
void Adams3Integrator<T>::reset(const T &resetToValue)
{
 d_currentOutput = resetToValue;
 d_previousInput = 0.0;
 d_d1 = 0.0;
 d_d2 = 0.0;
 d_d3 = 0.0;	 
}


//======================================================================== 
// Adams3Integrator::integrate
//========================================================================  
template<class T>
T Adams3Integrator<T>::integrate(T &currentInput)
{
 T tmp1, tmp2;
 d_currentOutput = d_currentOutput + this->d_samplingPeriod * 
                   (d_previousInput + (0.5 * d_d1) 
                   + ((5.0/12.0) * d_d2) + (0.375 * d_d3));
 
 tmp1 = d_previousInput;
 d_previousInput = currentInput;
 tmp2 = d_d1;
 d_d1 = d_previousInput - tmp1;
 tmp1 = d_d2;
 d_d2 = d_d1 - tmp2;
 d_d3 = d_d2 - tmp1;
 
 return d_currentOutput;
}

#endif

