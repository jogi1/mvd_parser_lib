use mvd;
use Data::Dumper;

$parser = mvd::load("../../test.mvd");
mvd::parse($parser);

$test = "meh";

print $parser . " %parser \n";
print $parser->{'name'} . " <- name\n";

$player = mvd::get_player($parser, 0);

print $player . "\n";

print $player->{name} . "\n";


