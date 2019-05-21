#ifndef _ASM_GENERIC_ERRNO_BASE_H
#define _ASM_GENERIC_ERRNO_BASE_H

#define	ERRPERM		 	1	/* Operation not permitted */
#define	ERRNOENT	 	2	/* No such file or directory */
#define	ERRSRCH		 	3	/* No such process */
#define	ERRINTR		 	4	/* Interrupted system call */
#define	EERIO		 	5	/* I/O error */
#define	ERRNXIO		 	6	/* No such device or address */
#define	ERR2BIG		    7	/* Argument list too long */
#define	ERRNOEXEC	    8	/* Exec format error */
#define	ERRBADF		 	9	/* Bad file number */
#define	ERRCHILD		10	/* No child processes */
#define	ERRAGAIN		11	/* Try again */
#define	ERRNOMEM		12	/* Out of memory */
#define	ERRACCES		13	/* Permission denied */
#define	ERRFAULT		14	/* Bad address */
#define	ERRNOTBLK		15	/* Block device required */
#define	ERRBUSY		    16	/* Device or resource busy */
#define	ERREXIST		17	/* File exists */
#define	ERRXDEV		    18	/* Cross-device link */
#define	ERRNODEV		19	/* No such device */
#define	ERRNOTDIR		20	/* Not a directory */
#define	ERRISDIR		21	/* Is a directory */
#define	ERRINVAL		22	/* Invalid argument */
#define	ERRNFILE		23	/* File table overflow */
#define	ERRMFILE		24	/* Too many open files */
#define	ERRNOTTY		25	/* Not a typewriter */
#define	ERRTXTBSY		26	/* Text file busy */
#define	ERRFBIG		27	/* File too large */
#define	ERRNOSPC		28	/* No space left on device */
#define	ERRSPIPE		29	/* Illegal seek */
#define	ERRROFS		30	/* Read-only file system */
#define	ERRMLINK		31	/* Too many links */
#define	ERRPIPE		32	/* Broken pipe */
#define	ERRDOM		33	/* Math argument out of domain of func */
#define	ERRRANGE		34	/* Math result not representable */


#define	ERRNOMSG		35	/* No message of desired type */
#define	ERRIDRM		    36	/* Identifier removed */
#define	ERRCHRNG		37	/* Channel number out of range */
#define	ERRL2NSYNC	    38	/* Level 2 not synchronized */
#define	ERRL3HLT		39	/* Level 3 halted */
#define	ERRL3RST		40	/* Level 3 reset */
#define	ERRLNRNG		41	/* Link number out of range */
#define	ERRUNATCH		42	/* Protocol driver not attached */
#define	ERRNOCSI		43	/* No CSI structure available */
#define	ERRL2HLT		44	/* Level 2 halted */
#define	ERRDEADLK		45	/* Resource deadlock would occur */
#define	ERRDEADLOCK	    ERRDEADLK
#define	ERRNOLCK		46	/* No record locks available */
#define	ERRILSEQ		47	/* Illegal byte sequence */

#define	ERRNONET		50	/* Machine is not on the network */
#define	ERRNODATA		51	/* No data available */
#define	ERRTIME		    52	/* Timer expired */
#define	ERRNOSR		    53	/* Out of streams resources */
#define	ERRNOSTR		54	/* Device not a stream */
#define	ERRNOPKG		55	/* Package not installed */

#define	ERRNOLINK		57	/* Link has been severed */
#define	ERRADV		    58	/* Advertise error */
#define	ERRSRMNT		59	/* Srmount error */
#define	ERRCOMM		    60	/* Communication error on send */
#define	ERRPROTO		61	/* Protocol error */

#define	ERRMULTIHOP	    64	/* Multihop attempted */

#define	ERRDOTDOT		66	/* RFS specific error */
#define	ERRBADMSG		67	/* Not a data message */
#define	ERRUSERS		68	/* Too many users */
#define	ERRDQUOT		69	/* Quota exceeded */
#define	ERRSTALE		70	/* Stale NFS file handle */
#define	ERRREMOTE		71	/* Object is remote */
#define	ERROVERFLOW	    72	/* Value too large for defined data type */

