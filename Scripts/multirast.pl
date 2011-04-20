use threads;
use threads::shared;
my $counter:shared;
$counter = 0;
my $max = 53;
print "starting: '$counter' > '$max'\n";
my @threads;

for($i = 0; $i < 8; $i++)
{
	$threads[$i] = threads->create('threadfunc',"LULC_A");
}

for($i = 0; $i < 8; $i++)
{
	$result = $threads[$i]->join();	
	$result = $threads[$i]->detach();
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
		print "perl rasterize.pl Year_$count.shp idu.xml Year_$count @_ 400 @_$count.tif";
		system("perl rasterize.pl Year_$count.shp idu.xml Year_$count @_ 400 @_$count.tif");
		#print "$count :thread @_\n";
		lock $counter;
		$count = $counter++;
	}
}


