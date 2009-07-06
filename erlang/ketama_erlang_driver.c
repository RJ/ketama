/*
 *  Expects a one-byte length header, followed by a key (<255bytes)
 *  Returns an ip:port string with 1 byte len header *
 *
 */
#include <ketama.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned char byte;


int read_exact(byte *buf, int len)
{
    int i, got = 0;
    do {
        if((i=read(0,buf+got, len-got))<=0) return i;
        got += i;
    } while(got<len);
    return len;
}

int main(int argc, char **argv)
{
    if(argc==1){
        printf("Usage: %s <ketama.servers file>\n", *argv);
        return 1;
    }

    ketama_continuum c;
    ketama_roll( &c, *++argv );
    mcs *m;

    byte len; 
    byte buffer[256];
    while ( 1 ) {
        if( 1 != read_exact(&len, 1) ) break;
        if( (int)len >= 255 ) break; 
        read_exact((byte *)&buffer, (int)len);
        buffer[len] = '\0';
        m = ketama_get_server( (char *) &buffer, c );
        sprintf((char *)&buffer, "%s",m->ip);
        int respleni = strlen(m->ip);
        char l = (0xff & respleni);
        write(1, &l, 1);
        write(1, (char*)&buffer, respleni);
    }

    return 0;
}
