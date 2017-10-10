/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

const char *default_ca_str = "-----BEGIN CERTIFICATE-----\n"\
                             "MIIDtzCCAp+gAwIBAgIJAOxbLdldR1+SMA0GCSqGSIb3DQEBBQUAMHIxCzAJBgNV\n"\
                             "BAYTAkNOMREwDwYDVQQIDAh6aGVqaWFuZzERMA8GA1UEBwwIaGFuZ3pob3UxEDAO\n"\
                             "BgNVBAoMB2FsaWJhYmExDjAMBgNVBAsMBXl1bm9zMRswGQYDVQQDDBIqLnNtYXJ0\n"\
                             "LmFsaXl1bi5jb20wHhcNMTQwOTE3MDI0OTM0WhcNMjQwOTE0MDI0OTM0WjByMQsw\n"\
                             "CQYDVQQGEwJDTjERMA8GA1UECAwIemhlamlhbmcxETAPBgNVBAcMCGhhbmd6aG91\n"\
                             "MRAwDgYDVQQKDAdhbGliYWJhMQ4wDAYDVQQLDAV5dW5vczEbMBkGA1UEAwwSKi5z\n"\
                             "bWFydC5hbGl5dW4uY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\n"\
                             "pwFEj4onz7YZ0ESpG7BNZmuK4KJgGMlFHnEL3AT1YtvB7DGePTNsW9hX3WYsaqy7\n"\
                             "+4PyzJgfNJY3WQr6uPv/EMwqlYMO0F8kg9AmFepuicHh5JvCeTJciG7OH/8qHhEB\n"\
                             "3b3w35un5YxUXuffw3SiFTj+vnFdc3Yj9pBv++0nsDvl6l8TSkJMnRLY8lRzzi1T\n"\
                             "rbdsDeNXLnfeThElMPFeI1h+s7amt2ktBGnv6HAg7a9OehUI8uTpFZ7559Yf8Dpm\n"\
                             "MDijYc6LLLSE6OO5C7im0pg8IRu6oZo1F5raK5gbRU/QI7K58IuIo+k4+clcvtko\n"\
                             "Ck4RkwdvC8cc0u5mJ8mXJwIDAQABo1AwTjAdBgNVHQ4EFgQUw6RWDo81JEoy+Vnf\n"\
                             "vMTvRsLkZ30wHwYDVR0jBBgwFoAUw6RWDo81JEoy+VnfvMTvRsLkZ30wDAYDVR0T\n"\
                             "BAUwAwEB/zANBgkqhkiG9w0BAQUFAAOCAQEAS2UQtfguKHnlxH9jpfJUyGKFoaTT\n"\
                             "8/XhPW3CW9c++zDrgNq920iQ42Yl9zB58iJ/v0w6n9quTtIta6QD72ssEJtWc2v2\n"\
                             "rwu14WJB5tGhBRagMvtF7p/56KYxib0p3fqjaE3Neer5r8Mgb6oI13tHbf0WT4JM\n"\
                             "GJCLsDoz4ZwvemLISeonZVSVIezs0BDU/TeEK2kIeUDB14FR6fY/U4ovS/v+han8\n"\
                             "NLhWorEpB1p2sgnSPgSVc6ZPHHyjIQOcWdn56vnOf41rLF/zqjD0Sn7YgceAd5OT\n"\
                             "rJ/t7PbiC/sn8SR7+0ATOMh0vRSA9HuuvoDz0adMhoFnba2FwiENfsLlhw==\n"\
                             "-----END CERTIFICATE-----\n";

