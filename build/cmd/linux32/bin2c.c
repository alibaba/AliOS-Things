

#pragma warning(disable:4996)  // Disable MSVC++ warnings about unsafe functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined( _MSC_VER )
int getopt(int argc, char *argv[], const char *optstring);
char   *optarg;
int     optind = 0;
extern int getopt(int ac, char *av[], const char *optstring);
extern int optind;
extern char *optarg;
#elif defined __GNUC__
#define _GNU_SOURCE
#include <getopt.h>
#else
#error Need definition for getopt
#endif
#include <sys/stat.h>

int opt_bigendian = 0;
int opt_word = 0;
int opt_indent = 8;
int opt_maxcol = 79;

char *opt_utype = NULL;

#define SWAP32(x)   ((((x) >> 24) & 0x000000ff) | \
             (((x) >>  8) & 0x0000ff00) | \
             (((x) <<  8) & 0x00ff0000) | \
             (((x) << 24) & 0xff000000))

int
convert(char *ifn, char *ofn, char *array_name)
{
    FILE *ifp, *ofp;
    int c, lc;
    struct stat istat;
    char *array_type;
    int array_len;

    if (stat(ifn, &istat) < 0) {
        fprintf(stderr, "error: could not stat input file %s: %s\n",
                ifn, strerror(errno));
        return -1;
    }

    array_type = (opt_word ? "const unsigned int" : "const unsigned char");
    array_len = (opt_word ? ((int)istat.st_size + 3) / 4 : (int)istat.st_size);

    if ((ifp = fopen(ifn, "rb")) == NULL) {
        fprintf(stderr, "error: could not open input file %s: %s\n",
                ifn, strerror(errno));
        return -1;
    }

    if ((ofp = fopen(ofn, "w")) == NULL) {
        fclose(ifp);
        fprintf(stderr, "error: could not open output file %s: %s\n",
                ofn, strerror(errno));
        return -1;
    }

    fprintf(ofp, "/* FILE-CSTYLED */\n");
    if (opt_utype != NULL) {
        fprintf(ofp,
                "#define %s %s_align._%s\n"
                "union %s_u {\n"
                "%*s%s _%s[%d];\n"
                "%*s%s align;\n"
                "} %s_align = {{\n",
                array_name, array_name, array_name,
                array_name,
                opt_indent, "", array_type, array_name, array_len,
                opt_indent, "", opt_utype,
                array_name);
    } else {
        fprintf(ofp,
                "%s %s[%d] = {\n",
                array_type, array_name, array_len);
    }

    lc = 0;

    if (opt_word) {
        while ((c = fgetc(ifp)) != EOF) {
            int i;
            unsigned int val = (unsigned int)c;
            for (i = 0; i < 3; i++) {
                if ((c = getc(ifp)) == EOF)
                    c = 0;
                val = (val << 8) | (unsigned int)c;
            }
            if (!opt_bigendian)
                val = SWAP32(val);
            if (lc > 0 && lc >= opt_maxcol - 12) {
                fprintf(ofp, ",\n");
                lc = 0;
            }
            if (lc == 0)
                lc += fprintf(ofp, "%*s0x%08x", opt_indent, "", val);
            else
                lc += fprintf(ofp, ", 0x%08x", val);
        }
    } else {
        while ((c = getc(ifp)) != EOF) {
            if (lc > 0 && lc >= opt_maxcol - 5) {
                fprintf(ofp, ",\n");
                lc = 0;
            }
            if (lc == 0)
                lc += fprintf(ofp, "%*s%d", opt_indent, "", c);
            else
                lc += fprintf(ofp, ", %d", c);
        }
    }

    if (lc > 0)
        fprintf(ofp, "\n");

    if (opt_utype != NULL)
        fprintf(ofp, "}");
    fprintf(ofp, "};\n");

    (void)fclose(ifp);

    if (fclose(ofp) < 0) {
        fprintf(stderr, "error: could not close output file %s: %s\n",
                ofn, strerror(errno));
        return -1;
    }

    return 0;
}

void
usage(void)
{
    fprintf(stderr,
            "Usage: bin2c [-w] [-eb] [-u type] [-i indent] [-m maxcol]\n"
            "                  <input> <output> <arrayname>\n"
            "    -w         Output 32-bit ints instead of bytes\n"
            "    -eb        Output for big-endian CPU (when used with -w)\n"
            "    -u type    Output a union with specified alignment type\n"
            "    -i indent  Indentation amount\n"
            "    -m maxcol  Maximum output columns to use\n");
    exit(1);
}

int
main(int argc, char **argv)
{
    int c;

    while ((c = getopt(argc, argv, "we:u:i:m:")) > 0)
        switch (c) {
        case 'w':
            opt_word = 1;
            break;
        case 'e':
            opt_bigendian = (optarg[0] == 'b');
            break;
        case 'u':
            opt_utype = optarg;
            break;
        case 'i':
            opt_indent = atoi(optarg);
            break;
        case 'm':
            opt_maxcol = atoi(optarg);
            break;
        default:
            usage();
        }

    if (optind + 3 != argc)
        usage();

    if (convert(argv[optind], argv[optind + 1], argv[optind + 2]) < 0)
        exit(1);

    exit(0);
}


#if defined( _MSC_VER )

// From XGetopt.cpp  Version 1.2
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// Description:
//     XGetopt.cpp implements getopt(), a function to parse command lines.
//
// History
//     Version 1.2 - 2003 May 17
//     - Added Unicode support
//
//     Version 1.1 - 2002 March 10
//     - Added example to XGetopt.cpp module header
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

int getopt(int argc, char *argv[], const char *optstring)
{
    char c;
    char *cp;
    static char *next = NULL;
    if (optind == 0)
        next = NULL;

    optarg = NULL;

    if (next == NULL || *next == '\0')
    {
        if (optind == 0)
            optind++;

        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
        {
            optarg = NULL;
            if (optind < argc)
                optarg = argv[optind];
            return EOF;
        }

        if (strcmp(argv[optind], "--") == 0)
        {
            optind++;
            optarg = NULL;
            if (optind < argc)
                optarg = argv[optind];
            return EOF;
        }

        next = argv[optind];
        next++;     // skip past -
        optind++;
    }

    c = *next++;
    cp = strchr(optstring, c);

    if (( cp == NULL ) || ( c == ':') )
        return '?';

    cp++;
    if ( *cp == ':')
    {
        if (*next != '\0')
        {
            optarg = next;
            next = NULL;
        }
        else if (optind < argc)
        {
            optarg = argv[optind];
            optind++;
        }
        else
        {
            return '?';
        }
    }

    return c;
}

#endif /* if defined( _MSC_VER ) */
