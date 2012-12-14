/*
* Copyright (c) 2007, Last.fm, All rights reserved.
* Richard Jones <rj@last.fm>
* Christian Muehlhaeuser <chris@last.fm>
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Last.fm Limited nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Last.fm ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Last.fm BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ketama.h"
#include "md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>           /* for reading last time modification   */
#include <unistd.h>         /* needed for usleep                    */
#include <math.h>           /* floor & floorf                       */
#include <sys/stat.h>       /* various type definitions             */
#include <sys/shm.h>        /* shared memory functions and structs  */
#ifdef DEBUG
#include <syslog.h>
#endif


char k_error[255] = "";

int num_sem_ids = 0;
int num_shm_ids = 0;
int num_data = 0;
int sem_ids_size = 1024;
int shm_ids_size = 1024;
int shm_data_size = 1024;
int *sem_ids = NULL;
int *shm_ids = NULL;
int **shm_data = NULL;

static void
init_sem_id_tracker() {
    sem_ids = malloc(sizeof(int)*1024);
}

static void
init_shm_id_tracker() {
    shm_ids = malloc(sizeof(int)*1024);
}

static void
init_shm_data_tracker() {
    shm_data = malloc(sizeof(int*)*1024);
}

static void
track_shm_data(int *data) {
    if (num_data == shm_data_size) {
        void *tmp = realloc(shm_data, sizeof(int*)*(shm_data_size + 1024));
        if (tmp != NULL) {
    	    shm_data = tmp;
        } else {
            sprintf( k_error, "Cannot realloc shm data tracker");
            exit(1);
        }

        shm_data_size += 1024;
    }

    shm_data[num_data] = data;
    num_data++;
}

static void
track_sem_id(int semid) {
    if (num_sem_ids == sem_ids_size) {
    void *tmp = realloc(sem_ids, sizeof(int)*(sem_ids_size + 1024));
        if (tmp != NULL) {
            sem_ids = tmp;
        } else {
            sprintf( k_error, "Cannot realloc semids");
            exit(1);
        }

        sem_ids_size += 1024;
    }

    sem_ids[num_sem_ids] = semid;
    num_sem_ids++;
}

static void
track_shm_id(int shmid) {
    if (num_shm_ids == shm_ids_size) {
        void *tmp = realloc(shm_ids, sizeof(int)*(shm_ids_size + 1024));
        if (tmp != NULL) {
            shm_ids = tmp;
        } else {
            sprintf( k_error, "Cannot realloc shmids");
            exit(1);
        }

        shm_ids_size += 1024;
    }

    shm_ids[num_shm_ids] = shmid;
    num_shm_ids++;
}

/** \brief Locks the semaphore.
  * \param sem_set_id The semaphore handle that you want to lock. */
static void
ketama_sem_lock( int sem_set_id )
{
    union semun sem_val;
    sem_val.val = 2;
    semctl( sem_set_id, 0, SETVAL, sem_val );
}


/** \brief Unlocks the semaphore.
  * \param sem_set_id The semaphore handle that you want to unlock. */
static void
ketama_sem_unlock( int sem_set_id )
{
    union semun sem_val;
    sem_val.val = 1;
    semctl( sem_set_id, 0, SETVAL, sem_val );
}


/** \brief Initialize a semaphore.
  * \param key Semaphore key to use.
  * \return The freshly allocated semaphore handle. */
static int
ketama_sem_init( key_t key )
{
    if (sem_ids == NULL) {
        init_sem_id_tracker();
    }

    int sem_set_id;

    sem_set_id = semget( key, 1, 0 );
	track_sem_id(sem_set_id);

    if ( sem_set_id == -1 )
    {
        // create a semaphore set with ID SEM_ID
        sem_set_id = semget( key, 1, IPC_CREAT | 0666 );
        track_sem_id(sem_set_id);

		if ( sem_set_id == -1 )
        {
            strcpy( k_error, "Could not open semaphore!" );
            return 0;
        }

        ketama_sem_unlock( sem_set_id );
    }

    return sem_set_id;
}


/* ketama.h does not expose this function */
void
ketama_md5_digest( char* inString, unsigned char md5pword[16] )
{
    md5_state_t md5state;

    md5_init( &md5state );
    md5_append( &md5state, (unsigned char *)inString, strlen( inString ) );
    md5_finish( &md5state, md5pword );
}


