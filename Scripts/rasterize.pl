#!/usr/bin/perl
# use module
use XML::Simple;
use Data::Dumper;
print "===================================================\n";
print "GEOTIFF rasterizer for use with osgEarth.\n";
print "author: Nick Schultz <nick.schultz\@live.com>\n";
print "===================================================\n\n";
if ($#ARGV != 6 ) {
	print "ERROR, not enough arguments\n\n";
	print "usage: rasterize.pl [shapefile] [discript.xml] [layer] [attribute] [pixelsize] [output.tif] \n";
	print "ex: rasterize.pl Year_50.shp idu.xml Year_50 PopDens 30 pt50_PopDens.tif \n";
	exit;
}

$shapefile = $ARGV[0];
$discript = $ARGV[1];
$layer = $ARGV[2];
$attribute = $ARGV[3];
$pixelsize = $ARGV[4];
$output = $ARGV[5];

$TEMPFILE = "temp.tif";
#test file existance
sub fileExists
{
	die " File '$_[0]' doesn't exist.\n"
		if !(-e $_[0]);
}
fileExists($shapefile);
fileExists($descript);

# create object
$xml = new XML::Simple(KeyAttr=>[]);

# read XML file
$data = $xml->XMLin($descript);


# dereference hash ref
$field = $data->{field}[0];
	
@args = ("L:\\gdal_bin\\gdal_rasterize.exe",
			"-init", "0",
			"-init", "0",
			"-init", "0",
			"-init", "0",
			"-tr", $pixelsize, $pixelsize,
			"-ot", "Byte", 
			"-burn", "0",
			"-l", $layer,
			"-where", "\"".$attribute."=0\"", $shapefile,  $TEMPFILE);	

system(@args);
foreach $field ( @{$data->{field}})	
{
	if( $field->{col} ne $attribute){
		next;
	}
		
	foreach $attrib ( @{ $field->{attributes}->{attr}})
	{
		if( $attrib->{color} =~ /\((\d+),(\d+),(\d+)\)/ )
		{
			if((exists $attrib->{minVal}) && (exists $attrib->{maxVal}))
			{
				@args = ("L:\\gdal_bin\\gdal_rasterize.exe",
					"-b", "1",
					"-b", "2",
					"-b", "3",
					"-b", "4",
					"-burn", $1,
					"-burn", $2,
					"-burn", $3,
					"-burn", "255",	
					"-l", $layer,
					"-where", "\"".$attribute. ">=" . $attrib->{minVal} . " AND " . $attribute . "<" . $attrib->{maxVal} .  "\"", 
					$shapefile,  $TEMPFILE);
			}
			elsif(exists $attrib->{value})
			{
				@args = ("L:\\gdal_bin\\gdal_rasterize.exe",
					"-b", "1",
					"-b", "2",
					"-b", "3",
					"-b", "4",
					"-burn", $1,
					"-burn", $2,
					"-burn", $3,
					"-burn", "255",	
					"-l", $layer,
					"-where", "\"".$attribute. ">=" . $attrib->{value} ."\"", 
					$shapefile,  $TEMPFILE);
			}
							
			print @args , "\n";
			system(@args);
		}				
	}	
}
#warp the raster to osgEarth coordinate system
@args = ("L:\\gdal_bin\\gdalwarp",
			"-multi",
			"-t_srs", "epsg:4326",
			"-r", "cubic",
			$TEMPFILE, "reprojected.tif");
system(@args);

#tile the raster
@args = ("L:\\gdal_bin\\gdal_translate",
			"-of", "GTiff",
			"-co", "\"TILED=YES\"",
			".\\reprojected.tif",
			$filename);
system(@args);

#mipmap the raster
@args = ("L:\\gdal_bin\\gdaladdo",
			"-r", "gauss",
			$filename,
			"2", "4", "8", "16");
system(@args);	

#delete temp files
@args = ("del", ".\\reprojected.tif");
system(@args);
@args = ("del", $TEMPFILE);
system(@args);

print "done!\n";