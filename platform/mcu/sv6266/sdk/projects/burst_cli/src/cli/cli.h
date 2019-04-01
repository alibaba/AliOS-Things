#ifndef _CLI_H_
#define _CLI_H_

#define CLI_BUFFER_SIZE   (4096)
#define CLI_ARG_SIZE		(6)
#define CLI_PROMPT			("?>")

typedef void (*CliCmdFunc) ( s32, char  ** );


typedef struct CLICmds_st
{
        const char           *Cmd;
        CliCmdFunc          CmdHandler;
        const char           *CmdUsage;

} CLICmds, *PCLICmds;

extern const CLICmds gCliCmdTable[];

void Cli_Init( void );
void Cli_Start( void );
void Cli_Task( void *args );



#endif /* _CLI_H_ */

