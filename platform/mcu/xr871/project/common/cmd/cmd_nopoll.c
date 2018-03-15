/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include "net/nopoll/nopoll.h"

static const char g_root_pem[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIID2TCCAsGgAwIBAgIJAM55lRNEqQ/FMA0GCSqGSIb3DQEBCwUAMIGCMQswCQYD\r\n"
"VQQGEwJFUzEPMA0GA1UECAwGTWFkcmlkMQ8wDQYDVQQHDAZNYWRyaWQxDTALBgNV\r\n"
"BAoMBEFTUEwxEzARBgNVBAsMClRJIFN1cHBvcnQxDTALBgNVBAMMBFJvb3QxHjAc\r\n"
"BgkqhkiG9w0BCQEWD3NvcG9ydGVAYXNwbC5lczAeFw0xNzA4MTMwNDMzNTdaFw0y\r\n"
"MDA2MDIwNDMzNTdaMIGCMQswCQYDVQQGEwJFUzEPMA0GA1UECAwGTWFkcmlkMQ8w\r\n"
"DQYDVQQHDAZNYWRyaWQxDTALBgNVBAoMBEFTUEwxEzARBgNVBAsMClRJIFN1cHBv\r\n"
"cnQxDTALBgNVBAMMBFJvb3QxHjAcBgkqhkiG9w0BCQEWD3NvcG9ydGVAYXNwbC5l\r\n"
"czCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPKbeM0iCe0u/IaZcDAA\r\n"
"8uWRGgKQZaXLOZmC4i6DaARX93UWyqY/BL5iTgDhW2hEVd+IKHg6gfqT8M4MhZAN\r\n"
"HplrZC+5kB8zBebhsTOy0xlTc7TZUGGN32IlJL7yX6uG0dAezEHiLLwbnszd6LxK\r\n"
"qv9+bI2dXcbpUyPpgSoPwEM+DYmmzaHvJYOjh23P9Qqsb9BxIvQMgQnevo7mKV7+\r\n"
"jem4J6zZURFgoSjqGAKX9DKmU73WUpf+aGByqCYA/c7Mq+Zc0zcMv42Z/GF34iST\r\n"
"8PA98LnhvzsA/XvDJ9Ilw5xuSx/AwX3luyrq/3h9r3XGH4IfPh74i/R253nR5gz1\r\n"
"fRcCAwEAAaNQME4wHQYDVR0OBBYEFO/TotXSWOl7YvpaGlS2fz5SvrJ4MB8GA1Ud\r\n"
"IwQYMBaAFO/TotXSWOl7YvpaGlS2fz5SvrJ4MAwGA1UdEwQFMAMBAf8wDQYJKoZI\r\n"
"hvcNAQELBQADggEBAM1F0Rs8y40qAHxE0NGAbhueoD7FydD12tn1Iwctox958/9k\r\n"
"UKZpHKZLpK7r4o9ysEJm5achMSNjOuVP7E5GeCONvrlIMB0ofW+5ilN23X4cQQpb\r\n"
"6CciICtYN00PR0RhpljvVqssv5m7OZA2ak2+mmTcxki0ZfBpkiID8EFEygPXkOgS\r\n"
"noTZmeh73OhQxHRQmmoqcZMmelcE0Mkygw/MVmdFhwM9kjkQ6agXXeWLJ83ORVCm\r\n"
"5bOt2dFsDvGwb+qo0EedNsBswthPlx66NRHD3+ULwQdNFIlE9MFVezwIWRyI+F+D\r\n"
"+5hCmkR6W8fWKxXLJqR501AsSqJCloKa3EOUCxQ=\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEA8pt4zSIJ7S78hplwMADy5ZEaApBlpcs5mYLiLoNoBFf3dRbK\r\n"
"pj8EvmJOAOFbaERV34goeDqB+pPwzgyFkA0emWtkL7mQHzMF5uGxM7LTGVNztNlQ\r\n"
"YY3fYiUkvvJfq4bR0B7MQeIsvBuezN3ovEqq/35sjZ1dxulTI+mBKg/AQz4NiabN\r\n"
"oe8lg6OHbc/1Cqxv0HEi9AyBCd6+juYpXv6N6bgnrNlREWChKOoYApf0MqZTvdZS\r\n"
"l/5oYHKoJgD9zsyr5lzTNwy/jZn8YXfiJJPw8D3wueG/OwD9e8Mn0iXDnG5LH8DB\r\n"
"feW7Kur/eH2vdcYfgh8+HviL9HbnedHmDPV9FwIDAQABAoIBAQDO6vtlOhrtSJ3o\r\n"
"zwV4a9a/JbrhZPbNKT+/RnpS4ZDd44kTPJUqpaUOHBobdhnYHDCBrkeWA5DIf1Vq\r\n"
"6BMigY4PnCCe882QTpxCB0xzo5mYiCyvmomMKszTkIp+sYoXaa2ZrCraJmWxKn2O\r\n"
"jdXqgmTF9LXxMr7A4FAg2Q9pKHX64lDcYQKinB6ehuCa5TB/I4KZUdhck7zGYy4o\r\n"
"vaFM57tWo36Cl9VWC/kpwBnzecSIASm/EI+Tro0shybLzdBzbgC4AmMHl/s0sVmy\r\n"
"nh6LbDrLW/HLDyIqreGpkg5WL7sforz62v05BLN3yRNhf4wXJr+yvy9+pl39SA34\r\n"
"TcqW79ABAoGBAPoWc8fF+/qyxnv75WqmjaPqfIyFAzEGin0ftyAGjFPuiBV7qLWf\r\n"
"F5UaKuCPwJKXziAoOpC9eCLnKNa9b4x3lMUoUAQaswePUSKZetjygg165NZcKbaU\r\n"
"VRe7ZomHfoZCMmCuPxDcPGvzkqWILggSUwT706dxP8FKzUT9GMHG/nUBAoGBAPhX\r\n"
"v24Llj5xSFQ9T96tv05ZbDUzJhF/D5KJQPGgze8Y+yPcc0qXwvNDWv2deP956ozD\r\n"
"izPR0BiTif7BLYtGl1QCqVsHH1bXNL41zDoQyd0MYbnvgMtO63JzEnXgjeEPf9MB\r\n"
"URGrzTtybH0QbjjjyOHDYN2ov6ibo4BDqIff1voXAoGAQ5S5qOyZkT+qX2rMritf\r\n"
"70G4coinOiJYXkOpyt+6IBw/bf7CsoHRb5toptJx6atYKlx0pxRsWJI6+PysHjta\r\n"
"WoTwSoukgZB7DmyHRAUUm+vv1vWE2C/xZr5fkR6qVEGjN3ke1cGOGmXx5QWN2uSI\r\n"
"xrn+kisKXYOHQPNcM1FBBgECgYBYTaMktvwCUfd89Jwcur0GV+eQ9xH3MK3R1foA\r\n"
"sMPo24IZGz7nbBUGuJF3EnI+rRQIX1N05orO4k80BW5zUnNmjNe5JUwwba80Xx6y\r\n"
"GtwcPFXXbmDmabZwaJhMz77DIua5mYNA/EyqcaI/ygLXOa9pDza3OlSD2GuHpQcc\r\n"
"4SuomwKBgQCb9TSOwLBvhUQuJeLnKOFJ2BNseYUVMWGq1+Kkg/47UfA/fPH8dk8q\r\n"
"8OuTKucLMi8AiBb9LKlnF87/019E1HReOuUIsrBfgBWwfsZUIypc8lGQHvoBjj1D\r\n"
"G98fdnnfXT6Xs5YwKCGi+DF7ZKq402GsjTa+XByAN95PdxEKqCUQnw==\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

static const char g_server_pem[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDkzCCAnsCCQDpTEc56X02UDANBgkqhkiG9w0BAQsFADCBgjELMAkGA1UEBhMC\r\n"
"RVMxDzANBgNVBAgMBk1hZHJpZDEPMA0GA1UEBwwGTWFkcmlkMQ0wCwYDVQQKDARB\r\n"
"U1BMMRMwEQYDVQQLDApUSSBTdXBwb3J0MQ0wCwYDVQQDDARSb290MR4wHAYJKoZI\r\n"
"hvcNAQkBFg9zb3BvcnRlQGFzcGwuZXMwHhcNMTcwODEzMDQzMzU3WhcNMjAwNjAy\r\n"
"MDQzMzU3WjCBkzELMAkGA1UEBhMCRVMxDzANBgNVBAgMBk1hZHJpZDEPMA0GA1UE\r\n"
"BwwGTWFkcmlkMQ0wCwYDVQQKDARBU1BMMRMwEQYDVQQLDApUSSBTdXBwb3J0MR4w\r\n"
"HAYDVQQDDBVzZXJ2ZXIubm9wb2xsLmFzcGwuZXMxHjAcBgkqhkiG9w0BCQEWD3Nv\r\n"
"cG9ydGVAYXNwbC5lczCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALgr\r\n"
"2XCOc1Ee/O8QnMV2aWwqarsQDlEjwtn6uvwz7MWM42iKgPfbA+eJzhdqCmbnwNsJ\r\n"
"K6aC8Xt0P/OjVlC8UNgGk6IcGhceoteyQXVEYF3ccVMDOX37xwAAcVbRIXXA0sAc\r\n"
"H+kXOPnjuJiQyaZa4K/Wv6WFQKVZ5tefg1dcn1TiS/1gbLNhqgGBPEueNH5+S0/2\r\n"
"2LiG9+IN8hA+Xag604d7ryscmhim1JpwHx57jd96a4WiZ744vz/V8rQB3Gx0iwJV\r\n"
"wj+6NEkxvDmDp0N5IFwQNgEqVTZg900w1I1aNVn1tP4kiNtT3mzyrMYjvKTbunOt\r\n"
"UfRvdND3U6y+7VnZG4MCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAKoxmoX+DNLyA\r\n"
"2Wj/WO/HGY38tQboQYhFUftWLyNxFIXL+BibIPGuPcdKHJI56V+pXRDopdZ4s8Wm\r\n"
"zv+UW2/EjKS1QsGl7C780OzRFFUgOJfRM2WpdvY2DhBpyiKOG3+n9N1Yawk1bWe9\r\n"
"sjL34kcNdSwU48BQsSLhhdqhnXrJOwM6khIHoWcWrDexBD6cYkqzB1vJIuRx/mEy\r\n"
"LzV6vLARttavNljLr9m3jCI4jl/xrVVn1enhoBoR+EaX/NRC25RTnu4NpVNIZsjH\r\n"
"QG7o4N84J5qiLhzSVwsMFaOgZFboEtsIopdn54GuxLPuW8Lj96pQwI7Zn7BcGqqj\r\n"
"epOMHKhd3w==\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAuCvZcI5zUR787xCcxXZpbCpquxAOUSPC2fq6/DPsxYzjaIqA\r\n"
"99sD54nOF2oKZufA2wkrpoLxe3Q/86NWULxQ2AaTohwaFx6i17JBdURgXdxxUwM5\r\n"
"ffvHAABxVtEhdcDSwBwf6Rc4+eO4mJDJplrgr9a/pYVApVnm15+DV1yfVOJL/WBs\r\n"
"s2GqAYE8S540fn5LT/bYuIb34g3yED5dqDrTh3uvKxyaGKbUmnAfHnuN33prhaJn\r\n"
"vji/P9XytAHcbHSLAlXCP7o0STG8OYOnQ3kgXBA2ASpVNmD3TTDUjVo1WfW0/iSI\r\n"
"21PebPKsxiO8pNu6c61R9G900PdTrL7tWdkbgwIDAQABAoIBAHl0GWH73xppGB5D\r\n"
"0075Js65LkeMcBScQ7xid/sn9wXUQofHhwNtAKcpnqqcOJ3iSbFo28tkYQGsCRyP\r\n"
"47U8awf7VlK6u43xmywplrdRz0c2v9khsEFfz701jYjWQ32XGqSsJ3Mc6GsK/MpH\r\n"
"+WNmXPQ454utU5N3tR8PR5tOLpjXwItT0QvtbojP58vvvdRAmEbG0m3jSV8vhxuu\r\n"
"7/SRtWZhDk/aHYv47bdwyb8xQYneAjXjF5CINQWKEs6qnZRlvGWiiGgZR+zyoFQ6\r\n"
"H8jN8zYsEnLbII+ODDRkCXkVeEmJ9bZl7fARpYXQoCraYra1Tyk4RkkVYC8hjUji\r\n"
"ruf4LnECgYEA58lvPw/dTs6AXAnQz8A9s4743REvSy2ivR5zFzsPkfcpplMNmhBH\r\n"
"JsolBrRBsFzd1O+Wh8uENFmez7l2mnGlpn/n8dDBzN7Iy9sJ+o9dQhRRQX3mtKr7\r\n"
"80MBmTMxiGCgNk2Zc1/K4Ze8B/1gQlyIWcwj1A/bQEpDBYA/mU9jVkkCgYEAy2kN\r\n"
"7AQKZfRcmFM+SZi8xyr4ulI4Wb7wbrIVIROv9l3ea0jAz1etozt/HOShpj0s3jYm\r\n"
"5nXEGAwu7MOaSKMwRG323KA3+pRHUoyISA4OWeY0dVJbmi3HYT8eqkWFupXwbgBi\r\n"
"x5dFQ0a0EfK9J3THs1Q6mhwF8XWX6ZTWPIoHs2sCgYEAihhjZUkIa5lwtGVMuVop\r\n"
"6L732o4QhVPtL4viuiaAAlElyWiWJAIqwhuRr8rZkD/kEqekIcfJPn9etRgkZ6Pq\r\n"
"M6blBBeCP5NEQuB/s36S63m7Z6+hMmbksWxsmV/81Onsyi21jR23fGywpHn7Tc5f\r\n"
"llCf0AqacFVOVQqx2Z+I6bECgYEAwbueEeNLUA2sPzxRf+EkM4G/Ah6bIwcTkiq0\r\n"
"qxtIJZsD4ySY1yJH88cP6yHqwB1V5ZKn/CnACbNY4hHumS4sI5CkZT+H8XZWc837\r\n"
"Yr1Yd9Ekt3UoX7EoS3vFpiCvHKYAJ5Tgf/6Ybloh2VJbTAASz77yTaNRGceJV2R1\r\n"
"o5VHfl8CgYB3ZZMaMTP7f27CtKFqk5x/nreBPaT0KWe3nI06AO7o0odY5BnkjBxB\r\n"
"tRj/fbqi8CLMcD4shbUr6RSAbf8lsmmiUgsDUYZA7e6wspn4S1f2z9uREGzfUopm\r\n"
"3i20zlGmTOFFr96ny6qx/meiZBvVy7Kl5kWKRM1B6Pu6X4mtxi0hvA==\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

static const char g_client_pem[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDkzCCAnsCCQDpTEc56X02UTANBgkqhkiG9w0BAQsFADCBgjELMAkGA1UEBhMC\r\n"
"RVMxDzANBgNVBAgMBk1hZHJpZDEPMA0GA1UEBwwGTWFkcmlkMQ0wCwYDVQQKDARB\r\n"
"U1BMMRMwEQYDVQQLDApUSSBTdXBwb3J0MQ0wCwYDVQQDDARSb290MR4wHAYJKoZI\r\n"
"hvcNAQkBFg9zb3BvcnRlQGFzcGwuZXMwHhcNMTcwODEzMDQzMzU4WhcNMjAwNjAy\r\n"
"MDQzMzU4WjCBkzELMAkGA1UEBhMCRVMxDzANBgNVBAgMBk1hZHJpZDEPMA0GA1UE\r\n"
"BwwGTWFkcmlkMQ0wCwYDVQQKDARBU1BMMRMwEQYDVQQLDApUSSBTdXBwb3J0MR4w\r\n"
"HAYDVQQDDBVjbGllbnQubm9wb2xsLmFzcGwuZXMxHjAcBgkqhkiG9w0BCQEWD3Nv\r\n"
"cG9ydGVAYXNwbC5lczCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALqc\r\n"
"fTM7ADZT3TNLX5dojfpmwh8nxcQjM9/JeC5aPa027hds9js80JlQVeoAcgWNuJjD\r\n"
"V9ISOnAEED6taOrz4cNMRweHbniYUJAC0ADaGJNVjhl7Y1TsQ2v+Ja/Pypj8tSJp\r\n"
"14sKH8qFJz5wm0e8F0x2ZpoMxkhp7G1lFW/3CpjZEcpIwzh0BL6AhngIPgohAmQt\r\n"
"v+SkKhooZhy3aDh87ex1Al4BjQ7z2pckK/mYLKOnZFZS9KRxLC1nB2H+SdwzRd3b\r\n"
"woP5V+OkjdoLFPvPLH+AFlvrYMRj/Rhc5cHSTyqM1DO6sMpCuKOuXClmkjURAXiA\r\n"
"Q7CJTPZs/FurvHbgiSkCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEArXstlNlqZgYx\r\n"
"2qImPdZeKBMGS9ymWcb7Vh3JgqMcERZzwjq5cYiY6+CTL+oAyZ0iWGRqTAUQkuZN\r\n"
"iQ18j5norcGJTTC3hmJqJTDE0P60f/0L84vd/T+nJ9wmwIPcfkK+KcdKlfEdSXNg\r\n"
"dDzLb1nBTyDnb6+uPZIxA3ujO+szSBqD1c7fUh//+DhjAZoUD6EMFixTRmjnD90c\r\n"
"gEemQl++Z/6f1T3XzSiszo/+K1rJISyG/nQ08jo3uH0qBIPhjziNqoZ6LvZ8BAwr\r\n"
"Bvvz7zkPtCWoxEiHq+nhzS4lS6fSiHJ5Zeu9tUeljhbE7Zg0y6T2jqqE22npcYRZ\r\n"
"E2Z//IOQnA==\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAupx9MzsANlPdM0tfl2iN+mbCHyfFxCMz38l4Llo9rTbuF2z2\r\n"
"OzzQmVBV6gByBY24mMNX0hI6cAQQPq1o6vPhw0xHB4dueJhQkALQANoYk1WOGXtj\r\n"
"VOxDa/4lr8/KmPy1ImnXiwofyoUnPnCbR7wXTHZmmgzGSGnsbWUVb/cKmNkRykjD\r\n"
"OHQEvoCGeAg+CiECZC2/5KQqGihmHLdoOHzt7HUCXgGNDvPalyQr+Zgso6dkVlL0\r\n"
"pHEsLWcHYf5J3DNF3dvCg/lX46SN2gsU+88sf4AWW+tgxGP9GFzlwdJPKozUM7qw\r\n"
"ykK4o65cKWaSNREBeIBDsIlM9mz8W6u8duCJKQIDAQABAoIBADj34wTLw0HSklnB\r\n"
"bdpEEevaiPKH6sj40jJESAsmg2OovECsg/MLmsbjDodUQXrOjAcew2WfKPXfV7I6\r\n"
"sCfh8g7z708uGhx4q0d7hFJuQkhgOvQuheF8Cw0h1hF5BulzZDywQRvyKEhkI9nc\r\n"
"+JqKojY2gxYORk3/AC1ZSZUXvsVwIWrDzx14y2fiHixwgrudBVD3V+cNXtivGV3N\r\n"
"dEt3Q/taTLUGLm9z0iRu0fY5O4Q7NGt7tBWmeKqd+tSQQf+IZIuv4JSETnnKMBFz\r\n"
"fdG1V379eBt9G60k31DyLwdnNXZ1UXwF8Yiuh3FWoTdF47EIjUvhY3W79eamcxQ0\r\n"
"ZG0OEk0CgYEA5lizFcihcYx2ABSnQaUBfntpEiRJ9aXMugJIxctJtvg/0LPvrMBI\r\n"
"wz/ISjRTNXQfzgvFK2ho9Z9ZCn5gACgg9TajoJXhiEW3YJmv+bQCkwHqvmlfrYhQ\r\n"
"CHskWAcC2wG4Eai4mhrquBZWC0P407FkrIPqXR5VuP0Lnt968YP7xa8CgYEAz2Tg\r\n"
"NoL6hi/BvkjiJEbtx6ysDIxfmxy/c3v8zeTbSnjUa7NfpDlmHZ7foeHJwsTvB9Es\r\n"
"iGhpolfewPyBJTNRaKTO7KG0h9h4ZNOzswUepE+y9sQYXzqWLrVIa59Fd/OD1DjX\r\n"
"tyvRWnE+vlMi2jr90oMoQ6gtxgW8f5OqEs1/rKcCgYEAmt0VgdbCW9HgX4uDK4oh\r\n"
"KAUWjgDhzIpsyr3QaVKUO7RJSyo+P/mCsepG4USukKiC3YdiRx3KOyCnv3tZVaGl\r\n"
"DtduSP+bgn/EmuiYvBt/A5DtXpj/n26eyCTqOVh1XgKY2x6BBfKEzcEOlkJbnw+L\r\n"
"rMR/o51KarWNw4Fiac+fTLcCgYBcBk7W4Vd4o0g8vupZXn+z0hKlnsPtd9wbyjBq\r\n"
"y9niOatH/te5bx64lf51PlLgcbH+rfosrOfyeTsGNwS3FQZSgI6wwS/x90O0uZtL\r\n"
"HEJ0dkja5jhKSRzKrPXti0av6wXb7T0Ksg67KvTETu9wFiz6c+gx3TYyX+k8UrFs\r\n"
"lnLIkQKBgQCLTxcJcf48uBQOBJ3yBZJWI9R/1ZQnaA4BcLj9VuC0iMK0URyV/uc4\r\n"
"hZ7F+/4URAuobYENLFJ6dagHW4JDFlUTdTT+CLqQLzEhglGMXiUIqz31o6QQft1j\r\n"
"lQrSLp9Di/IQhMs2r78NCbHbkopvqCjO186ufJHvE+e3eDefBQZ/BQ==\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

#define CMD_NOPOLL_MSG_EXIT		(0)
#define CMD_NOPOLL_MSG_OPEN		(1)
#define CMD_NOPOLL_MSG_SEND		(2)
#define CMD_NOPOLL_MSG_RECV		(3)
#define CMD_NOPOLL_MSG_CLOSE	(4)

struct cmd_nopoll_open_data {
	uint32_t tls;
	uint32_t verify;
	uint32_t cer;
};

struct cmd_nopoll_send_data {
	uint8_t *buf;
	uint32_t size;
};

struct cmd_nopoll_msg {
	uint32_t	type;
	void	   *data;
};

#define CMD_NOPOLL_QUEUE_WAIT_TIME (5000)
#define CMD_NOPOLL_THREAD_STACK_SIZE (4 * 1024)
static OS_Thread_t g_nopoll_server_thread;
static OS_Thread_t g_nopoll_client_thread;

static char g_server_host[32] = {0};
static char g_server_port[8] = {0};
static noPollCtx *g_server_ctx = NULL;
static noPollConn *g_listener = NULL;
static noPollConnOpts *g_listener_opts = NULL;

static OS_Queue_t g_client_queue;
static char g_client_host[32] = {0};
static char g_client_port[8] = {0};
static noPollCtx *g_client_ctx = NULL;
static noPollConn *g_conn = NULL;
static noPollConnOpts *g_conn_opts = NULL;

static void cmd_nopoll_server_task(void *arg)
{
	nopoll_loop_wait(g_server_ctx, 0);

	CMD_DBG("%s() end\n", __func__);
	OS_ThreadDelete(&g_nopoll_server_thread);
}

static void cmd_nopoll_server_on_close(noPollCtx *ctx, noPollConn *conn, noPollPtr user_data)
{
	CMD_LOG(1, "called connection close\n");
}

static nopoll_bool cmd_nopoll_server_on_open(noPollCtx *ctx, noPollConn *conn, noPollPtr user_data)
{
	CMD_LOG(1, "called connection open\n");

	nopoll_conn_set_on_close(conn, cmd_nopoll_server_on_close, NULL);

	if (!nopoll_conn_set_sock_block(nopoll_conn_socket(conn), nopoll_false)) {
		CMD_ERR("ERROR: failed to configure non-blocking state to connection..\n");
		return nopoll_false;
	}

	return nopoll_true;
}

static void cmd_nopoll_server_on_message(noPollCtx *ctx, noPollConn *conn, noPollMsg *msg, noPollPtr user_data)
{
	int size = nopoll_msg_get_payload_size(msg);
	const char *content = (const char *)nopoll_msg_get_payload(msg);

	CMD_LOG(1, "server receive message:\n");
	CMD_LOG(1, "%s\n", content);

	nopoll_conn_send_text(conn, content, size);

	if (nopoll_cmp(content, "nopoll test stop running")) {
		nopoll_loop_stop(g_server_ctx);
	}
}

static enum cmd_status cmd_nopoll_server_init_exec(char *cmd)
{
	int cnt;
	uint32_t dbg;

	cnt = cmd_sscanf(cmd, "d=%u", &dbg);
	if (cnt != 1) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (g_server_ctx != NULL) {
		CMD_ERR("already init\n");
		return CMD_STATUS_FAIL;
	}

	g_server_ctx = nopoll_ctx_new();
	if (g_server_ctx == NULL) {
		CMD_ERR("nopoll_ctx_new failed\n");
		return CMD_STATUS_FAIL;
	}

	if (dbg == 0)
		nopoll_log_enable(g_server_ctx, nopoll_false);
	else
		nopoll_log_enable(g_server_ctx, nopoll_true);

	/* set on open */
	nopoll_ctx_set_on_open(g_server_ctx, cmd_nopoll_server_on_open, NULL);

	/* set on message received */
	nopoll_ctx_set_on_msg(g_server_ctx, cmd_nopoll_server_on_message, NULL);

	OS_ThreadSetInvalid(&g_nopoll_server_thread);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_server_open_exec(char *cmd)
{
	int cnt;
	uint32_t tls;
	uint32_t verify;

	cnt = cmd_sscanf(cmd, "h=%s p=%s t=%u v=%u", g_server_host, g_server_port, &tls, &verify);
	if (cnt != 4) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (g_server_ctx == NULL) {
		CMD_ERR("without init\n");
		return CMD_STATUS_FAIL;
	}

	if ((g_listener != NULL) || (g_listener_opts != NULL)) {
		CMD_ERR("already open\n");
		return CMD_STATUS_FAIL;
	}

	g_listener_opts = nopoll_conn_opts_new();
	if (g_listener_opts == NULL) {
		CMD_ERR("nopoll_conn_opts_new failed\n");
		return CMD_STATUS_FAIL;
	}

	if (tls == 0)
		g_listener = nopoll_listener_new_opts(g_server_ctx, g_listener_opts, g_server_host, g_server_port);
	else {
		if (!nopoll_conn_opts_set_ssl_certs(g_listener_opts,
											g_server_pem, sizeof(g_server_pem),
											g_server_pem, sizeof(g_server_pem),
											NULL, 0,
											g_root_pem, sizeof(g_root_pem))) {
			CMD_ERR("nopoll_conn_opts_set_ssl_certs failed\n");
			nopoll_conn_opts_free(g_listener_opts);
			g_listener_opts = NULL;
			return CMD_STATUS_FAIL;
		}

		if (verify == 0)
			nopoll_conn_opts_ssl_peer_verify (g_listener_opts, nopoll_false);
		else
			nopoll_conn_opts_ssl_peer_verify (g_listener_opts, nopoll_true);

		g_listener = nopoll_listener_tls_new_opts(g_server_ctx, g_listener_opts, g_server_host, g_server_port);
	}

	if (g_listener == NULL) {
		CMD_ERR("nopoll_listener(_tls)_new_opts failed\n");
		nopoll_conn_opts_free(g_listener_opts);
		g_listener_opts = NULL;
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_server_start_exec(char *cmd)
{
	if ((g_listener == NULL) || (g_listener_opts == NULL)) {
		CMD_ERR("without open\n");
		return CMD_STATUS_FAIL;
	}

	if (OS_ThreadIsValid(&g_nopoll_server_thread)) {
		CMD_ERR("already start\n");
		return CMD_STATUS_FAIL;
	}

	if (OS_ThreadCreate(&g_nopoll_server_thread,
						"nopoll test server",
						cmd_nopoll_server_task,
						NULL,
						OS_THREAD_PRIO_CONSOLE,
						CMD_NOPOLL_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("thread create failed\n");
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_server_stop_exec(char *cmd)
{
	if (!OS_ThreadIsValid(&g_nopoll_server_thread)) {
		CMD_ERR("without start\n");
		return CMD_STATUS_FAIL;
	}

	nopoll_loop_stop(g_server_ctx);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_server_close_exec(char *cmd)
{
	if ((g_listener == NULL) || (g_listener_opts == NULL)) {
		CMD_ERR("without open\n");
		return CMD_STATUS_FAIL;
	}

	if (OS_ThreadIsValid(&g_nopoll_server_thread)) {
		CMD_ERR("without stop\n");
		return CMD_STATUS_FAIL;
	}

	nopoll_conn_close(g_listener);
	g_listener = NULL;
	g_listener_opts = NULL;

	cmd_memset(g_server_host, 0, sizeof(g_server_host));
	cmd_memset(g_server_port, 0, sizeof(g_server_port));

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_server_deinit_exec(char *cmd)
{
	if (g_server_ctx == NULL) {
		CMD_ERR("without init\n");
		return CMD_STATUS_FAIL;
	}

	nopoll_ctx_unref(g_server_ctx);
	nopoll_cleanup_library();
	g_server_ctx = NULL;

	return CMD_STATUS_OK;
}

static void cmd_nopoll_client_open_task(struct cmd_nopoll_open_data *data)
{
	g_conn_opts = nopoll_conn_opts_new();
	if (g_conn_opts == NULL) {
		CMD_ERR("nopoll_conn_opts_new failed\n");
		goto open_task_exit;
	}

	if (data->tls == 0)
		g_conn = nopoll_conn_new_opts(g_client_ctx, g_conn_opts, g_client_host, g_client_port, NULL, NULL, NULL, NULL);
	else {
		if (data->cer != 0) {
			if (!nopoll_conn_opts_set_ssl_certs(g_conn_opts,
												g_client_pem, sizeof(g_client_pem),
												g_client_pem, sizeof(g_client_pem),
												NULL, 0,
												g_root_pem, sizeof(g_root_pem))) {
				CMD_ERR("nopoll_conn_opts_set_ssl_certs failed\n");
				nopoll_conn_opts_free(g_conn_opts);
				g_conn_opts = NULL;
				goto open_task_exit;
			}
		}

		if (data->verify == 0)
			nopoll_conn_opts_ssl_peer_verify (g_conn_opts, nopoll_false);
		else
			nopoll_conn_opts_ssl_peer_verify (g_conn_opts, nopoll_true);

		g_conn = nopoll_conn_tls_new(g_client_ctx, g_conn_opts, g_client_host, g_client_port, NULL, NULL, NULL, NULL);
	}

	if (g_conn == NULL) {
		CMD_ERR("nopoll_conn(_tls)_new(_opts) failed\n");
		goto open_task_exit;
	}

	if (nopoll_conn_wait_until_connection_ready(g_conn, 10))
		CMD_LOG(1, "the connection is ready\n");
	else
		CMD_ERR("connection timeout\n");

open_task_exit:
	cmd_free(data);
	data = NULL;
	return;
}

static void cmd_nopoll_client_send_task(struct cmd_nopoll_send_data *data)
{
	int ret = nopoll_conn_send_text(g_conn, (char *)data->buf, data->size);
	if (ret != (int)data->size)
		CMD_ERR("size = %u, but nopoll_conn_send_text ret = %d\n", data->size, ret);

	cmd_free(data->buf);
	data->buf = NULL;
	cmd_free(data);
	data = NULL;
	return;
}

static void cmd_nopoll_client_recv_task(void *data)
{
	uint32_t msec = 10 * 1000;
	noPollMsg *msg;
	const char *content;

	while ((msg = nopoll_conn_get_msg(g_conn)) == NULL) {
		if (!nopoll_conn_is_ok(g_conn)) {
			CMD_ERR("received websocket connection close\n");
			return;
		}

		if (msec == 0) {
			CMD_ERR("get message timeout\n");
			return;
		}

		nopoll_sleep(100000);
		msec -= 100;
	}

	content = (const char *)nopoll_msg_get_payload(msg);
	CMD_LOG(1, "receive message:\n");
	CMD_LOG(1, "%s\n", content);

	nopoll_msg_unref (msg);
	return;
}

static void cmd_nopoll_client_close_task(void *data)
{
	nopoll_conn_close(g_conn);
	g_conn = NULL;
	g_conn_opts = NULL;

	cmd_memset(g_client_host, 0, sizeof(g_client_host));
	cmd_memset(g_client_port, 0, sizeof(g_client_port));

	return;
}

static void cmd_nopoll_client_task(void *arg)
{
	uint8_t task_exit = 0;
	struct cmd_nopoll_msg *msg;

	while (task_exit == 0) {

		if (OS_MsgQueueReceive(&g_client_queue, (void **)&msg, OS_WAIT_FOREVER) != OS_OK) {
			CMD_ERR("msg queue receive failed\n");
			break;
		}

		CMD_DBG("recv msg type %u\n", msg->type);

		switch (msg->type) {
		case CMD_NOPOLL_MSG_EXIT:
			task_exit = 1;
			break;
		case CMD_NOPOLL_MSG_OPEN:
			cmd_nopoll_client_open_task(msg->data);
			break;
		case CMD_NOPOLL_MSG_SEND:
			cmd_nopoll_client_send_task(msg->data);
			break;
		case CMD_NOPOLL_MSG_RECV:
			cmd_nopoll_client_recv_task(msg->data);
			break;
		case CMD_NOPOLL_MSG_CLOSE:
			cmd_nopoll_client_close_task(msg->data);
			break;
		default:
			CMD_WRN("unknown msg\n");
			break;
		}

		cmd_free(msg);
		msg = NULL;
	}

	CMD_DBG("%s() end\n", __func__);
	OS_ThreadDelete(&g_nopoll_client_thread);
}

static enum cmd_status cmd_nopoll_client_init_exec(char *cmd)
{
	int cnt;
	uint32_t dbg;

	cnt = cmd_sscanf(cmd, "d=%u", &dbg);
	if (cnt != 1) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (g_client_ctx != NULL) {
		CMD_ERR("already init\n");
		return CMD_STATUS_FAIL;
	}

	g_client_ctx = nopoll_ctx_new();
	if (g_client_ctx == NULL) {
		CMD_ERR("nopoll_ctx_new failed\n");
		return CMD_STATUS_FAIL;
	}

	if (dbg == 0)
		nopoll_log_enable(g_client_ctx, nopoll_false);
	else
		nopoll_log_enable(g_client_ctx, nopoll_true);

	OS_ThreadSetInvalid(&g_nopoll_client_thread);

	if (OS_MsgQueueCreate(&g_client_queue, 1) != OS_OK) {
		CMD_ERR("msg queue create failed\n");
		return CMD_STATUS_FAIL;
	}

	if (OS_ThreadCreate(&g_nopoll_client_thread,
						"nopoll test client",
						cmd_nopoll_client_task,
						NULL,
						OS_THREAD_PRIO_CONSOLE,
						CMD_NOPOLL_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("thread create failed\n");
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_client_open_exec(char *cmd)
{
	int cnt;
	uint32_t tls;
	uint32_t verify;
	uint32_t cer;
	struct cmd_nopoll_open_data *data;
	struct cmd_nopoll_msg *msg;

	cnt = cmd_sscanf(cmd, "h=%s p=%s t=%u v=%u c=%u", g_client_host, g_client_port, &tls, &verify, &cer);
	if (cnt != 5) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (g_client_ctx == NULL) {
		CMD_ERR("without init\n");
		return CMD_STATUS_FAIL;
	}

	if ((g_conn != NULL) || (g_conn_opts != NULL)) {
		CMD_ERR("already open\n");
		return CMD_STATUS_FAIL;
	}

	msg = cmd_malloc(sizeof(struct cmd_nopoll_msg));
	data = cmd_malloc(sizeof(struct cmd_nopoll_open_data));
	if ((msg == NULL) || (data == NULL)) {
		CMD_ERR("malloc failed\n");
		if (msg)
			cmd_free(msg);
		if (data)
			cmd_free(data);
		return CMD_STATUS_FAIL;
	}

	data->tls = tls;
	data->verify = verify;
	data->cer = cer;

	msg->type = CMD_NOPOLL_MSG_OPEN;
	msg->data = data;

	if (OS_MsgQueueSend(&g_client_queue, msg, CMD_NOPOLL_QUEUE_WAIT_TIME) != OS_OK) {
		CMD_ERR("msg queue send failed\n");
		if (msg)
			cmd_free(msg);
		if (data)
			cmd_free(data);
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_client_send_exec(char *cmd)
{
	int cnt;
	int ret;
	uint32_t size;
	uint8_t *buf;
	struct cmd_nopoll_send_data *data;
	struct cmd_nopoll_msg *msg;

	cnt = cmd_sscanf(cmd, "s=%u", &size);
	if (cnt != 1) {
		CMD_ERR("cmd_sscanf return: cnt = %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (size == 0) {
		CMD_ERR("size = 0\n");
		return CMD_STATUS_INVALID_ARG;
	}

	if ((g_conn == NULL) || (g_conn_opts == NULL)) {
		CMD_ERR("without open\n");
		return CMD_STATUS_FAIL;
	}

	msg = cmd_malloc(sizeof(struct cmd_nopoll_msg));
	data = cmd_malloc(sizeof(struct cmd_nopoll_send_data));
	buf = cmd_malloc(size);
	if ((msg == NULL) || (data == NULL) || (buf == NULL)) {
		CMD_ERR("malloc failed\n");
		if (msg)
			cmd_free(msg);
		if (data)
			cmd_free(data);
		if (buf)
			cmd_free(buf);
		return CMD_STATUS_FAIL;
	}

	cmd_write_respond(CMD_STATUS_OK, "OK");

	cmd_raw_mode_enable();
	ret = cmd_raw_mode_read(buf, size, 10000);
	if (ret != (int)size) {
		CMD_ERR("size = %u, but cmd_raw_mode_read ret = %d\n", size, ret);
		cmd_free(msg);
		cmd_free(data);
		cmd_free(buf);
		cmd_raw_mode_write((uint8_t *)"FAIL\n", 5);
		cmd_raw_mode_disable();
		return CMD_STATUS_ACKED;
	}
	cmd_raw_mode_disable();

	data->buf = buf;
	data->size = size;

	msg->type = CMD_NOPOLL_MSG_SEND;
	msg->data = data;

	if (OS_MsgQueueSend(&g_client_queue, msg, CMD_NOPOLL_QUEUE_WAIT_TIME) != OS_OK) {
		CMD_ERR("msg queue send failed\n");
		if (msg)
			cmd_free(msg);
		if (data)
			cmd_free(data);
		if (buf)
			cmd_free(buf);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_ACKED;
}

static enum cmd_status cmd_nopoll_client_recv_exec(char *cmd)
{
	struct cmd_nopoll_msg *msg;

	if ((g_conn == NULL) || (g_conn_opts == NULL)) {
		CMD_ERR("without open\n");
		return CMD_STATUS_FAIL;
	}

	msg = cmd_malloc(sizeof(struct cmd_nopoll_msg));
	if (msg == NULL) {
		CMD_ERR("msg queue send failed\n");
		return CMD_STATUS_FAIL;
	}

	msg->type = CMD_NOPOLL_MSG_RECV;
	msg->data = NULL;

	if (OS_MsgQueueSend(&g_client_queue, msg, CMD_NOPOLL_QUEUE_WAIT_TIME) != OS_OK) {
		CMD_ERR("msg queue send failed\n");
		if (msg)
			cmd_free(msg);
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_client_close_exec(char *cmd)
{
	struct cmd_nopoll_msg *msg;

	if ((g_conn == NULL) || (g_conn_opts == NULL)) {
		CMD_ERR("without open\n");
		return CMD_STATUS_FAIL;
	}

	msg = cmd_malloc(sizeof(struct cmd_nopoll_msg));
	if (msg == NULL) {
		CMD_ERR("msg queue send failed\n");
		return CMD_STATUS_FAIL;
	}

	msg->type = CMD_NOPOLL_MSG_CLOSE;
	msg->data = NULL;

	if (OS_MsgQueueSend(&g_client_queue, msg, CMD_NOPOLL_QUEUE_WAIT_TIME) != OS_OK) {
		CMD_ERR("msg queue send failed\n");
		if (msg)
			cmd_free(msg);
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_nopoll_client_deinit_exec(char *cmd)
{
	struct cmd_nopoll_msg *msg;

	if (g_client_ctx == NULL) {
		CMD_ERR("without init\n");
		return CMD_STATUS_FAIL;
	}

	nopoll_ctx_unref(g_client_ctx);
	g_client_ctx = NULL;

	msg = cmd_malloc(sizeof(struct cmd_nopoll_msg));
	if (msg == NULL) {
		CMD_ERR("msg queue send failed\n");
		return CMD_STATUS_FAIL;
	}

	msg->type = CMD_NOPOLL_MSG_EXIT;
	msg->data = NULL;

	if (OS_MsgQueueSend(&g_client_queue, msg, CMD_NOPOLL_QUEUE_WAIT_TIME) != OS_OK) {
		CMD_ERR("msg queue send failed\n");
		if (msg)
			cmd_free(msg);
		return CMD_STATUS_FAIL;
	}

	OS_MSleep(100);

	if (OS_MsgQueueDelete(&g_client_queue) != OS_OK) {
		CMD_ERR("msg queue delete failed\n");
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

static struct cmd_data g_nopoll_server_cmds[] = {
	{ "init",	cmd_nopoll_server_init_exec },
	{ "open",	cmd_nopoll_server_open_exec },
	{ "start",	cmd_nopoll_server_start_exec },
	{ "stop",	cmd_nopoll_server_stop_exec },
	{ "close",	cmd_nopoll_server_close_exec },
	{ "deinit",	cmd_nopoll_server_deinit_exec },
};

static struct cmd_data g_nopoll_client_cmds[] = {
	{ "init",	cmd_nopoll_client_init_exec },
	{ "open",	cmd_nopoll_client_open_exec },
	{ "send",	cmd_nopoll_client_send_exec },
	{ "recv",	cmd_nopoll_client_recv_exec },
	{ "close",	cmd_nopoll_client_close_exec },
	{ "deinit",	cmd_nopoll_client_deinit_exec },
};

static enum cmd_status cmd_nopoll_server_exec(char *cmd)
{
	return cmd_exec(cmd, g_nopoll_server_cmds, cmd_nitems(g_nopoll_server_cmds));
}

static enum cmd_status cmd_nopoll_client_exec(char *cmd)
{
	return cmd_exec(cmd, g_nopoll_client_cmds, cmd_nitems(g_nopoll_client_cmds));
}

static struct cmd_data g_nopoll_cmds[] = {
	{ "srv",	cmd_nopoll_server_exec },
	{ "cli",	cmd_nopoll_client_exec },
};

enum cmd_status cmd_nopoll_exec(char *cmd)
{
	return cmd_exec(cmd, g_nopoll_cmds, cmd_nitems(g_nopoll_cmds));
}

