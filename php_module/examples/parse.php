<?php
$demo = mvd_parse("./test.mvd");
print $demo['map'] . "\n";
print $demo['name'] . "\n";
print $demo['frames'] . "\n";
print $demo['time'] . "\n";
print count($demo['players']) . "\n";

for ($i=0; $i<count($demo['players']); $i++)
{
	print "name: " . $demo['players'][$i]['name'] . "\n";
	print "team: " . $demo['players'][$i]['team'] . "\n";
	print "frags: " . $demo['players'][$i]['frags'] . "\n";
	print "ping: " . $demo['players'][$i]['ping'] . "\n";
	print "pl: " . $demo['players'][$i]['pl'] . "\n";
}
?>
