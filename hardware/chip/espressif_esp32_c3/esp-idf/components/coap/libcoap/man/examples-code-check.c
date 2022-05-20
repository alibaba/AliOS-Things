/*
* examples_code_check.c -- Validate the code in EXAMPLES of the man pages
*
* Exits with a non-zero value if there is a coding error.
*
* Copyright (C) 2020 Jon Shallow <supjps-libcoap@jpshallow.com>
*
* SPDX-License-Identifier: BSD-2-Clause
*
* This file is part of the CoAP library libcoap. Please see README for terms
* of use.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#ifdef _WIN32
#define GCC_OPTIONS "-I../include"
#else /* ! _WIN32 */
#define GCC_OPTIONS "\
  -I../include \
  -std=c99 \
  -g \
  -O2 \
  -pedantic \
  -Wall \
  -Wcast-qual \
  -Wextra \
  -Wformat-security \
  -Winline \
  -Wmissing-declarations \
  -Wmissing-prototypes \
  -Wnested-externs \
  -Wpointer-arith \
  -Wshadow \
  -Wstrict-prototypes \
  -Wswitch-default \
  -Wswitch-enum \
  -Wunused \
  -Wwrite-strings \
  -Wno-unused-function \
  -Wno-unused-but-set-variable \
  -Werror \
  "
#endif /* ! _WIN32 */

const char *inline_list[] = {
  "coap_malloc(",
  "coap_free(",
};

const char *define_list[] = {
  "coap_string_equal(",
  "coap_binary_equal(",
  "coap_log(",
};

const char *struct_list[] = {
  "coap_fixed_point_t ",
  "coap_string_t ",
  "coap_str_const_t ",
  "coap_binary_t ",
  "coap_bin_const_t ",
  "coap_opt_iterator_t ",
  "coap_opt_t ",
};

const char *number_list[] = {
  "coap_log_t ",
  "coap_pdu_type_t ",
  "coap_mid_t ",
  "coap_pdu_code_t ",
  "coap_proto_t ",
  "coap_session_state_t ",
  "coap_session_type_t ",
  "int ",
  "uint32_t ",
  "uint64_t ",
  "unsigned int ",
  "size_t ",
  "const uint8_t ",
};

int exit_code = 0;

static void check_synopsis(const char* file) {
  char buffer[1024];
  char file_name[300];
  FILE *fpcode;
  int  status;

  snprintf(file_name, sizeof (file_name), "tmp/%s-header.c", file);
  fpcode = fopen(file_name, "w");
  if (!fpcode) {
    fprintf(stderr, "fopen: %s: %s (%d)\n", file_name, strerror(errno), errno);
    exit_code = 1;
    return;
  }
  fprintf(fpcode, "#include <coap3/coap.h>\n");
  fprintf(fpcode, "#ifdef __GNUC__\n");
  fprintf(fpcode, "#define U __attribute__ ((unused))\n");
  fprintf(fpcode, "#else /* not a GCC */\n");
  fprintf(fpcode, "#define U\n");
  fprintf(fpcode, "#endif /* GCC */\n");
  fprintf(fpcode, "\n");
  fprintf(fpcode, "#include \"%s.h\"\n", file);
  fclose(fpcode);
  file_name[strlen(file_name)-1] = '\000';
  snprintf (buffer, sizeof (buffer),
           "gcc " GCC_OPTIONS " -c %sc -o %so",
           file_name, file_name);
  status = system(buffer);
  if (WEXITSTATUS(status)) {
    exit_code = WEXITSTATUS(status);
  }
  return;
}

