<?php
$demo = mvd_load("./test.mvd");
while (mvd_step($demo) == 0)
{
	$info = mvd_get_player_info($demo);
	for ($i=0; $i<count($info['players']); $i++)
	{
		print "name: " . $info['players'][$i]['name'] . "\n";
		print " frags: " . $info['players'][$i]['frags'] . "\n";
	}
}
?>