/** \brief Retrieve the modification time of a file.
  * \param filename The full path to the file.
  * \return The timestamp of the latest modification to the file. */
static time_t
file_modtime( char* filename )
{
    struct tm* clock;
    struct stat attrib;

    stat( filename, &attrib );
    clock = gmtime( &( attrib.st_mtime ) );

    return mktime( clock );
}


/** \brief Retrieve a serverinfo struct for one a sever definition.
  * \param line The entire server definition in plain-text.
  * \return A serverinfo struct, parsed from the given definition. */
static serverinfo
read_server_line( char* line )
{
    char* delim = "\t ";
    serverinfo server;
    server.memory = 0;

    char* tok = strtok( line, delim );
    if ( ( strlen( tok ) - 1 ) < 23 )
    {
        char* mem = 0;
        char* endptr = 0;

        strncpy( server.addr, tok, strlen( tok ) );
        server.addr[ strlen( tok ) ] = '\0';

        tok = strtok( 0, delim );
        /* We do not check for a NULL return earlier because strtok will
         * always return at least the first token; hence never return NULL.
         */
        if ( tok == 0 )
        {
            strcpy( k_error, "Unable to find delimiter" );
            server.memory = 0;
        }
        else
        {
            mem = (char *)malloc( strlen( tok ) );
            strncpy( mem, tok, strlen( tok ) - 1 );
            mem[ strlen( tok ) - 1 ] = '\0';

            errno = 0;
            server.memory = strtol( mem, &endptr, 10 );
            if ( errno == ERANGE || endptr == mem )
            {
                strcpy( k_error, "Invalid memory value" );
                server.memory = 0;
            }

            free( mem );
        }
    }

    return server;
}


/** \brief Retrieve all server definitions from a file.
  * \param filename The full path to the file which contains the server definitions.
  * \param count The value of this pointer will be set to the amount of servers which could be parsed.
  * \param memory The value of this pointer will be set to the total amount of allocated memory across all servers.
  * \return A serverinfo array, containing all servers that could be parsed from the given file. */
static serverinfo*
read_server_definitions( char* filename, unsigned int* count, unsigned long* memory )
{
    serverinfo* slist = 0;
    unsigned int lineno = 0;
    unsigned int numservers = 0;
    unsigned long memtotal = 0;

    FILE* fi = fopen( filename, "r" );
    while ( fi && !feof( fi ) )
    {
        char sline[128] = "";

        if (fgets( sline, 127, fi ) == NULL)
            continue;

        lineno++;

        if ( strlen( sline ) < 2 || sline[0] == '#' )
            continue;

        serverinfo server = read_server_line( sline );
        if ( server.memory > 0 && strlen( server.addr ) )
        {
            slist = (serverinfo*)realloc( slist, sizeof( serverinfo ) * ( numservers + 1 ) );
            memcpy( &slist[numservers], &server, sizeof( serverinfo ) );
            numservers++;
            memtotal += server.memory;
        }
        else
        {
            /* This kind of tells the parent code that
             * "there were servers but not really"
             */
            *count = 1;
            free( slist );
            sprintf( k_error, "%s (line %d in %s)",
                k_error, lineno, filename );
            return 0;
        }
    }

    if ( !fi )
    {
        sprintf( k_error, "File %s doesn't exist!", filename );

        *count = 0;
        return 0;
    }
    else
    {
        fclose( fi );
    }

    *count = numservers;
    *memory = memtotal;
    return slist;
}


unsigned int
ketama_hashi( char* inString )
{
    unsigned char digest[16];

    ketama_md5_digest( inString, digest );
    return (unsigned int)(( digest[3] << 24 )
                        | ( digest[2] << 16 )
                        | ( digest[1] <<  8 )
                        |   digest[0] );
}


mcs*
ketama_get_server( char* key, ketama_continuum cont )
{
    unsigned int h = ketama_hashi( key );
    int highp = cont->numpoints;
    mcs (*mcsarr)[cont->numpoints] = cont->array;
    int lowp = 0, midp;
    unsigned int midval, midval1;

    // divide and conquer array search to find server with next biggest
    // point after what this key hashes to
    while ( 1 )
    {
        midp = (int)( ( lowp+highp ) / 2 );

        if ( midp == cont->numpoints )
            return &( (*mcsarr)[0] ); // if at the end, roll back to zeroth

        midval = (*mcsarr)[midp].point;
        midval1 = midp == 0 ? 0 : (*mcsarr)[midp-1].point;

        if ( h <= midval && h > midval1 )
            return &( (*mcsarr)[midp] );

        if ( midval < h )
            lowp = midp + 1;
        else
            highp = midp - 1;

        if ( lowp > highp )
            return &( (*mcsarr)[0] );
    }
}


