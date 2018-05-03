/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

//==============================================================================
//                                INCLUDE FILES
//==============================================================================
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


//==============================================================================
//                                    MACROS
//==============================================================================
#define INDEX_CFILE		"webpage.c"
#define INDEX_HFILE		"webpage.h"
#define OBJFILE			".objfiles"
#define FNAMESIZE		255

#define DOCGI_BEGIN_TAG			"<!--DO_CGI"
#define DOCGI_END_TAG			"DO_CGI-->"
#define JS_REAL_BEGIN_TAG		"/*REAL"
#define JS_REAL_END_TAG			"REAL*/"
#define HTML_REAL_BEGIN_TAG		"<!--REAL"
#define HTML_REAL_END_TAG		"REAL-->"
#define JS_DEMO_BEGIN_TAG		"/*DEMO*/"
#define JS_DEMO_END_TAG			"/*END_DEMO*/"
#define	HTML_DEMO_BEGIN_TAG		"<!--DEMO-->"
#define HTML_DEMO_END_TAG		"<!--END_DEMO-->"
#define JS_OPTION_BEGIN_TAG		"/*OPT"
#define JS_OPTION_TAIL			"*/"
#define JS_OPTION_END_TAG		"/*END_OPT*/"
#define	HTML_OPTION_BEGIN_TAG	"<!--OPT"
#define HTML_OPTION_TAIL		"-->"
#define HTML_OPTION_END_TAG		"<!--END_OPT-->"
#define HTML_PAGE_OPTION_TAG	"<!--AOPT"

#define OPT_DEFINE				"#"
#define OPT_NOT_DEFINE			"!#"

#define C_BLANK		0x20

//==============================================================================
//                               TYPE DEFINITIONS
//==============================================================================

//==============================================================================
//                               LOCAL VARIABLES
//==============================================================================
char *inline_c_file[] = {
	"html",
	"htm" ,
	"asp" ,
	"bin" ,
	"js" ,
	NULL ,
};


//==============================================================================
//                          LOCAL FUNCTION PROTOTYPES
//==============================================================================
static int compile( char *infile, char *prefix, char *outdir, char *indexfile, char *pflag, char *optfile);
static void write_array(FILE *h_fp, FILE *dbg_fp2, char *buf, int len, char *name, int num);
static void print_title(FILE *fp);
static int proc_file_type(char *inname, char *name );
static void print_common_head(FILE *fp);
static int option_match(char *optname, char *optfile);
//static int proc_fake(char *inblk, int *len, char *optfile);
static char* proc_fake(char *inblk, int *len, char *optfile);
void mem_move(char *startp, char *endp, int len);
int proc_tag(char **opos, int olen, char **npos, int *nlen, char *optfile);
int proc_cgi(char *outdir, FILE *outfp, char *basic_name, char *infile, int filelen);

static void usage();

//==============================================================================
//                              EXTERNAL FUNCTIONS
//==============================================================================



