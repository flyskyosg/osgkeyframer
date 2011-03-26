#!/usr/bin/perl

# use module
use XML::Simple;
use Data::Dumper;

# create object
$xml = new XML::Simple (KeyAttr=>[]);

# read XML file
#$data = $xml->XMLin("watershed_hydromodel.xml");
$data = $xml->XMLin("idu.xml");
#print Dumper($data);
$raster_attrib = "PopDens";
$filename = "pt50_PopDens.tif";
$shapefile = "Year_50.shp";
$layer = "Year_50";
# dereference hash ref

$field = $data->{field}[0];
	
@args = ("L:\\gdal_bin\\gdal_rasterize.exe",
			"-init", "0",
			"-init", "0",
			"-init", "0",
			"-init", "0",
			"-tr", "30", "30",
			"-ot", "Byte", 
			"-burn", "0",
			"-l", $layer,
			"-where", "\"".$raster_attrib."=0\"", $shapefile,  $filename);	

system(@args);
foreach $field ( @{$data->{field}})	
{
	if( $field->{col} ne $raster_attrib){
		next;
	}
		
	foreach $attrib ( @{ $field->{attributes}->{attr}})
	{
		if( $attrib->{color} =~ /\((\d+),(\d+),(\d+)\)/ )
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
					"-where", "\"".$raster_attrib. ">=" . $attrib->{minVal} . " AND " . $raster_attrib . "<" . $attrib->{maxVal} .  "\"", 
					$shapefile,  $filename);
			print @args , "\n";
			system(@args);

		}				
	}
	
}

@args = ("L:\\gdal_bin\\gdalwarp",
			"-multi",
			"-t_srs", "epsg:4326",
			"-r", "cubic",
			$filename, "reprojected.tif");
system(@args);
@args = ("L:\\gdal_bin\\gdal_translate",
			"-of", "GTiff",
			"-co", "\"TILED=YES\"",
			".\\reprojected.tif",
			"..\\..\\Images\\PopDens\\" . $filename);
system(@args);
@args = ("L:\\gdal_bin\\gdaladdo",
			"-r", "gauss",
			"..\\..\\Images\\PopDens\\" . $filename,
			"2", "4", "8", "16");
system(@args);			
@args = ("del", ".\\reprojected.tif");
system(@args);
@args = ("del", $filename);
#system(@args);