/* these errnos are defined by Linux but not HPUX. */
#define	ERRBADE		    160	/* Invalid exchange */
#define	ERRBADR		    161	/* Invalid request descriptor */
#define	ERRXFULL		162	/* Exchange full */
#define	ERRNOANO		163	/* No anode */
#define	ERRBADRQC		164	/* Invalid request code */
#define	ERRBADSLT		165	/* Invalid slot */
#define	ERRBFONT		166	/* Bad font file format */
#define	ERRNOTUNIQ	    167	/* Name not unique on network */
#define	ERRBADFD		168	/* File descriptor in bad state */
#define	ERRREMCHG		169	/* Remote address changed */
#define	ERRLIBACC		170	/* Can not access a needed shared library */
#define	ERRLIBBAD		171	/* Accessing a corrupted shared library */
#define	ERRLIBSCN		172	/* .lib section in a.out corrupted */
#define	ERRLIBMAX		173	/* Attempting to link in too many shared libraries */
#define	ERRLIBEXEC	    174	/* Cannot exec a shared library directly */
#define	ERRRESTART	    175	/* Interrupted system call should be restarted */
#define	ERRSTRPIPE	    176	/* Streams pipe error */
#define	ERRUCLEAN		177	/* Structure needs cleaning */
#define	ERRNOTNAM		178	/* Not a XENIX named type file */
#define	ERRNAVAIL		179	/* No XENIX semaphores available */
#define	ERRISNAM		180	/* Is a named type file */
#define	ERRREMOTEIO	    181	/* Remote I/O error */
#define	ERRNOMEDIUM	    182	/* No medium found */
#define	ERRMEDIUMTYPE	183	 /*Wrong medium type */
#define	ERRNOKEY		184	/* Required key not available */
#define	ERRKEYEXPIRED	185	/* Key has expired */
#define	ERRKEYREVOKED	186	/* Key has been revoked */
#define	ERRKEYREJECTED	187	/* Key was rejected by service */

/* We now return you to your regularly scheduled HPUX. */
#define ERRNOSYM		    215	/* symbol does not exist in executable */
#define	ERRNOTSOCK	        216	/* Socket operation on non-socket */
#define	ERRDESTADDRREQ	    217	/* Destination address required */
#define	ERRMSGSIZE	        218	/* Message too long */
#define	ERRPROTOTYPE	    219	/* Protocol wrong type for socket */
#define	ERRNOPROTOOPT	    220	/* Protocol not available */
#define	ERRPROTONOSUPPORT	221	/* Protocol not supported */
#define	ERRSOCKTNOSUPPORT	222	/* Socket type not supported */
#define	ERROPNOTSUPP	223	/* Operation not supported on transport endpoint */
#define	ERRPFNOSUPPORT	224	/* Protocol family not supported */
#define	ERRAFNOSUPPORT	225	/* Address family not supported by protocol */
#define	ERRADDRINUSE    226	/* Address already in use */
#define	ERRADDRNOTAVAIL	227	/* Cannot assign requested address */
#define	ERRNETDOWN	    228	/* Network is down */
#define	ERRNETUNREACH	229	/* Network is unreachable */
#define	ERRNETRESET	    230	/* Network dropped connection because of reset */
#define	ERRCONNABORTED	231	/* Software caused connection abort */
#define	ERRCONNRESET    232	/* Connection reset by peer */
#define	ERRNOBUFS		233	/*No buffer space available */
#define	ERRISCONN		234	 /*Transport endpoint is already connected */
#define	ERRNOTCONN	    235	/* Transport endpoint is not connected */
#define	ERRSHUTDOWN	    236	/* Cannot send after transport endpoint shutdown */
#define	ERRTOOMANYREFS	237	/* Too many references: cannot splice */
#define ERRREFUSED	    ERRCONNREFUSED	/* for HP's NFS apparently */
#define	ERRTIMEDOUT	    238	/* Connection timed out */
#define	ERRCONNREFUSED	239	/* Connection refused */
#define ERRREMOTERELEASE	240	/* Remote peer released connection */
#define	ERRHOSTDOWN	    241	/* Host is down */
#define	ERRHOSTUNREACH	242	/* No route to host */

#define	ERRALREADY	    244	/* Operation already in progress */
#define	ERRINPROGRESS	245	/* Operation now in progress */
#define	ERRWOULDBLOCK	246	/* Operation would block (Linux returns EAGAIN) */
#define	ERRNOTEMPTY	    247	/* Directory not empty */
#define	ERRNAMETOOLONG	248	/* File name too long */
#define	ERRLOOP		    249	/* Too many symbolic links encountered */
#define	ERRNOSYS	    251	/* Function not implemented */

#define ERRNOTSUP		252	/* Function not implemented (POSIX.4 / HPUX) */
#define ERRCANCELLED	253	/* aio request was canceled before complete (POSIX.4 / HPUX) */
#define ERRCANCELED	    ERRCANCELLED	/* SuSv3 and Solaris wants one 'L' */

/* for robust mutexes */
#define ERROWNERDEAD	    254	/* Owner died */
#define ERRNOTRECOVERABLE	255	/* State not recoverable */

#define	ERRRFKILL		    256	/* Operation not possible due to RF-kill */

#define ERRHWPOISON	        257	/* Memory page has hardware error */
#endif
// eof