/** \brief Generates the continuum of servers (each server as many points on a circle).
  * \param key Shared memory key for storing the newly created continuum.
  * \param filename Server definition file, which will be parsed to create this continuum.
  * \return 0 on failure, 1 on success. */
static int
ketama_create_continuum( key_t key, char* filename )
{
    if (shm_ids == NULL) {
        init_shm_id_tracker();
    }

    if (shm_data == NULL) {
        init_shm_data_tracker();
    }

    int shmid;
    int* data;  /* Pointer to shmem location */
    unsigned int numservers = 0;
    unsigned long memory;
    serverinfo* slist;

    slist = read_server_definitions( filename, &numservers, &memory );
    /* Check numservers first; if it is zero then there is no error message
     * and we need to set one. */
    if ( numservers < 1 )
    {
        sprintf( k_error, "No valid server definitions in file %s", filename );
        return 0;
    }
    else if ( slist == 0 )
    {
        /* read_server_definitions must've set error message. */
        return 0;
    }
#ifdef DEBUG
     syslog( LOG_INFO, "Server definitions read: %u servers, total memory: %lu.\n",
        numservers, memory );
#endif

    /* Continuum will hold one mcs for each point on the circle: */
    mcs continuum[ numservers * 160 ];
    unsigned int i, k, cont = 0;

    for( i = 0; i < numservers; i++ )
    {
        float pct = (float)slist[i].memory / (float)memory;
        unsigned int ks = floorf( pct * 40.0 * (float)numservers );
#ifdef DEBUG
        int hpct = floorf( pct * 100.0 );

        syslog( LOG_INFO, "Server no. %d: %s (mem: %lu = %u%% or %d of %d)\n",
            i, slist[i].addr, slist[i].memory, hpct, ks, numservers * 40 );
#endif

        for( k = 0; k < ks; k++ )
        {
            /* 40 hashes, 4 numbers per hash = 160 points per server */
            char ss[30];
            unsigned char digest[16];

            sprintf( ss, "%s-%d", slist[i].addr, k );
            ketama_md5_digest( ss, digest );

            /* Use successive 4-bytes from hash as numbers
             * for the points on the circle: */
            int h;
            for( h = 0; h < 4; h++ )
            {
                continuum[cont].point = ( digest[3+h*4] << 24 )
                                      | ( digest[2+h*4] << 16 )
                                      | ( digest[1+h*4] <<  8 )
                                      |   digest[h*4];

                memcpy( continuum[cont].ip, slist[i].addr, 22 );
                cont++;
            }
        }
    }
    free( slist );

    /* Sorts in ascending order of "point" */
    qsort( (void*) &continuum, cont, sizeof( mcs ), (compfn)ketama_compare );

    /* Add data to shmmem */
    shmid = shmget( key, MC_SHMSIZE, 0644 | IPC_CREAT );
    track_shm_id(shmid);

	data = shmat( shmid, (void *)0, 0 );
    if ( data == (void *)(-1) )
    {
        strcpy( k_error, "Can't open shmmem for writing." );
        return 0;
    }

    time_t modtime = file_modtime( filename );
    int nump = cont;
    memcpy( data, &nump, sizeof( int ) );
    memcpy( data + 1, &modtime, sizeof( time_t ) );
    memcpy( data + 1 + sizeof( void* ), &continuum, sizeof( mcs ) * nump );

    /* We detatch here because we will re-attach in read-only
     * mode to actually use it. */
#ifdef SOLARIS
    if ( shmdt( (char *) data ) == -1 )
#else
    if ( shmdt( data ) == -1 )
#endif
        strcpy( k_error, "Error detatching from shared memory!" );

    return 1;
}


