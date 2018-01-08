/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "cut.h"

struct cut_runtime cut;

static void filter(int argc, char** argv)
{
    int i = 0;
    struct cut_case *c = NULL;
    if(argc==2 && 0==strcmp(argv[1], "all"))
        return;

    for (i = 0; i < cut.ccnt_total; i++) {
        c = cut.clist[i];
        if ((argc==2 && (NULL==strstr(c->sname, argv[1]))) ||
            (argc==3 && (NULL==strstr(c->sname, argv[1]) || NULL==strstr(c->cname, argv[2])))) {
            cut.clist[i]->skip = 1;
            cut.ccnt_skip++;
        }
    }
}

static void usage(const char* me)
{
    cut_printf("Usage: %s [OPTION] [SUITE] [CASE]\n\n" \
    "OPTION:\n" \
    "  --help  : print this help\n" \
    "  --list  : list cases\n" \
    "  --count : print case count\n" \
    "    SUITE : suite name filter, e.g. '%s all' means run all suites\n" \
    "    CASE  : case name filter\n", me, me);
}

static int parse_arg(int argc, char** argv)
{
    if (argc >= 2) {
        if (0 == strcmp(argv[1], "--list")) {
            int i = 0;
            int cnt = 0;
            cut_printf("\33[1;34mCASE_LIST_BEGIN\n");
            for(i=0; i<cut.ccnt_total; i++) {
                struct cut_case *c = cut.clist[i];
                if(argc==2 ||
                  (argc==3 && 0==strcmp(argv[2], "all")) ||
                  (argc==3 && NULL!=strstr(c->sname, argv[2])) ||
                  (argc==4 && NULL!=strstr(c->sname, argv[2]) && NULL!=strstr(c->cname, argv[3])))
                    cut_printf("\33[1;34m[%02d] %s.%s\33[0m\n", ++cnt, c->sname, c->cname);
            }
            cut_printf("\33[1;34mCASE_LIST_END\n");
            return 0;
        }
        if (0 == strcmp(argv[1], "--count")) {
            cut_printf("total %d case(s).\n", cut.ccnt_total);
            return 0;
        }
        if (0 == strcmp(argv[1], "--help")) {
            usage(argv[0]);
            return 0;
        }
    }
    return 1;
}

static void cut_result_report(struct cut_runtime *cut)
{
    int i = 0;

    /* print test result locally */
    cut_printf("===========================================================================\n");
    if (cut->ccnt_fail > 0) {
        cut_printf("FAIL LIST:\n");
        for (i = 0; i < cut->ccnt_fail; i++) {
            cut_printf("  [%02d] %s\n", i + 1, cut->cerrmsg[i]);
            cut_free(cut->cerrmsg[i]);
        }
        cut_printf("---------------------------------------------------------------------------\n");
    }
    cut_printf("SUMMARY:\n" \
               "     TOTAL:    %d\n" \
               "   SKIPPED:    %d\n" \
               "   MATCHED:    %d\n" \
               "      PASS:    %d\n" \
               "    FAILED:    %d\n", cut->ccnt_total, cut->ccnt_skip,
               cut->ccnt_total-cut->ccnt_skip, cut->ccnt_pass, cut->ccnt_fail);
    cut_printf("===========================================================================\n");
}


int cut_main(int argc, char** argv)
{
    int i = 0, cnt = 0;
    char *_argv[2] = {"None", "--list"};

    cut.ccnt_pass = 0;
    cut.ccnt_fail = 0;
    cut.ccnt_skip = 0;
    for(i=0; i<cut.ccnt_total; i++) {
        cut.clist[i]->skip = 0;
        cut.clist[i]->flag = 1;
    }

    if (0 == parse_arg(argc, argv))
        return 0;

    parse_arg(2, _argv);

    filter(argc, argv);

    for(i=0; i < cut.ccnt_total; i++) {
        cut.ccur = cut.clist[i];
        if (cut.ccur->skip)
            continue;

        cut_printf("\33[1;33mTEST [%d/%d] %s.%s...\33[0m\n",
                    ++cnt, cut.ccnt_total-cut.ccnt_skip, cut.ccur->sname, cut.ccur->cname);
        if (cut.ccur->setup)
            cut.ccur->setup(cut.ccur->data);
        cut.ccur->run((struct cut_case *)(cut.ccur->data));
        if (cut.ccur->teardown)
            cut.ccur->teardown(cut.ccur->data);
        if (cut.ccur->flag) {
            cut_printf("\33[1;32m[OK]\33[0m\n\n");
            cut.ccnt_pass++;
        }
        else {
            cut_printf("\33[1;31m[FAIL]: %s\33[0m\n\n", cut.cerrmsg[cut.ccnt_fail]);
            cut.ccnt_fail++;
        }
    }
    cut_result_report(&cut);
    return cut.ccnt_fail;
}

