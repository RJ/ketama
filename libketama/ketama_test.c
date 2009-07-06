/*
 * Using a known ketama.servers file, and a fixed set of keys
 * print and hash the output of this program using your modified
 * libketama, compare the hash of the output to the known correct
 * hash in the test harness.
 *
 */

#include <ketama.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  if(argc==1){
	printf("Usage: %s <ketama.servers file>\n", *argv);
	return 1;
  }

  ketama_continuum c;
  ketama_roll( &c, *++argv );

  printf( "%s\n", ketama_error() );

  int i;
  for ( i = 0; i < 1000000; i++ )
  {
    char k[10];
    sprintf( k, "%d", i );
    unsigned int kh = ketama_hashi( k );
    mcs* m = ketama_get_server( k, c );

    printf( "%u %u %s\n", kh, m->point, m->ip );
  }

  return 0;
}