int main(int argc, char* argv[])
{
  DIR *pdir;
  struct dirent *pdir_ent;
  char buffer[1024];
  int  status;

  if (argc != 2) {
    fprintf(stderr, "usage: %s man_directory\n", argv[0]);
    exit (1);
  }

  pdir = opendir(argv[1]);
  if (pdir == NULL) {
    fprintf(stderr, "opendir: %s: %s (%d)\n", argv[1], strerror(errno), errno);
    exit(1);
  }
  if (chdir(argv[1]) == -1) {
    fprintf(stderr, "chdir: %s: %s (%d)\n", argv[1], strerror(errno), errno);
    exit(1);
  }
  if (mkdir("tmp", 0777) == -1 && errno != EEXIST) {
    fprintf(stderr, "mkdir: %s: %s (%d)\n", "tmp", strerror(errno), errno);
    exit(1);
  }

  while ((pdir_ent = readdir (pdir)) != NULL) {
    if (!strncmp(pdir_ent->d_name, "coap_", sizeof ("coap_")-1) &&
        strstr (pdir_ent->d_name, ".txt.in")) {
      FILE*  fp;
      int skip = 1;
      int in_examples = 0;
      int in_synopsis = 0;
      int count = 1;
      char keep_line[1024] = {0};
      FILE* fpcode = NULL;
      FILE* fpheader = NULL;
      char file_name[300];
      int is_void_func = 0;
      int is_number_func = 0;
      int is_inline_func = 0;
      int is_struct_func = 0;
      char *func_start = NULL;
      int is_struct = 0;
      unsigned int i;

      fprintf(stderr, "Processing: %s\n", pdir_ent->d_name);

      snprintf(buffer, sizeof (buffer), "%s", pdir_ent->d_name);
      fp = fopen(buffer, "r");
      if (fp == NULL) {
        fprintf(stderr, "fopen: %s: %s (%d)\n", buffer, strerror(errno), errno);
        continue;
      }
      while (fgets(buffer, sizeof (buffer), fp) != NULL) {
        if (strncmp(buffer, "SYNOPSIS", sizeof("SYNOPSIS")-1) == 0) {
          in_synopsis = 1;
          snprintf(file_name, sizeof (file_name), "tmp/%s.h",
                   pdir_ent->d_name);
          fpheader = fopen(file_name, "w");
          if (!fpheader) {
            fprintf(stderr, "fopen: %s: %s (%d)\n", file_name,
                    strerror(errno), errno);
            goto bad;
          }
          continue;
        }
        if (strncmp(buffer, "DESCRIPTION", sizeof("DESCRIPTION")-1) == 0) {
          in_synopsis = 0;
          if (fpheader)
            fclose(fpheader);
          fpheader = NULL;
          check_synopsis(pdir_ent->d_name);
          continue;
        }
        if (strncmp(buffer, "EXAMPLES", sizeof("EXAMPLES")-1) == 0) {
          in_synopsis = 0;
          in_examples = 1;
          continue;
        }
        if (strncmp(buffer, "SEE ALSO", sizeof("SEE ALSO")-1) == 0) {
          break;
        }
        if (in_synopsis) {
          /* Working in SYNOPSIS section */
          size_t len;
          char outbuf[1024];
          char *cp;
          char *ecp;

          if (buffer[0] == '\n')
            continue;
          if (buffer[0] == '-')
            continue;
          if (buffer[0] == 'F') {
            /* For specific ..... is the end */
            in_synopsis = 0;
            if (fpheader)
              fclose(fpheader);
            fpheader = NULL;
            check_synopsis(pdir_ent->d_name);
            continue;
          }
          if (buffer[0] == '*' && buffer[1] == '#')
            continue;
          if (buffer[0] == '*') {
            /* start of a new function */
            is_void_func = 0;
            is_number_func = 0;
            is_struct_func = 0;
            is_inline_func = 0;
            is_struct = 0;
            func_start = NULL;

            if (strncmp(buffer, "*void ", sizeof("*void ")-1) == 0) {
              if (strncmp(buffer, "*void *", sizeof("*void *")-1) == 0) {
                func_start = &buffer[sizeof("*void *")-1];
              }
              else {
                is_void_func = 1;
                func_start = &buffer[sizeof("*void ")-1];
              }
            }

            for (i = 0; i < sizeof(number_list)/sizeof(number_list[0]); i++) {
              if (strncmp(&buffer[1], number_list[i],
                          strlen(number_list[i])) == 0) {
                if (buffer[1 + strlen(number_list[i])] == '*') {
                  func_start = &buffer[2 + strlen(number_list[i])];
                }
                else {
                  is_number_func = 1;
                  func_start = &buffer[1 + strlen(number_list[i])];
                }
                break;
              }
            }

            for (i = 0; i < sizeof(struct_list)/sizeof(struct_list[0]); i++) {
              if (strncmp(&buffer[1], struct_list[i],
                          strlen(struct_list[i])) == 0) {
                if (buffer[1 + strlen(struct_list[i])] == '*') {
                  func_start = &buffer[2 + strlen(struct_list[i])];
                }
                else {
                  is_struct_func = i + 1;
                  func_start = &buffer[1 + strlen(struct_list[i])];
                }
                break;
              }
            }

            if (strncmp(buffer, "*struct ", sizeof("*struct ")-1) == 0) {
              is_struct = 1;
            }
          }

          if (func_start) {
            /* see if COAP_STATIC_INLINE function */
            for (i = 0; i < sizeof(inline_list)/sizeof(inline_list[0]); i++) {
              if (strncmp(func_start, inline_list[i],
                          strlen(inline_list[i])) == 0) {
                is_inline_func = 1;
                break;
              }
            }
            /* see if #define function */
            for (i = 0; i < sizeof(define_list)/sizeof(define_list[0]); i++) {
              if (strncmp(func_start, define_list[i], strlen(define_list[i])) == 0) {
                break;
              }
            }
            if (i != sizeof(define_list)/sizeof(define_list[0]))
              continue;
          }

          /* Need to include use of U for unused parameters just before comma */
          cp = buffer;
          ecp = strchr(cp, ',');
          if (!ecp) ecp = strchr(cp, ')');
          outbuf[0] = '\000';
          while (ecp) {
            len = strlen(outbuf);
            if (strncmp(cp, "void", ecp-cp) == 0)
              snprintf(&outbuf[len], sizeof(outbuf)-len, "%*.*s%c",
                      (int)(ecp-cp), (int)(ecp-cp), cp, *ecp);
            else
              snprintf(&outbuf[len], sizeof(outbuf)-len, "%*.*s U%c",
                      (int)(ecp-cp), (int)(ecp-cp), cp, *ecp);
            cp = ecp+1;
            if(*cp) {
              ecp = strchr(cp, ',');
              if (!ecp) ecp = strchr(cp, ')');
            }
            else {
              ecp = NULL;
            }
          }
          if (*cp) {
            len = strlen(outbuf);
            snprintf(&outbuf[len], sizeof(outbuf)-len, "%s", cp);
          }

          len = strlen(outbuf);
          if (len > 3 && ((outbuf[len-3] == ';' && outbuf[len-2] == '*') ||
                          (outbuf[len-3] == '*' && outbuf[len-2] == ';'))) {
            if (is_inline_func) {
              strcpy(&outbuf[len-3], ";\n");
            }
            /* Replace ;* or ;* with simple function definition */
            else if (is_void_func) {
              strcpy(&outbuf[len-3], "{}\n");
            }
            else if (is_number_func) {
              strcpy(&outbuf[len-3], "{return 0;}\n");
            }
            else if (is_struct_func) {
              snprintf(&outbuf[len-3], sizeof(outbuf)-(len-3),
                     "{%s v; memset(&v, 0, sizeof(v)); return v;}\n",
                     struct_list[is_struct_func - 1]);
            }
            else if (is_struct) {
              strcpy(&outbuf[len-3], ";\n");
            }
            else {
              strcpy(&outbuf[len-3], "{return NULL;}\n");
            }
          }
          if (outbuf[0] == '*') {
            fprintf(fpheader, "%s", &outbuf[1]);
          }
          else {
            fprintf(fpheader, "%s", outbuf);
          }
          continue;
        }

        if (!in_examples) {
          continue;
        }
        /* Working in EXAMPLES section */
        if (skip) {
          if (!strcmp(buffer, "----\n") || !strcmp(buffer, "---\n") ||
              !strcmp(buffer, "--\n") || !strcmp(buffer, "-\n") ||
              !strcmp(buffer, "-----\n")) {
            /* Found start of code */
            if (strcmp(buffer, "----\n")) {
              fprintf(stderr,
                     "Unexpected start of code '%.*s' - expected ----\n",
                     (int)strlen(buffer)-1, buffer);
            }
            snprintf(file_name, sizeof (file_name), "tmp/%s-%d.c",
                     pdir_ent->d_name, count++);
            fpcode = fopen(file_name, "w");
            if (!fpcode) {
              fprintf(stderr, "fopen: %s: %s (%d)\n", file_name,
                      strerror(errno), errno);
              goto bad;
            }
            else {
              fprintf(fpcode, "/* %s */\n", keep_line);
            }
            skip = 0;
            fprintf(stderr, "Processing: %s EXAMPLE - '%d'\n",
                    pdir_ent->d_name,
                    count-1);
          }
          else if (buffer[0] == '*') {
            snprintf(keep_line, sizeof (keep_line), "%s", buffer);
          }
          continue;
        }
        if (!strcmp(buffer, "----\n") || !strcmp(buffer, "---\n") ||
            !strcmp(buffer, "--\n") || !strcmp(buffer, "-\n") ||
            !strcmp(buffer, "-----\n")) {
          /* Found end of code */

          skip = 1;
          if (fpcode) fclose(fpcode);
          keep_line[0] = '\000';
          file_name[strlen(file_name)-1] = '\000';
          snprintf (buffer, sizeof (buffer),
                   "gcc " GCC_OPTIONS " -c %sc -o %so",
                   file_name, file_name);
          status = system(buffer);
          if (WEXITSTATUS(status)) {
            exit_code = WEXITSTATUS(status);
          }
          continue;
        }
        if (fpcode) {
          if (strstr (buffer, "LIBCOAP_API_VERSION")) {
            fprintf(fpcode, "#include <coap3/coap.h>\n");
            fprintf(fpcode, "#ifdef __GNUC__\n");
            fprintf(fpcode, "#define U __attribute__ ((unused))\n");
            fprintf(fpcode, "#else /* not a GCC */\n");
            fprintf(fpcode, "#define U\n");
            fprintf(fpcode, "#endif /* GCC */\n");
            fprintf(fpcode, "#include \"%s.h\"\n", pdir_ent->d_name);
            fprintf(fpcode, "#undef U\n");
            continue;
          }
          fprintf(fpcode, "%s", buffer);
        }
      }
bad:
      fclose(fp);
    }
  }
  closedir (pdir);
  exit(exit_code);
}
