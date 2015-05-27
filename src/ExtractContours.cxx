#include "itkBinaryContourImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLabelContourImageFilter.h"
#include "itkSliceBySliceImageFilter.h"

template <unsigned int ImageDimension>
int ExtractContours( int argc, char *argv[] )
{
  typedef float PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

//  if( argc < 7 )
//    {
    typedef itk::LabelContourImageFilter<ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( reader->GetOutput() );
    if( argc > 4 )
      {
      filter->SetFullyConnected( static_cast<PixelType>( atof( argv[4] ) ) );
      }

    typedef itk::ImageFileWriter<ImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( argv[3] );
    writer->SetInput( filter->GetOutput() );
    writer->Update();
//    }
//  else
//    {
//    typedef itk::BinaryContourImageFilter<ImageType, ImageType> FilterType;
//    typename FilterType::Pointer filter = FilterType::New();
//    filter->SetInput( reader->GetOutput() );
//    if( argc > 4 )
//      {
//      filter->SetFullyConnected( static_cast<PixelType>( atof( argv[4] ) ) );
//      }
//    if( argc > 5 )
//      {
//      filter->SetBackgroundValue( static_cast<PixelType>( atof( argv[5] ) ) );
//      }
//
//    typedef itk::ImageFileWriter<ImageType> WriterType;
//    typename WriterType::Pointer writer = WriterType::New();
//    writer->SetFileName( argv[3] );
//    writer->SetInput( filter->GetOutput() );
//    writer->Update();
//    }


  return 0;
}

int ExtractContoursSliceBySlice( int argc, char *argv[] )
{
  const unsigned int ImageDimension = 3;
  typedef float PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  typedef itk::SliceBySliceImageFilter<ImageType, ImageType> SliceFilterType;
  SliceFilterType::Pointer sliceFilter = SliceFilterType::New();
  sliceFilter->SetInput( reader->GetOutput() );

  typedef itk::LabelContourImageFilter<SliceFilterType::InternalInputImageType, SliceFilterType::InternalOutputImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  if( argc > 4 )
    {
    filter->SetFullyConnected( static_cast<PixelType>( atof( argv[4] ) ) );
    }

  sliceFilter->SetFilter( filter );

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( sliceFilter->GetOutput() );
  writer->Update();

  return 0;
}

int main( int argc, char *argv[] )
{
  if ( argc < 4 )
    {
    std::cout << argv[0] << " imageDimension inputImage outputImage"
      << " [fullyConnected] " << std::endl;
    exit( 1 );
    }

  if( *argv[1] == 'X' )
    {
    ExtractContoursSliceBySlice( argc, argv );
    }
  else
    {
    switch( atoi( argv[1] ) )
     {
     case 2:
       ExtractContours<2>( argc, argv );
       break;
     case 3:
       ExtractContours<3>( argc, argv );
       break;
     default:
        std::cerr << "Unsupported dimension" << std::endl;
        exit( EXIT_FAILURE );
     }
   }
}
