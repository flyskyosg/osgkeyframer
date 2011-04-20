use threads;
use threads::shared;
my $counter:shared;
$counter = 0;
my $max = 52;
print "starting: '$counter' > '$max'\n";
my @threads;

for($i = 0; $i < 8; $i++)
{
	$threads[$i] = threads->create('threadfunc',"NEW_DU");
}

for($i = 0; $i < 8; $i++)
{
	$result = $threads[$i]->join();	
}


sub threadfunc
{
	my $count;
	{
		lock $counter;
		$count = $counter++;
	}		
	while ($count < $max)
	{
		
		@args = "gdal_rasterize  -init 0 -tr 400 400  -ot Byte -a @_ -l Year_$count Year_$count.shp raw@_$count.tif";
		
		print @args ,"\n";
		system(@args);
		
		@args = "gdaladdo -r gauss raw@_$count.tif 2 4 8 16";
		print @args ,"\n";
		system(@args);
		
		#print "$count :thread @_\n";
		lock $counter;
		$count = $counter++;
	}
}