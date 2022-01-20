#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define PACKAGE "wgram"
#define VERSION "0.0.4"
#define MAXLINE 1024
#define MAXGRAM 32

/* status epilepticus .. print help */
void print_help(int exval);

int main (int argc, char *argv[]) {
 /* word delimeter for strtok() */
 char delim[] = ".,:;`/\"+-_(){}[]<>*&^%$#@!?~/|\\=1234567890 \t\n";
 char line[MAXLINE];     /* input buff, fgets() */
 char *stray = NULL;     /* returned value by strtok() */
 char **strarray = NULL; /* array to hold all entrys */
 int i = 0;              /* general counter */
 int strcount = 0;       /* number of entrys in pointer array */
 int N = 3, pos = 0;     /* ngram size, 3 in this case */
 int opt = 0;            /* holds command line opt nr.. */
 int word_flag = 0;      /* print only the `raw' words */
 FILE *fp = stdin;       /* read input from `FILE', default is stdin */

 while((opt = getopt(argc, argv, "hvn:wf:")) != -1) {
  switch(opt) {
   case 'h':
    print_help(0);
    break;
   case 'v':
  exit(0);
    break;
   case 'n':
    N = atoi(optarg);
    if(N > MAXGRAM || N < 2) {
     fprintf(stderr, "%s: Error - Ngram length `%d' out of range `0-%d'\n", 
       PACKAGE, N, MAXGRAM);
     return 1;
    }
    break;
   case 'w':
    word_flag = 1;
    break;
   case 'f':
    if(freopen(optarg, "r", fp) == NULL) {
     fprintf(stderr, "%s: Error - opening `%s'\n", PACKAGE, optarg);
     return 1;
    }
    break;
   case '?':
    fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE, optopt);
    print_help(1);
  } /* switch */
 } /* while */

 /* start reading lines from file pointer, add all entrys to **strarray */ 
 while((fgets(line, MAXLINE, fp)) != NULL) {
  if(strlen(line) < 2)
   continue;

  stray = strtok(line, delim);
  while(stray != NULL) {
   strarray = (char **)realloc(strarray, (strcount + 1) * sizeof(char *));
   strarray[strcount++] = strdup(stray);
   stray = strtok(NULL, delim);
  }
 }

 if(word_flag == 0) {
  /* 
  // print the array of strings, jumping back each time
  // (N - 1) positions if a whole ngram of words has been printed
  */
  for(i = 0, pos = N; i < strcount; i++, pos--) {
   if(pos == 0) pos = N, i -= (N - 1), printf("\n");
    printf("%s ", strarray[i]);
  }
  printf("\n");
 } else {
  /* print raw words */
  for(i = 0; i < strcount; i++)
   printf("%s\n", strarray[i]);
 }

 /* free the string array */
 for(i = 0; i < strcount; i++)
  free(strarray[i]);

 free(strarray);
 return 0;
}

/* status epilepticus .. print help */
void print_help(int exval) {
 printf("%s,%s extract N-grams from text data\n", PACKAGE, VERSION);
 printf("Usage: %s [-h] [-v] [-n INT] [-w] [-f FILE]\n\n", PACKAGE);

 printf(" -h        print this help and exit\n");
 printf(" -v        print version and exit\n\n");

 printf(" -n INT    set ngram length (default=3)\n");
 printf(" -w        print only the extracted words\n");
 printf(" -f FILE   read input from `FILE' (default=stdin)\n\n");
 exit(exval);
}