int
ketama_roll( ketama_continuum* contptr, char* filename )
{
    if (shm_ids == NULL) {
        init_shm_id_tracker();
    }

    if (shm_data == NULL) {
        init_shm_data_tracker();
    }   

    strcpy( k_error, "" );

    key_t key;
    int shmid;
    int *data;
    int sem_set_id;

//     setlogmask( LOG_UPTO ( LOG_NOTICE | LOG_ERR | LOG_INFO ) );
//     openlog( "ketama", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1 );

    key = ftok( filename, 'R' );
    if ( key == -1 )
    {
        sprintf( k_error, "Invalid filename specified: %s", filename );
        return 0;
    }

    *contptr = malloc( sizeof( continuum ) );
    (*contptr)->numpoints = 0;
    (*contptr)->array = 0;
    (*contptr)->modtime = 0;

    sem_set_id = ketama_sem_init( key );

    int sanity = 0;
    while ( semctl( sem_set_id, 0, GETVAL, 0 ) == 2 )
    {
        // wait for the continuum creator to finish, but don't block others
        usleep( 5 );

        // if we are waiting for > 1 second, take drastic action:
        if(++sanity > 1000000)
	{
            usleep( rand()%50000 );
            ketama_sem_unlock( sem_set_id );
	    break;
        }
    }

    time_t modtime = file_modtime( filename );
    time_t* fmodtime = 0;
    while ( !fmodtime || modtime != *fmodtime )
    {
        shmid = shmget( key, MC_SHMSIZE, 0 ); // read only attempt.
        track_shm_id(shmid);

		data = shmat( shmid, (void *)0, SHM_RDONLY );

        if ( data == (void *)(-1) || (*contptr)->modtime != 0 )
        {
            ketama_sem_lock( sem_set_id );

//          if ( (*contptr)->modtime == 0 )
//              syslog( LOG_INFO, "Shared memory empty, creating and populating...\n" );
//          else
//              syslog( LOG_INFO, "Server definitions changed, reloading...\n" );

            if ( !ketama_create_continuum( key, filename ) )
            {
//                 strcpy( k_error, "Ketama_create_continuum() failed!" );
                ketama_sem_unlock( sem_set_id );
                return 0;
            }
/*          else
                syslog( LOG_INFO, "ketama_create_continuum() successfully finished.\n" );*/

            shmid = shmget( key, MC_SHMSIZE, 0 ); // read only attempt.
            track_shm_id(shmid);

            data = shmat( shmid, (void *)0, SHM_RDONLY );
            ketama_sem_unlock( sem_set_id );
        }

        if ( data == (void *)(-1) )
        {
            strcpy( k_error, "Failed miserably to get pointer to shmemdata!" );
            return 0;
        }

        (*contptr)->numpoints = *data;
        (*contptr)->modtime = ++data;
        (*contptr)->array = data + sizeof( void* );
        fmodtime = (time_t*)( (*contptr)->modtime );

        track_shm_data(data);
    }

    return 1;
}


void
ketama_smoke( ketama_continuum contptr )
{
    int i;
    if (shm_data != NULL) {
        for (i = 0; i < num_data; i++) {
            shmdt(shm_data[i]);
        }
        free(shm_data);
        shm_data = NULL;
        num_data = 0;
        shm_data_size = 1024;
    }

    if (sem_ids != NULL) {
    	for (i = 0; i < num_sem_ids; i++) {
            semctl(sem_ids[i], 0, IPC_RMID, 0);
        }
        free(sem_ids);
        sem_ids = NULL;
        num_sem_ids = 0;
        sem_ids_size = 1024;
    }

    if (shm_ids != NULL) {
        for (i = 0; i < num_shm_ids; i++) {
            shmctl(shm_ids[i], IPC_RMID, 0);
        }
        free(shm_ids);
        shm_ids = NULL;
        num_shm_ids = 0;
        shm_ids_size = 1024;
    }

    free(contptr);
}


void
ketama_print_continuum( ketama_continuum cont )
{
    int a;
    printf( "Numpoints in continuum: %d\n", cont->numpoints );

    if ( cont->array == 0 )
    {
        printf( "Continuum empty\n" );
    }
    else
    {
        mcs (*mcsarr)[cont->numpoints] = cont->array;
        for( a = 0; a < cont->numpoints; a++ )
        {
            printf( "%s (%u)\n", (*mcsarr)[a].ip, (*mcsarr)[a].point );
        }
    }
}


int
ketama_compare( mcs *a, mcs *b )
{
    return ( a->point < b->point ) ?  -1 : ( ( a->point > b->point ) ? 1 : 0 );
}


char*
ketama_error()
{
    return k_error;
}