//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	char		*infile, *prefix, *outdir, *indexfile, *pflag, *optfile;
	int comp;

	if (argc != 7) {
		usage();
	}
	infile = argv[1];
	prefix = argv[2];
	outdir = argv[3];
	indexfile = argv[4];
	pflag=argv[5];
	optfile=argv[6];

	if (compile( infile, prefix, outdir, indexfile, pflag, optfile) < 0) {
		return -1;
	}
	return 0;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static void usage()
{
	fprintf(stderr, "usage: webcomp prefix \n");
	exit(2);
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static void print_title(FILE *fp)
{
	time_t			now;

	time(&now);
	fprintf(fp, "/*\n");
	fprintf(fp, " * Generated by WEB converter: %s", ctime(&now));
	fprintf(fp, " * Do not edit\n");
	fprintf(fp, " */\n\n");

}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static void print_common_head(FILE *fp)
{
	fprintf(fp, "#include \"web_proc.h\"\n");
	fprintf(fp, "#include \"cgi_api.h\"\n");
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
//char line_buffer1[1024];
//char line_buffer2[1024];
//char line_buffer3[1024];

static int compile( char *infile, char *prefix, char *outdir, char *indexfile, char *pflag, char *optfile)
{
	FILE			*infp, *c_outfp, *c1_outfp, *h_outfp, *idxfp, *dbg_outfp;
	char			outfile[FNAMESIZE], basic_name[FNAMESIZE];
	char			*cp, *sl;
	char			buf[1000];
	int				len, nsection;
	int				process_inline_c;
	char 			*proc_blk;
	int 			proc_blk_len;
	char			*last, *next;
	char			*inputf;
	int 			size;
	char			cmdstr[255];
	struct stat 	st;
	int 			rc;
	int 			bad = 1;
	int				do_cgi=0;
    int             total_send_len = 0, send_len = 0;


		/* open source file to read */
		printf("infile:%s\n", infile);
		if ((infp = fopen(infile, "r")) == NULL ) {
			fprintf(stderr, "Can't open infile: %s\n", infile);
			return -1;
		}

		if( !strncmp(infile, prefix, strlen(prefix)) )
			inputf = &infile[strlen(prefix)];
		else
			inputf = infile;

		//strcpy(basic_name, infile);
		process_inline_c = proc_file_type(inputf, basic_name);
		//printf("process_inline_c:%d\n", process_inline_c);
		//printf("basic_name:%s\n", basic_name);

		/* open web_filename.c to write */
		sprintf(outfile, "%s/web_%s.c", outdir, basic_name);
		if ((c_outfp = fopen(outfile, "w")) == NULL ) {
			fprintf(stderr, "Can't open outfile: %s\n", outfile);
			return -1;
		}
/*dliu,031001 :add */
        //setlinebuf(c_outfp);
        //setbuffer(c_outfp, line_buffer1, 0);

		sprintf(outfile, "%s/web_%s_1.c", outdir, basic_name);
		if ((c1_outfp = fopen(outfile, "w")) == NULL ) {
			fprintf(stderr, "Can't open c1_outfile list %s\n", outfile);
			return -1;
		}
/*dliu,031001 :add */
        //setlinebuf(c1_outfp);
        //setbuffer(c1_outfp, line_buffer2, 0);
#if 0
		/* open web_filename.h to write */
		sprintf(outfile, "%s/web_%s.h", outdir, basic_name);
		if ((h_outfp = fopen(outfile, "w")) == NULL ) {
			fprintf(stderr, "Can't open file: %s\n", outfile);
			return -1;
		}
#endif
		sprintf(outfile, "%s/web_%s.dbg", outdir, basic_name);
		if ((dbg_outfp = fopen(outfile, "w")) == NULL ) {
			fprintf(stderr, "Can't open file: %s\n", outfile);
			return -1;
		}
/*dliu,031001 :add */
        //setlinebuf(h_outfp);
        //setbuffer(h_outfp, line_buffer3, 0);

		nsection = 0;
		proc_blk = 0;
		proc_blk_len = 0;
		last = 0;

		/* write general title and common delcaration to C file */
		print_title(c_outfp);
		print_common_head(c_outfp);
		/* include related head file */
		//fprintf(c_outfp, "#include \"./web_%s.h\"\n", basic_name);

		//print_title(h_outfp);

#if 0
		/* read entire file to a block */
		while ((len = fread(buf, 1, sizeof(buf),  infp)) > 0) {
			if(!proc_blk){
				proc_blk = (char*) malloc( len );
			}
			else{
				proc_blk = (char*) realloc( (void*) proc_blk, proc_blk_len+len );
			}
			memcpy( &(proc_blk[proc_blk_len]), buf, len );
			proc_blk_len += len;
    		proc_blk[proc_blk_len] = '\0';
		}
#else
		{
			if ((rc=stat(infile, &st)))
			{
				printf("stat err =%d, %s\n", rc,strerror(rc));
				goto bad;
			}
			if (st.st_size)
			{
				len=(int)st.st_size;
				printf("read file size=%d\n",len);
				//len=10000; //(len+256)&~256;

				if ((proc_blk=(char*) malloc( len+1 ))==0)
				{
					printf("malloc err\n");
					goto bad;
				}

				if ( (rc=fread(proc_blk, len, 1,  infp)) == 1 ) {
					proc_blk_len =len;
    				proc_blk[proc_blk_len] = '\0';
				}
				else
				{
					printf("fread errr\n");
					goto bad;
				}
			}
			else
			{
				printf("file size=0\n");
				goto bad;
			}
		}

#endif


		//Create cgi function
		do_cgi = proc_cgi(outdir, c1_outfp, basic_name, proc_blk, proc_blk_len);

		printf("before proc_fake: proc_blk_len:%d\n", proc_blk_len);

		/* print page main function begin*/
		fprintf(c1_outfp, "\n\nint web_%s(struct connstruct *cn, char *datap)\n{\n", basic_name);
		//fprintf(h_outfp, "unsigned char %s[] = {\n", basic_name);

		fprintf(c1_outfp, "\n\tint send_len = 0;\n");
		fprintf(c1_outfp, "\n\tint total_send_len = 0;\n");

		if(process_inline_c) {
			int outbytes = 0;
			/* remove fake section and action real section */
			//proc_fake(proc_blk, &proc_blk_len);
			proc_blk = proc_fake(proc_blk, &proc_blk_len, optfile);
			//if((proc_blk == 0) || (proc_blk == -1))
			if(proc_blk == NULL)
				goto bad;
			printf("after proc_fake: proc_blk_len:%d\n", proc_blk_len);

			last = proc_blk;
			while (*last && ((next = strstr(last, "<%")) != NULL)) {
				char *endp;
				int outside = 0;


				if(next > last){
					/* write HTML block to array */
					write_array(h_outfp, dbg_outfp, last, (next-last), basic_name, nsection);
					//fprintf(h_outfp, "unsigned int %s_s%d_len = %d;\n\n", basic_name, nsection, (next-last));
					//fprintf(c1_outfp, "\tWEB_write_blk(cn, %s_s%d, idx, %s_s%d_len);\n\n",
					//	basic_name, nsection, basic_name, nsection);
					fprintf(c1_outfp, "\tsend_len = WEB_write_blk(cn, datap, %d, %d);\n\n",
						outbytes, (int)(next-last));
					fprintf(c1_outfp, "\tif(send_len != %d)\n \t\treturn send_len;\n \ttotal_send_len += send_len;\n\n",
						(int)(next-last));
					outbytes += (next-last);
					nsection++;
				}

				next +=2;
				if(*next == '+'){
					outside = 1;
					next++;
				}
				if((endp = strstr(next, "%>")) != NULL ) {
					if(outside) {
						if(*(endp-1) != '+') {
							printf("ERROR: Unterminated inline C in %s", infile);
							goto done;
						}
						endp--;
					}
					*endp = '\0';
					last = endp+(outside ? 3 : 2);

					fwrite(next, endp-next, 1, (outside ? c_outfp :c1_outfp));
					fprintf((outside ? c_outfp :c1_outfp), "\n\n");
				}
				else {
					printf("ERROR: Unterminated inline C in %s", infile);
					goto done;
				}
			}
			/* write rest block to array */
			if(last && *last){
				write_array(h_outfp, dbg_outfp, last, strlen(last), basic_name, nsection);

				//fprintf(c1_outfp, "\tWEB_write_blk(cn, %s_s%d, idx, %s_s%d_len);\n\n",
				//		basic_name, nsection, basic_name, nsection);
				fprintf(c1_outfp, "\tsend_len = WEB_write_blk(cn, datap, %d, %d);\n\n",
						outbytes, (int)strlen(last));
				fprintf(c1_outfp, "\tif(send_len != %d)\n \t\treturn send_len;\n \ttotal_send_len += send_len;\n\n",
						(int)strlen(last));
				nsection++;
			}
		}
		else {
			/* general binary files, ex. *.jpg, *.js,...*/

			write_array(h_outfp, dbg_outfp, proc_blk, proc_blk_len, basic_name, nsection);

			//fprintf(c1_outfp, "\tWEB_write_blk(cn, %s_s%d, idx, %s_s%d_len);\n\n",
			//			basic_name, nsection, basic_name, nsection);
			fprintf(c1_outfp, "\tsend_len = WEB_write_blk(cn, datap, 0, %d);\n\n",
						proc_blk_len);
			fprintf(c1_outfp, "\tif(send_len != %d)\n \t\treturn send_len;\n \ttotal_send_len += send_len;\n\n",
					    proc_blk_len);
			nsection++;
		}

		fprintf(c1_outfp, "\treturn total_send_len;\n");
		//fprintf(h_outfp, "\t0 };\n\n");
		/* print page main function end*/
		fprintf(c1_outfp, "\n}\n");

done:
		if(proc_blk){
			free(proc_blk);
		}

		/* Remove the prefix and add a leading "/" when we print the path */
		if (strncmp(infile, prefix, strlen(prefix)) == 0) {
			cp = &infile[strlen(prefix)];
		}
		else {
			cp = infile;
		}
		while((sl = strchr(infile, '\\')) != NULL) {
			*sl = '/';
		}
		if (*cp == '/') {
			cp++;
		}

		bad = 0;
bad:
		if(c_outfp)
			fclose(c_outfp);
		if(c1_outfp)
			fclose(c1_outfp);
		//if(h_outfp);
		//	fclose(h_outfp);
		if(infp)
			fclose(infp);
		if(dbg_outfp)
			fclose(dbg_outfp);
		if(!bad)
		{
			/* concatenate c_outfp and c1_outfp */
			sprintf(outfile, "%s/web_%s.c", outdir, basic_name);
			if ((c_outfp = fopen(outfile, "a")) == NULL ) {
				fprintf(stderr, "Can't open file: %s\n", outfile);
				return -1;
			}

			sprintf(outfile, "%s/web_%s_1.c", outdir, basic_name);
			if ((c1_outfp = fopen(outfile, "r")) == NULL ) {
				fprintf(stderr, "Can't open file: %s\n", outfile);
				return -1;
			}
			while((len = fread(buf, 1, sizeof(buf), c1_outfp)) > 0) {
				fwrite(buf, 1, len, c_outfp);
			}
			if(c_outfp)
				fclose(c_outfp);
			if(c1_outfp)
				fclose(c1_outfp);
			unlink(outfile);
		}
		else
		{
			sprintf(outfile, "%s/web_%s.c", outdir, basic_name);
			unlink(outfile);
			//sprintf(outfile, "%s/web_%s.h", outdir, basic_name);
			//unlink(outfile);
			sprintf(outfile, "%s/web_%s.dbg", outdir, basic_name);
			unlink(outfile);
			sprintf(outfile, "%s/web_%s_1.c", outdir, basic_name);
			unlink(outfile);
		}

#if 1
		// product table of direction to function
		sprintf(cmdstr, "dos2unix %s/web_%s.dbg", outdir, basic_name);
		system(cmdstr);
		sleep(2);
		sprintf(cmdstr, "dos2unix %s/web_%s.c", outdir, basic_name);
		system(cmdstr);
		sleep(2);
		if(!bad && indexfile) {
			if ((idxfp = fopen(indexfile, "a")) == NULL ) {
				fprintf(stderr, "Can't open indexfile: %s\n", indexfile);
			return -1;
			}
			if (strstr(pflag, "WEBP_COMPRESS") != NULL)	// compression
			{
				sprintf(cmdstr, "../../../tools/bin/lzma e %s/web_%s.dbg %s/web_%s.bin -d16", outdir, basic_name, outdir, basic_name);
				//sprintf(cmdstr, "gzip -c %s/web_%s.dbg > %s/web_%s.bin", outdir, basic_name, outdir, basic_name);
			}
			else
			{
				sprintf(cmdstr, "cp %s/web_%s.dbg %s/web_%s.bin", outdir, basic_name, outdir, basic_name);
			}
			system(cmdstr);

			sprintf(cmdstr, "cat %s/web_%s.bin >> webdata.bin", outdir, basic_name);
			system(cmdstr);

			sprintf(cmdstr, "%s/web_%s.bin", outdir, basic_name);
			if ((rc=stat(cmdstr, &st)))
			{
				printf("stat err =%d, %s\n", rc,strerror(rc));
				goto bad;
			}
			size = (int)st.st_size;
			if(do_cgi)
			{
				fprintf(idxfp, "{ \"%s\", web_%s, %d, %s, (void *)0, cgi_%s, 0},\n",
						inputf, basic_name, size, pflag, basic_name);
			}
			else
			{
				fprintf(idxfp, "{ \"%s\", web_%s, %d, %s, (void *)0, NULL, 0 },\n",
						inputf, basic_name, size, pflag);
			}

			fclose(idxfp);
		}
#endif

	return 0;

}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//		conveter file name, ex. setup.asp -> setup_asp
//
// PARAMETERS
//
//
// RETURN
//		return 	1 -> need to process inline c
//  			0 -> no need to process inline c
//------------------------------------------------------------------------------
static int proc_file_type(char *inname, char* name )
{
    int fl;
    int i;
    char *c;

	strcpy(name, inname);
	c = name;
	while(*c) {
		if(*c == '/')
			*c = '_';
		c++;
	}

	fl = strlen( name );

    for ( i = 0; inline_c_file[i] != NULL ; ++i ) {
		int el = strlen( inline_c_file[i] );
		if ( strcasecmp( &(name[fl - el]), inline_c_file[i] ) == 0 ){
			name[fl-el-1] = '_';
	    	return 1;
	    }
	}
	c = strrchr(name, '.');
	if(c)
		*c = '_';
    return 0;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static void write_array(FILE *h_fp, FILE *dbg_fp, char *buf, int len, char *name, int num)
{
	int i, j;

	//printf("write_array: len:%d\n", len);
	if(len == 0)
		return;

	for (i = 0; i < len; ) {

		//fprintf(h_fp, "\t");
		for (j = 0; (i < len)&&(j < 16); j++) {
			//fprintf(h_fp, "%3d,", (unsigned char)buf[i]);
			fprintf(dbg_fp, "%c", (char)buf[i]);
			i++;
		}
		//fprintf(h_fp, "\n");

	}
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static void proc_fake1(char *inblk, int *len)
{
	char *startp, *endp;
	int outlen;

	outlen = *len;


	while((startp = strstr(inblk, JS_DEMO_BEGIN_TAG)) != NULL ) {
		if((endp = strstr(startp, JS_DEMO_END_TAG)) == NULL ){
			printf("can not find %s\n", JS_DEMO_END_TAG);
			return;
		}
		endp+=strlen(JS_DEMO_END_TAG);
		outlen = endp - startp;
		memset(startp, C_BLANK, outlen);
	}

	while((startp = strstr(inblk, HTML_DEMO_BEGIN_TAG)) != NULL ) {
		if((endp = strstr(startp, HTML_DEMO_END_TAG)) == NULL ){
			printf("can not find %s\n", HTML_DEMO_END_TAG);
			return;
		}
		endp+=strlen(HTML_DEMO_END_TAG);
		outlen = endp - startp;
		memset(startp, C_BLANK, outlen);
	}

	while((startp = strstr(inblk, JS_REAL_BEGIN_TAG)) != NULL ) {
		memset(startp, C_BLANK, strlen(JS_REAL_BEGIN_TAG));
	}

	while((startp = strstr(inblk, JS_REAL_END_TAG)) != NULL ) {
		memset(startp, C_BLANK, strlen(JS_REAL_END_TAG));
	}

	while((startp = strstr(inblk, HTML_REAL_BEGIN_TAG)) != NULL ) {
		memset(startp, C_BLANK, strlen(HTML_REAL_BEGIN_TAG));
	}

	while((startp = strstr(inblk, HTML_REAL_END_TAG)) != NULL ) {
		memset(startp, C_BLANK, strlen(HTML_REAL_END_TAG));
	}
	//*len  = len ;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static int option_match(char *optname, char *optfile)
{
	FILE *opt_fp;
	char buf[255];
	char *p;

	if ((opt_fp = fopen(optfile, "r")) == NULL ) {
			fprintf(stderr, "Can't open file: %s\n", optfile);
			return -1;
	}

	while(fgets(buf, 255, opt_fp))
	{
		if((p = strstr(buf, optname)) == NULL)
			continue;

		if(!memcmp(buf, "#define", strlen("#define")))
		{
			fclose(opt_fp);
			return 1;
		}
	}

	fclose(opt_fp);
	return 0;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
int proc_cgi(char *outdir, FILE *outfp, char *basic_name, char *infile, int filelen)
{
	int newblk_len, flag;
	char *blk_end, *newblk;
	char cgilist[FNAMESIZE];
	FILE *cgifp;
	char *end = infile+filelen;

	flag=0;
	while(infile < end)
	{
		if(!memcmp(infile, DOCGI_BEGIN_TAG, strlen(DOCGI_BEGIN_TAG)))
		{
			flag=1;
			if((blk_end = strstr(infile, DOCGI_END_TAG)) == NULL )
			{
				printf("can not find %s\n", DOCGI_END_TAG);
				return 0;
			}
			break;
		}
		infile++;
	}
	if(!flag) return 0;


	newblk_len = blk_end-infile;
	newblk = (char *)malloc(newblk_len+1);
	strncpy(newblk, infile, newblk_len);
	*(newblk+newblk_len)='\0';
	newblk = newblk+strlen(DOCGI_BEGIN_TAG);

	sprintf(cgilist, "%s/cgi_list", outdir);

	if ((cgifp = fopen(cgilist, "a")) == NULL )
	{
		fprintf(stderr, "Can't open %s\n", cgilist);
		return 0;
	}
	fprintf(cgifp, "extern char *cgi_%s(struct connstruct *cn);\n", basic_name);
	fclose(cgifp);

	fprintf(outfp, "\nchar *cgi_%s(struct connstruct *cn)\n{", basic_name);

	fprintf(outfp, "%s", newblk);
	fprintf(outfp, "}\n");


	free(newblk);

	return 1;
}
//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
int proc_tag(char **opos, int olen, char **npos, int *nlen, char *optfile)
{
	char *blk_end, *optn, *optb;
	int inverse;
	int clen = 0;
	char *oend = *opos+olen;
	char *next_end, *next_opt;

	while(*opos < oend)
	{
		if(!memcmp(*opos, JS_DEMO_BEGIN_TAG, strlen(JS_DEMO_BEGIN_TAG)))
		{
			if((blk_end = strstr(*opos, JS_DEMO_END_TAG)) == NULL )
			{
				printf("can not find %s\n", JS_DEMO_END_TAG);
				return -1;
			}
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos = blk_end+strlen(JS_DEMO_END_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, DOCGI_BEGIN_TAG, strlen(DOCGI_BEGIN_TAG)))
		{
			if((blk_end = strstr(*opos, DOCGI_END_TAG)) == NULL )
			{
				printf("can not find %s\n", DOCGI_END_TAG);
				return -1;
			}
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos = blk_end+strlen(DOCGI_END_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, HTML_DEMO_BEGIN_TAG, strlen(HTML_DEMO_BEGIN_TAG)))
		{
			if((blk_end = strstr(*opos, HTML_DEMO_END_TAG)) == NULL )
			{
				printf("can not find %s\n", HTML_DEMO_END_TAG);
				return -1;
			}
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos = blk_end+strlen(HTML_DEMO_END_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, JS_REAL_BEGIN_TAG, strlen(JS_REAL_BEGIN_TAG)))
		{
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos += strlen(JS_REAL_BEGIN_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, JS_REAL_END_TAG, strlen(JS_REAL_END_TAG)))
		{
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos += strlen(JS_REAL_END_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, HTML_REAL_BEGIN_TAG, strlen(HTML_REAL_BEGIN_TAG)))
		{
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos += strlen(HTML_REAL_BEGIN_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, HTML_REAL_END_TAG, strlen(HTML_REAL_END_TAG)))
		{
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			*opos += strlen(HTML_REAL_END_TAG);
			clen = 0;
		}
		else if(!memcmp(*opos, JS_OPTION_BEGIN_TAG, strlen(JS_OPTION_BEGIN_TAG)))
		{
			next_end = next_opt = *opos+strlen(JS_OPTION_BEGIN_TAG);
			do
			{
				if((blk_end = strstr(next_end, JS_OPTION_END_TAG)) == NULL )
				{
					printf("can not find %s\n", JS_OPTION_END_TAG);
					return -1;
				}
				else
					next_end = blk_end+strlen(JS_OPTION_END_TAG);

				if((next_opt = strstr(next_opt, JS_OPTION_BEGIN_TAG)))
				{
					next_opt += strlen(JS_OPTION_BEGIN_TAG);
				}
				else
					break;

			}while(blk_end > next_opt);

			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			optn = *opos+strlen(JS_OPTION_BEGIN_TAG);
			clen = 0;

			if(!memcmp(OPT_NOT_DEFINE, optn, strlen(OPT_NOT_DEFINE)))
			{
				inverse = 1;
				optn += strlen(OPT_NOT_DEFINE);
			}
			else if (!memcmp(OPT_DEFINE, optn, strlen(OPT_DEFINE)))
			{
				inverse = 0;
				optn += strlen(OPT_DEFINE);
			}
			else
			{
				printf("%s has not correct format\n", JS_OPTION_BEGIN_TAG);
				return -1;
			}

			/* give string an end */
			if((optb = strstr(optn, JS_OPTION_TAIL)))
			{
				*optb = '\0';
				optb += strlen(JS_OPTION_TAIL);
			}
			else
			{
				printf("can not find end of option name\n");
				return -1;
			}

			*opos = optb;

			if(option_match(optn, optfile)^inverse)
			{
				if(proc_tag(opos, (blk_end - *opos), npos, nlen, optfile) < 0)
					return -1;
			}

			*opos = blk_end+strlen(JS_OPTION_END_TAG);
		}
		else if(!memcmp(*opos, HTML_OPTION_BEGIN_TAG, strlen(HTML_OPTION_BEGIN_TAG)))
		{
			next_end = next_opt = *opos+strlen(HTML_OPTION_BEGIN_TAG);
			do
			{
				if((blk_end = strstr(next_end, HTML_OPTION_END_TAG)) == NULL )
				{
					printf("can not find %s\n", HTML_OPTION_END_TAG);
					return -1;
				}
				else
					next_end = blk_end+strlen(HTML_OPTION_END_TAG);

				if((next_opt = strstr(next_opt, HTML_OPTION_BEGIN_TAG)))
				{
					next_opt += strlen(HTML_OPTION_BEGIN_TAG);
				}
				else
					break;

			}while(blk_end > next_opt);

			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			optn = *opos+strlen(HTML_OPTION_BEGIN_TAG);
			clen = 0;

			if(!memcmp(OPT_NOT_DEFINE, optn, strlen(OPT_NOT_DEFINE)))
			{
				inverse = 1;
				optn += strlen(OPT_NOT_DEFINE);
			}
			else if (!memcmp(OPT_DEFINE, optn, strlen(OPT_DEFINE)))
			{
				inverse = 0;
				optn += strlen(OPT_DEFINE);
			}
			else
			{
				printf("%s has not correct format\n", JS_OPTION_BEGIN_TAG);
				return -1;
			}

			if((optb = strstr(optn, HTML_OPTION_TAIL)))
			{
				*optb = '\0';
				optb += strlen(HTML_OPTION_TAIL);
			}
			else
			{
				printf("can not find end of option name\n");
				return -1;
			}

			*opos = optb;

			if(option_match(optn, optfile)^inverse)
			{
				if(proc_tag(opos, (blk_end - *opos), npos, nlen, optfile) < 0)
					return -1;
			}

			*opos = blk_end+strlen(HTML_OPTION_END_TAG);
		}
		else if(!memcmp(*opos, HTML_PAGE_OPTION_TAG, strlen(HTML_PAGE_OPTION_TAG)))
		{
			memcpy(*npos, *opos-clen, clen);
			*npos += clen;
			*nlen += clen;
			clen = 0;
			optn = *opos+strlen(HTML_PAGE_OPTION_TAG);

			if(!memcmp(OPT_NOT_DEFINE, optn, strlen(OPT_NOT_DEFINE)))
			{
				inverse = 1;
				optn += strlen(OPT_NOT_DEFINE);
			}
			else if (!memcmp(OPT_DEFINE, optn, strlen(OPT_DEFINE)))
			{
				inverse = 0;
				optn += strlen(OPT_DEFINE);
			}
			else
			{
				printf("%s has not correct format\n", HTML_PAGE_OPTION_TAG);
				return -1;
			}

			if((optb = strstr(optn, HTML_OPTION_TAIL)))
			{
				*optb = '\0';
				optb += strlen(HTML_OPTION_TAIL);
			}
			else
			{
				printf("can not find end of option name\n");
				return -1;
			}

			*opos = optb;

			if(!(option_match(optn, optfile)^inverse))
			{
				printf("Do not include this page file!\n");
				return -1;
			}
		}
		else
		{
			(*opos)++;
			clen++;
		}
	}

	if(clen)
	{
		memcpy(*npos, *opos-clen, clen);
		*npos += clen;
		*nlen += clen;
		clen = 0;
	}

	return 0;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
//static int proc_fake(char *inblk, int *len, char *optfile)
static char* proc_fake(char *inblk, int *len, char *optfile)
{
	char *newblk, *oldblk, *npos, *ppos;
	int outlen;

	oldblk = inblk;
	ppos = oldblk;
	if(*len == 0)
		return NULL;
	newblk = (char *)malloc(*len+1);
	memset(newblk, 0, *len+1);
	npos = newblk;
	outlen = 0;

	if(proc_tag(&ppos, *len, &npos, &outlen, optfile) < 0)
	{
		return NULL;
	}

	free(inblk);
	*len  = outlen ;
	return newblk;
}

