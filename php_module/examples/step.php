<?php
$demo = mvd_load("./test.mvd");
mvd_init($demo);
mvd_load_fragfile($demo, "./fragfile.dat");
while (mvd_step($demo) == 0)
{
	$info = mvd_get_player_info($demo);
/*
	for ($i=0; $i<count($info['players']); $i++)
	{
		print "name: " . $info['players'][$i]['name'] . "\n";
		print " frags: " . $info['players'][$i]['frags'] . "\n";
	}
*/
	
	$frags = mvd_get_frags($demo);
	//for ($i=0; $i<count($frags); $i++)
	if (count($frags) > 0)
	{
		for ($i=0; $i<count($frags); $i++)
		{
			print "time: " . $frags[$i]["time"] . "\n";
			if ($frags[$i]["killer_name_readable"])
				print "killer: " . $frags[$i]["killer_name_readable"] . "\n";
			if ($frags[$i]["victim_name_readable"])
				print "victim: " . $frags[$i]["victim_name_readable"] . "\n";
			print "weapoin: " . $frags[$i]["weapon_long_name"] . "\n";
		}
	}
}
?>
