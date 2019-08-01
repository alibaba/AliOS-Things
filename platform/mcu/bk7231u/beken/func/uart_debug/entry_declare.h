#ifndef _ENTRY_DECLARE_H_
#define _ENTRY_DECLARE_H_


#ifdef CONFIG_AUTO_COMPLETE
#define _CMD_COMPLETE(x) x,
#else
#define _CMD_COMPLETE(x)
#endif

#ifdef CONFIG_SYS_LONGHELP
#define _CMD_HELP(x) x,
#else
#define _CMD_HELP(x)
#endif

#define ENTRY_CMD_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, _comp) \
		{ #_name, _maxargs, _rep, _cmd, _usage,			\
			_CMD_HELP(_help) _CMD_COMPLETE(_comp) }

#define ENTRY_CMD(_name, _maxargs, _rep, _cmd, _usage, _help)		\
	ENTRY_CMD_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, NULL)


#endif // _ENTRY_DECLARE_H_
