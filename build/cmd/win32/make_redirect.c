
 
/* This program redirects calls to 'make' to the correct 
 * executable in the toolchain whilst wiping most of the
 * environment variables to ensure they cannot affect the
 * building of the SDK
 */

#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <process.h>
#include <stdlib.h>

#define SAVED_PREFIX  "SAVED_"

char** get_saved_env_vars( const char* variable_name[], const char* prefixes[] );


int main( int argc, char *argv[], char** envp )
{
	int status;
	int i;
	int using_iar = 0;
    char host_os_arg[] = "HOST_OS=Win32";
	char make_base_path[] = ".\\MiCoder\\cmd\\Win32\\make.exe";

	/************************************************************/
    /*  Construct a full, safe path to the real make executable */

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

	_splitpath( argv[0], drive, dir, fname, ext );

	int make_path_len = sizeof(make_base_path) + strlen(drive) + strlen(dir);

    char * make_path  = (char*) malloc( make_path_len );
    if ( make_path == NULL )
    {
        printf( "out of memory for make path - requested %d bytes\n", make_path_len);
        return -1;
    }

    /* Start with empty string */
    strcpy( make_path, "" );

    /* Add a drive spec if present */
    if ( strlen(drive) != 0 )
    {
        strcat( make_path, drive );
    }
    /* Add a path directory if present */
    if ( strlen(dir) != 0 )
    {
        strcat( make_path, dir );
    }

    /* Add a path directory if present */
    strcat( make_path, make_base_path );

    /*****************************************************************************************************/
    /*  Construct a full, command line including the real make executable path and the HOST_OS parameter */


    /* Determine the total lenght of the command line string */
    int command_length = make_path_len + strlen(host_os_arg) + 3;  // +3 for quotes around Make path and space before host_os_arg

	for( i = 1; i < argc; i++ )
	{
	    command_length +=  strlen( argv[i] ) + 3; // +3 for two quotes and a space between args
	}


	/* Allocate a buffer for the command string */
	char * command  = (char*) malloc( command_length );
	if ( command == NULL )
	{
	    printf( "out of memory for command - requested %d bytes\n", command_length);
		return -1;
	}

    /* Allocate a buffer for the argument pointer list */
	char** arglist = (char**) malloc( (argc + 2) * sizeof(char*) );
	if ( arglist == NULL )
	{
	    printf( "out of memory for arglist - requested %d bytes\n",  (argc+2) * sizeof(char*) );
		return -1;
	}

	char** curr_arglist_pos = arglist;

    /* Add path of real make executable */
	/* surrounded by double quotes - to allow directories containing spaces to function */
	strcpy( command, "\"" );
    strcat( command, make_path );
    strcat( command, "\"" );
	*curr_arglist_pos = command;       /* Add to argument pointer list */
	curr_arglist_pos++;
	command += strlen(command) + 1;
	
	/* Add the HOST_OS argument */
    strcpy( command, host_os_arg );
	*curr_arglist_pos = command;       /* Add to argument pointer list  */
	curr_arglist_pos++;
	command += strlen(command) + 1;
	
	/* Add the other arguments - surrounded by double quotes to enable arguments with spaces to work */
	for( i = 1; i < argc; i++ )
	{
		strcpy( command, "\"" );
		strcat( command, argv[i] );
		strcat( command, "\"" );
		*curr_arglist_pos = command;    /* Add to argument pointer list */
		curr_arglist_pos++;
		command += strlen(command) + 1;

        if ( memcmp( argv[ i ], "IAR=1", 5 ) == 0 )
        {
            using_iar = 1;
        }
    }
	
	*curr_arglist_pos = NULL;


	/* Process the environment to get an environment list with most
	 * variables wiped out but some saved.
	 */
	const char* prefixes[] = { "",        SAVED_PREFIX, SAVED_PREFIX, (char *)0 };
    const char* env_vars[] = { "ComSpec", "HOME",       "PATH",       (char *)0 };

	char** env = get_saved_env_vars( env_vars, prefixes );

    /* Finally Spawn the real Make executable and wait for it to finish */
    if ( using_iar == 1 )
    {
        return _spawnvp( _P_WAIT, make_path, (const char * const *) arglist );
    }
    else
    {
        return _spawnve( _P_WAIT, make_path, (const char * const *) arglist, (const char * const *) env );
    }
}


/**
 * Creates a list of enivronment variables with modified names to
 * allow the makefile system to have limited access to variables without it being automatic.
 */
char** get_saved_env_vars( const char* variable_name[], const char* prefixes[] )
{
    int total_length = 0;
    int i = 0;
    int variable_count = 0;
    const char** curr_variable_name = variable_name;

    /* Count variables and calculate total size of variable space */
    while( *curr_variable_name != NULL )
    {
        char* val = getenv( *curr_variable_name );
        if ( val == NULL )
        {
            val = "";
        }
        total_length +=  strlen( prefixes[i] ) + strlen( *curr_variable_name ) + strlen( "=" ) + strlen( val ) + 1;  //+1 for null
        variable_count++;
        curr_variable_name++;
        i++;
    }
    total_length++; // for terminating null


    /* Allocate space for the variable strings */
    char* saved_vars = (char*) malloc( total_length );
    if ( saved_vars == NULL )
    {
        printf( "out of memory for saved environment - requested %d bytes\n", total_length );
        exit (-1 );
    }

    /* Allocate space for the variable pointer list */
    char** saved_vars_list = (char**) malloc( ( variable_count + 1 ) * sizeof(char*) );
    if ( saved_vars_list == NULL )
    {
        printf( "out of memory for saved environment list - requested %d bytes\n", ( variable_count + 1 ) * sizeof(char*) );
        exit (-1 );
    }

    /* Cycle through each variable, and add it to the string and pointer list */
    char** curr_list_pos = saved_vars_list;
    curr_variable_name = variable_name;
    char* curr = saved_vars;
    i = 0;
    while( *curr_variable_name != NULL )
    {
        /* Retrieve the current value from the environment */
        char* val = getenv( *curr_variable_name );
        if ( val == NULL )
        {
            val = "";
        }

        /* Save the variable string location in the pointer list */
        *curr_list_pos = curr;
        curr_list_pos++;

        /* Print the variable prefix/name/value into the string */
        curr += sprintf( curr, "%s%s=%s", prefixes[i], *curr_variable_name, val );
        curr_variable_name++;

        curr++; /* Ensure that each variable in the string is separated by a null */
        i++;
    }

    *curr = '\x00'; /* variable string terminator */

    *curr_list_pos = 0;  /* pointer list terminator */

    return saved_vars_list;
}
