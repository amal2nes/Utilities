/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGaborKernelFunction.h,v $
  Language:  C++
  Date:      $Date: 2008/10/18 00:20:03 $
  Version:   $Revision: 1.1.1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGaborKernelFunction_h
#define __itkGaborKernelFunction_h

#include "itkKernelFunction.h"
#include "vnl/vnl_math.h"
#include <math.h>
#include <complex>

namespace itk
{

/** \class GaborKernelFunction
 * \brief Gabor kernel used for various computer vision tasks.
 *
 * This class encapsulates a complex Gabor kernel used for 
 * various computer vision tasks such as texture segmentation, 
 * motion analysis, and object recognition.  It is essentially
 * a complex sinusoid enveloped within a gaussian. 
 * See the discussion in 
 *
 *   Andreas Klein, Forester Lee, and Amir A. Amini, "Quantitative
 *   Coronary Angiography with Deformable Spline Models", IEEE-TMI
 *   16(5):468-482, October 1997.
 *
 * for a basic discussion including additional references.   
 *
 * \sa KernelFunction
 *
 * \ingroup Functions
 */
class ITKCommon_EXPORT GaborKernelFunction : public KernelFunction
{
public:
  /** Standard class typedefs. */
  typedef GaborKernelFunction Self;
  typedef KernelFunction      Superclass;
  typedef SmartPointer<Self>  Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self ); 

  /** Run-time type information (and related methods). */
  itkTypeMacro( GaborKernelFunction, KernelFunction ); 

  /** Evaluate the real/imaginary portion of the function. */
  inline double Evaluate ( const double &u ) const
    { 
    double parameter = vnl_math_sqr( u / this->m_Sigma );
    double envelope = vcl_exp( -0.5 * parameter );
    double phase = 2.0 * vnl_math::pi * this->m_Frequency * u 
      + this->m_PhaseOffset;
    if ( this->m_CalculateImaginaryPart )
      {
      return envelope * sin( phase );
      }
    else
      {
      return envelope * cos( phase );
      }     
    }

  /** Evaluate the complex function. */
  inline std::complex<double> EvaluateComplex ( const double &u ) const
    { 
    double parameter = vnl_math_sqr( u / this->m_Sigma );
    double envelope = vcl_exp( -0.5 * parameter );
    double phase = 2.0 * vnl_math::pi * this->m_Frequency * u 
      + this->m_PhaseOffset;
    return std::complex<double>( 
      envelope * cos( phase ),  
      envelope * sin( phase ) );
    }

  itkSetMacro( Sigma, double );
  itkGetConstMacro( Sigma, double );
  
  itkSetMacro( Frequency, double );
  itkGetConstMacro( Frequency, double );
  
  itkSetMacro( PhaseOffset, double );
  itkGetConstMacro( PhaseOffset, double );
  
  itkSetMacro( CalculateImaginaryPart, bool );
  itkGetConstMacro( CalculateImaginaryPart, bool );
  itkBooleanMacro( CalculateImaginaryPart );

protected:
  GaborKernelFunction(); 
  ~GaborKernelFunction();
  void PrintSelf( std::ostream& os, Indent indent ) const
    { Superclass::PrintSelf( os, indent ); }  

private:
  GaborKernelFunction( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  /** Standard deviation of the Gaussian envelope */
  double m_Sigma;
 
  /** Modulation frequency of the sine or cosine component */
  double m_Frequency;

  /** Phase offset of the sine or cosine component */
  double m_PhaseOffset;

  /** Evaluate using the complex part */
  bool m_CalculateImaginaryPart;
};

} // end namespace itk

#endif
