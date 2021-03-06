#include "itkImageFileReader.h"
#include "itkImageRandomNonRepeatingIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkVectorImageFileReader.h"

#include "global.h"
#include "string.h"
#include <fstream.h>

#define isnan(x) ((x) != (x))

int main( int argc, char *argv[] )
{
  if ( argc < 3 )
    {
    std::cout << "Usage: " << argv[0] << " deformationField outputPrefix [type: pull=0, push=1] [maskImage]" << std::endl;
    exit( 1 );
    }
  
  typedef itk::Image<PixelType, ImageDimension> ImageType;
  typedef float RealType;
  typedef itk::Image<RealType, ImageDimension> RealImageType;
  typedef itk::Vector<RealType, ImageDimension> VectorType;
  typedef itk::Image<VectorType, ImageDimension> VectorFieldType;

  typedef itk::VectorImageFileReader<RealImageType, VectorFieldType> VectorFieldReaderType;
  VectorFieldReaderType::Pointer reader = VectorFieldReaderType::New();
  reader->SetFileName( argv[1] );
  reader->SetUseAvantsNamingConvention( true );
  reader->Update();

  typedef itk::Image<unsigned short, ImageDimension> MaskImageType;
  MaskImageType::Pointer mask = MaskImageType::New();

  unsigned int type = 0;
  if ( argc >= 4 )
    {
    type = atoi( argv[3] );
    }

  if ( argc >= 5 )
    {
    typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
    MaskReaderType::Pointer maskreader = MaskReaderType::New();
    maskreader->SetFileName( argv[4] );
    maskreader->Update();
    mask = maskreader->GetOutput();
    }     
  else
    {
    mask->SetRegions( reader->GetOutput()->GetLargestPossibleRegion() );
    mask->SetOrigin( reader->GetOutput()->GetOrigin() );
    mask->SetSpacing( reader->GetOutput()->GetSpacing() );
    mask->Allocate();
    mask->FillBuffer( 1 );
    } 

  typedef itk::NearestNeighborInterpolateImageFunction<MaskImageType> InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetInputImage( mask );

  std::string filenameM = std::string( argv[2] ) + std::string( "Moving.txt" );
  ofstream strM( filenameM.c_str() );
  std::string filenameF = std::string( argv[2] ) + std::string( "Fixed.txt" );
  ofstream strF( filenameF.c_str() );

  strM << "0 0 0 0" << std::endl;
  strF << "0 0 0 0" << std::endl;

/*
  itk::ImageRandomNonRepeatingIteratorWithIndex<MaskImageType> It
    ( mask, mask->GetLargestPossibleRegion() );
  unsigned long numberOfSamples = 1;
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    numberOfSamples *= mask->GetLargestPossibleRegion().GetSize()[i];
    } 
  It.SetNumberOfSamples( static_cast<unsigned long>( 0.25*numberOfSamples ) ); 
*/

  itk::ImageRegionIteratorWithIndex<MaskImageType> It
    ( mask, mask->GetLargestPossibleRegion() );
  
  unsigned long index = 1;
  for ( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
    VectorType disp = reader->GetOutput()->GetPixel( It.GetIndex() );
    if ( disp.GetSquaredNorm() == 0 )
      {
      continue;
      } 

    VectorFieldType::PointType pt;
    reader->GetOutput()->TransformIndexToPhysicalPoint( It.GetIndex(), pt );

    InterpolatorType::PointType tmp_pt;
    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      if ( type == 0 )
        {
        tmp_pt[i] = pt[i] + disp[i];
        }
      else
        {
        tmp_pt[i] = pt[i];
        }    
      } 
    if ( interpolator->IsInsideBuffer( tmp_pt ) && interpolator->Evaluate( tmp_pt ) > 0 )
      {
      strM << pt[0] << " " << pt[1] << " ";  
      strF << pt[0]+disp[0] << " " << pt[1]+disp[1] << " ";
      if ( ImageDimension == 3 )
        {
        strM << pt[2] << " " << index << std::endl;  
        strF << pt[2]+disp[2] << " " << index << std::endl;
        }    
      else if ( ImageDimension == 2 )
        {
        strM << "0 " << index << std::endl;  
        strF << "0 " << index << std::endl;  
        }    
      index++;
      }
    }      
    
  strM << "0 0 0 0" << std::endl;
  strF << "0 0 0 0" << std::endl;
  
  return 0;
}
