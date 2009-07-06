<?php
echo "Starting\n";

// try loading server definitions from specified file.
// ketama_roll will automatically re-read the contents
// whenever the file changes.
$continuum1 = ketama_roll( "ketama.servers" );
if ( !$continuum1 )
	die( "Continuum one doesn't exist!\n" );
	
$continuum2 = ketama_roll( "ketama.two.servers" );
if ( !$continuum2 )
	die( "Continuum two doesn't exist!\n" );

// find the matching server for key $i in the continuum
// specified by resource $continuum.
for ( $i = 0; $i < 25; $i++ )
{
	$server = ketama_get_server( $i, $continuum1 );
	echo "Continuum 1: Key " .$i. " is mapped to server " . $server[ "ip" ] . " ".
	     "at point: " . $server[ "point" ] . "\n";

	$server = ketama_get_server( $i, $continuum2 );
	echo "Continuum 2: Key " .$i. " is mapped to server " . $server[ "ip" ] . " ".
	     "at point: " . $server[ "point" ] . "\n";	     
}

// not strictly needed (will be auto-cleaned)
ketama_destroy( $continuum1 );
ketama_destroy( $continuum2 );

echo "Finished\n";
?>
