#! /usr/bin/perl -w

use File::Find;
use File::Basename;
use File::Path;
use File::Spec;

my ( $inputDataDir, $outputDataDir, $outputScriptDir ) = @ARGV;

$inputDataDir = File::Spec->rel2abs( $inputDataDir );
$outputDataDir = File::Spec->rel2abs( $outputDataDir );
$outputScriptDir = File::Spec->rel2abs( $outputScriptDir );
if( ! -e $outputScriptDir )
  {
  mkpath( $outputScriptDir, {verbose => 0, mode => 0755} ) or
    die "Can't create output directory $outputScriptDir\n\t";
  }


my $registrationTemplate = "/home/njt4n/share/Data/Stone/BreacherPhaseI/T_templateTotal.nii.gz";

find( \&wanted, $inputDataDir );

sub wanted
  {
  my ( $filename, $directories, $suffix ) = fileparse( $File::Find::name );

  my @comps = split( '/', $directories );

  if( $filename =~ m/t1\.nii\.gz$/ )
    {

    my $baseScript = "/home/njt4n/Pkg/scripts/pbs_test.sh";
    open( BASEFILE, "<$baseScript" );
    my @baseScriptContents = <BASEFILE>;
    close( BASEFILE );

    my @incomps = split( '/', $inputDataDir );

    my $localOutputDir = '';
    for( $i = 0; $i < @comps-1; $i++ )
      {
      if( $i >= @incomps || $incomps[$i] !~ m/^${comps[$i]}$/ )
        {
        $localOutputDir .= "$comps[$i]/";
        }
      }
    $localOutputDir .= "ABP";
    $localOutputDir = "${outputDataDir}/${localOutputDir}/";
    if( !-e $localOutputDir )
      {
      mkpath( $localOutputDir, {verbose => 0, mode => 0755} ) or
        die "Can't create output directory $localOutputDir\n\t";
      }

    $baseScriptContents[8] = "T1=${directories}/${filename}\n";
    $baseScriptContents[9] = "REGISTRATION_TEMPLATE=${registrationTemplate}\n";
    $baseScriptContents[10] = "OUTPUT_DATA_DIR=${localOutputDir}\n";

    my $pbsFile = "${outputScriptDir}/${filename}";
    $pbsFile =~ s/\.nii\.gz$/_pbs\.sh/;
    open( FILE, ">$pbsFile" ) or die "Couldn't open $pbsFile";
    print FILE @baseScriptContents;
    close( FILE );

    system( "qsub $pbsFile" );

    }
  }
