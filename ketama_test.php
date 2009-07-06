<?php
echo "Starting\n";

// try loading server definitions from specified file.
// ketama_roll will automatically re-read the contents
// whenever the file changes.
$continuum = ketama_roll( "ketama.servers" );
if ( !$continuum )
	die( "Continuum doesn't exist!\n" );

// find the matching server for key $i in the continuum
// specified by resource $continuum.
for ( $i = 0; $i < 25; $i++ )
{
	$server = ketama_get_server( $i, $continuum );
	echo "Key " .$i. " is mapped to server " . $server[ "ip" ] . " ".
	     "at point: " . $server[ "point" ] . "\n";
}

// not strictly needed (will be auto-cleaned)
ketama_destroy( $continuum );

echo "Finished\n";
?>
