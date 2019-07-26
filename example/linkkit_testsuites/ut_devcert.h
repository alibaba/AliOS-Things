#ifdef SDK_TEAM_TEST
    #ifndef _UT_DEVCERT_H_
        #define _UT_DEVCERT_H_

        void ut_replace_cert(char **pk, char **ps, char **dn, char **ds, void *case_data);
        void ut_replace_subcert(char *pk, char *ps, char *dn, char *ds, int dev_index, void *case_data);

        #define TEST_REPLACE_DEVCERT(pk,ps,dn,ds) ut_replace_cert(pk,ps,dn,ds,case_data)
        #define TEST_REPLACE_SUBDEVCERT(pk,ps,dn,ds,index) ut_replace_subcert(pk,ps,dn,ds,index,case_data)

    #endif
#endif