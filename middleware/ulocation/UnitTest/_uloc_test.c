/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define ULOC_TEST_CLOUD 1

int uloc_update_locationinfo_test(location_t ** gps, 
                                  char ** mmac, char ** macs,                   /* wifi       */
                                  char ** cdma, char ** bts,  char ** nearbts)  /* 2g network */
{

#if (ULOC_TEST_CLOUD)
    *gps  = NULL;
    *mmac = NULL;  *macs = NULL;
    *cdma = NULL;  *bts  = NULL; *nearbts = NULL;

    /* 2g testing */
    *cdma = "0";

    static char tmpbts_buf[128];
    static char tmpnearbts_buf[512];

    memset(tmpbts_buf, 0, sizeof(tmpbts_buf));
    memset(tmpnearbts_buf, 0, sizeof(tmpnearbts_buf));

#define TESTDATA6
    /* data 1 */    
#if defined (TESTDATA1)
    snprintf(tmpbts_buf, sizeof(tmpbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x1807, 0x256a, 75-113);
    snprintf(tmpnearbts_buf, sizeof(tmpnearbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x1807, 0x2569, 34-113);
#elif defined (TESTDATA2)
    /* data 2  Ali Center： */    
    snprintf(tmpbts_buf, sizeof(tmpbts_buf),         "%d,%d,%d,%d,%d", 460, 0, 0x1807, 0x256a, 76-113);
    snprintf(tmpnearbts_buf, sizeof(tmpnearbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x1844, 0x0da9, 24-113);
#elif defined (TESTDATA3)    
    /* data 3 
       close to hongqiao railway statiion, Shenchang Road, Ali-shattle bus stop
       Mellower coffee */    
    snprintf(tmpbts_buf, sizeof(tmpbts_buf),         "%d,%d,%d,%d,%d", 460, 0, 0x1807, 0x0e0d, 57-113);

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "%d,%d,%d,%d,%d", 460, 0, 0x1807, 0x4604, 39-113);
    
    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x1807, 0x0e0b, 39-113);

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x1807, 0xcec2, 29-113);
#elif defined (TESTDATA4) 
    /* Weining road subway station, north-east gate 1 */
    snprintf(tmpbts_buf, sizeof(tmpbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x1824, 0x8514, 77-113);

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "%d,%d,%d,%d,%d", 460, 0, 0x1824, 0x1d3e, 63-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x1824, 0x1d3d, 58-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x1824, 0x84b8, 40-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x1824, 0x84b7, 28-113);    

#elif defined (TESTDATA5) 
    /* Zhongshana park subway station, gate number 5 */
    snprintf(tmpbts_buf, sizeof(tmpbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x18a7, 0x5e0f, 65-113);

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "%d,%d,%d,%d,%d", 460, 0, 0x18a7, 0x5e7c, 50-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x189d, 0x5fe5, 47-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x189b, 0x605f, 46-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x189d, 0x5fe6, 44-113);    

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x189b, 0x6037, 43-113);  
    
    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x189b, 0x60ca, 43-113);      

#elif defined (TESTDATA6) 
    /* century avenue subway station */
    snprintf(tmpbts_buf, sizeof(tmpbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x180b, 0x0c3f, 50-113);

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "%d,%d,%d,%d,%d", 460, 0, 0x180b, 0x0c44, 42-113);   
    
    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x180b, 0x66a4, 41-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x1860, 0x6549, 39-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x180b, 0x6718, 35-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x180b, 0x66c8, 34-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x180b, 0x6739, 34-113);   


#elif defined (TESTDATA7) 
    /* zhangjiang high tech subway station */
    snprintf(tmpbts_buf, sizeof(tmpbts_buf), "%d,%d,%d,%d,%d", 460, 0, 0x18c2, 0x5203, 49-113);

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "%d,%d,%d,%d,%d", 460, 0, 0x188d, 0x5da9, 41-113);   
    
    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x18c2, 0x51cb, 38-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x18c2, 0x520d, 31-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x18c2, 0x16c8, 30-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x18c2, 0x16c9, 30-113);   

    snprintf(tmpnearbts_buf + strlen(tmpnearbts_buf), 
             sizeof(tmpnearbts_buf) - strlen(tmpnearbts_buf),
             "|%d,%d,%d,%d,%d", 460, 0, 0x18c2, 0x5211, 23-113); 

#endif  /* defined (TESTDATA1) */

    *bts     = tmpbts_buf;
    *nearbts = tmpnearbts_buf;

    /*
    static location_t tmp_gps;
    tmp_gps.outdoor.longitude = 10;
    tmp_gps.outdoor.latitude  = 10;
    *gps = &tmp_gps;
    */
    
#endif /* ULOC_TEST_CLOUD */

    return 0;
}
