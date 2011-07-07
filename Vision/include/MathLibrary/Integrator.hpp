//======================================================================== 
// Package		: The Math Library - Ex
// Authors		: Vilas Kumar Chitrakaran
// Start Date		: Wed Dec 20 11:08:28 GMT 2000
// Compiler		: GNU C++ 2.95.3 and above
// ----------------------------------------------------------------------  
// File: Integrator.hpp
// Interface of the class Integrator.
//========================================================================  
 

#ifndef INCLUDED_Integrator_hpp
#define INCLUDED_Integrator_hpp
 
 
//========================================================================
// class Integrator
// -----------------------------------------------------------------------
// \brief
// The class \c Integrator uses the trapeziodal rule as the default 
// numerical method for integration. 
//
// The user can reimplement a derived class with any other method of 
// integration if desired. The class \c Integrator can be used with any data 
// type (double, int, RowVector, ColumnVector, Matrix, etc).
//
// <b>Example Program:</b>
// \include Integrator.t.cpp
//========================================================================  
 
template<class T = double>
class Integrator
{
 public:
  inline Integrator();
   // The default constructor.
   // The sampling period is set to 
   // default of 0.001 seconds.
   // The initial value is set to 0.
	
  inline Integrator(double samplingPeriod, const T &initialValue);
   // The constructor to initialize the sampling period 
   // and initial Value.
   //  samplingPeriod 	The sampling period in seconds.
   //  initialValue	The initial value at the start of integration.

  virtual ~Integrator(){}
   // The default destructor.

  inline void setSamplingPeriod(double p) { d_samplingPeriod = p; }
   // Sets the sampling period of the integrator 
   //  p 	The sampling period in seconds.

  virtual void reset(const T &value);
   // Reset the output of the Integrator.
   //  value  The value to which the integrator output is reset to.
		
  inline virtual T integrate(T &input);
   // This function provides the numerical method for integration. The 
   // default is trapezoidal rule of integration. Override this method in 
   // a derived class to use another algorithm.
   //  input  The time-varying signal to be integrated.

  // ========== END OF INTERFACE ==========
 
 protected:
  double d_samplingPeriod;
 
 private:
  T d_previousInput;
  T d_currentOutput;
  T d_previousOutput;
};

//======================================================
// End of declarations.
//======================================================


//======================================================================== 
// Integrator::Integrator: Constructor of the Integrator class
//========================================================================  
template<class T>
Integrator<T>::Integrator()
{
 T resetToValue; 
 resetToValue = 0.0;
 d_samplingPeriod = 0.001;
 reset(resetToValue);
}

template<class T>
Integrator<T>::Integrator(double samplingPeriod, const T &initialValue)
{
 d_samplingPeriod = samplingPeriod;
 reset(initialValue);
}


//======================================================================== 
// Integrator::reset
//========================================================================  
template<class T>
void Integrator<T>::reset(const T &resetToValue)
{
 d_previousOutput = resetToValue;
 d_previousInput = 0;
}


//======================================================================== 
// Integrator::Integrate
//========================================================================  
template<class T>
T Integrator<T>::integrate(T &currentInput)
{
 d_currentOutput = (1/2.0) * (d_previousInput + currentInput) 
                   * d_samplingPeriod + d_previousOutput;
 d_previousInput = currentInput;
 d_previousOutput = d_currentOutput;
 return d_currentOutput;		
}


#endif

