use mvd;
use Data::Dumper;

$parser = mvd::load($ARGV[0]);
$data = mvd::parse($parser);

$test = "meh";

print $parser . " %parser \n";
print $parser->{'name'} . " <- name\n";
print Dumper($parser);

$player = mvd::get_player($parser, 0);

print $player . "\n";

print $player->{name} . "\n";