const char *debug_ca_str = "-----BEGIN CERTIFICATE-----\n\
MIIEoTCCA4mgAwIBAgIJAMQSGd3GuNGVMA0GCSqGSIb3DQEBBQUAMIGRMQswCQYD\n\
VQQGEwJDTjERMA8GA1UECBMIWmhlamlhbmcxETAPBgNVBAcTCEhhbmd6aG91MRYw\n\
FAYDVQQKEw1BbGliYWJhIEdyb3VwMQ4wDAYDVQQLEwVBbGluazEOMAwGA1UEAxMF\n\
QWxpbmsxJDAiBgkqhkiG9w0BCQEWFWFsaW5rQGFsaWJhYmEtaW5jLmNvbTAeFw0x\n\
NDA4MjkwMzA5NDhaFw0yNDA4MjYwMzA5NDhaMIGRMQswCQYDVQQGEwJDTjERMA8G\n\
A1UECBMIWmhlamlhbmcxETAPBgNVBAcTCEhhbmd6aG91MRYwFAYDVQQKEw1BbGli\n\
YWJhIEdyb3VwMQ4wDAYDVQQLEwVBbGluazEOMAwGA1UEAxMFQWxpbmsxJDAiBgkq\n\
hkiG9w0BCQEWFWFsaW5rQGFsaWJhYmEtaW5jLmNvbTCCASIwDQYJKoZIhvcNAQEB\n\
BQADggEPADCCAQoCggEBAMHr21qKVy3g1GKWdeGQj3by+lN7dMjGoPquLxiJHzEs\n\
6auxiAiWez8pFktlekIL7FwK5F7nH1px5W5G8s3cTSqRvunex/Cojw8LbNAStpXy\n\
HrqUuDhL3DYF7x87M/7H3lqFuIlucSKNC60Yc03yuIR1I/W0di40rDNeXYuCzXIv\n\
yheg+b7zD939HOe+RS878hDW5/p75FY+ChI8GA4dPEQb5fyRrjHAXneo+S8jdnqr\n\
SCjHSS7+jI36dyEfZ72rkLNJ3v1WboH02Rchj1fFIfagn+Ij4v0ruejOTIvc/ngD\n\
OLZNTUyF4B3EG4IAZRlO12jDECc4Com0yfFQ0IxkNVMCAwEAAaOB+TCB9jAdBgNV\n\
HQ4EFgQU9iyOWx+oGSOhdlpHeWMYsHXRwwkwgcYGA1UdIwSBvjCBu4AU9iyOWx+o\n\
GSOhdlpHeWMYsHXRwwmhgZekgZQwgZExCzAJBgNVBAYTAkNOMREwDwYDVQQIEwha\n\
aGVqaWFuZzERMA8GA1UEBxMISGFuZ3pob3UxFjAUBgNVBAoTDUFsaWJhYmEgR3Jv\n\
dXAxDjAMBgNVBAsTBUFsaW5rMQ4wDAYDVQQDEwVBbGluazEkMCIGCSqGSIb3DQEJ\n\
ARYVYWxpbmtAYWxpYmFiYS1pbmMuY29tggkAxBIZ3ca40ZUwDAYDVR0TBAUwAwEB\n\
/zANBgkqhkiG9w0BAQUFAAOCAQEAO7u7ozEES2TgTepq3ZTk1VD5qh2zhcSLLr+b\n\
yDQlkbm0lnah/GOGmpr/Wlr8JSXUJEWhsLcbnG1dhbP72DzFHri8ME4wO8hbeyXU\n\
7cFOHmP4DZi+Ia2gyy/GZ66P6L9df89MJzMOt46NYn+A3rI12M0qTJ6GNdUHz2R9\n\
VGkahs6bfGQGi0Se24jj4Es+MeAlrG5U0d0wVY5Dt4jpS9wHLupyAiANbj4Ls5x2\n\
6cwS4+Q4ErezFMnKiQ5VKc6S88ohYszt82CYMPEqIiZRkCfjsVz/r6vw2DFwN0Ox\n\
8Cb9myZwXypcOZbI7M+9W8909Z+TSHW1UlNkyrIsqDGuzE866w==\n\
-----END CERTIFICATE-----\n";
