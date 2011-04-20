use threads;
use threads::shared;
my $counter:shared;
$counter = 0;
my $max = 52;
print "starting: '$counter' > '$max'\n";
my @threads;

for($i = 0; $i < 8; $i++)
{
	$threads[$i] = threads->create('threadfunc',"rawNew_DU");
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
		@args = "gdal_translate -srcwin 130 60 256 256 @_$count.tif crop@_$count.tif";
		print @args ,"\n";
		system(@args);
		#print "$count :thread @_\n";
		lock $counter;
		$count = $counter++;
	}
}