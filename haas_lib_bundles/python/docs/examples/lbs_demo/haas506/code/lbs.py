# -*- coding: UTF-8 -*-
import net
import http
import ujson

class LBS():
    def __init__(self):
        self.requesturl = ''
        self.url = 'http://apilocate.amap.com/position'
        self.accesstype = 0
        #input your key here
        self.key = 'yourKey'
        self.cdma = 0
        self.output = 'json'
        self.bts = ''
        self.nearbts = ''
        self.htp = http.client()
        pass

    def get_lbs(self):
        nbrinfo = net.getCellInfo()
        srvinfo = net.getState()
        #gsm
        if len(srvinfo[0]) > 0:
            pass
        if len(srvinfo[1]) > 0:
            pass
        #lte
        if len(srvinfo[2]) > 0:
            srv = srvinfo[2]
            self.bts = str(hex(srv[2]))[2:] + ',' + str(srv[3]) +','+ str(srv[6])+ ',' + str(srv[1]) + ',' + str(srv[7])
            pass
        if len(nbrinfo[0]) > 0:
            pass
        if len(nbrinfo[1]) > 0:
            pass
        if len(nbrinfo[2]) > 0:
            nbr = nbrinfo[2]
            print(nbr,nbrinfo,len(nbr))
            for i in range(len(nbr)):
                self.nearbts += str(hex(nbr[i][2]))[2:] + ',' + str(nbr[i][3]) + ',' + str(nbr[i][6]) + ',' + str(nbr[i][1]) + ',' + str(nbr[i][7]) 
                if i != (len(nbr) - 1):
                    self.nearbts += '|'
            pass
        
        self.requesturl += self.url + '?' + 'accesstype=' + str(self.accesstype) + '&key=' + self.key + '&cdma=' + str(self.cdma) + '&output=' + self.output + '&bts=' + self.bts + '&nearbts=' + self.nearbts

        print(self.requesturl)

        ret = self.htp.get(self.requesturl)
        if ret < 0:
            return None
        retstr = self.htp.get_response()
        self.htp.close()
        locationstr = None
        lat = None
        lon = None
        try:
            retdict = ujson.loads(retstr)
            try:
                if int(retdict['infocode']) != 10000:
                    return None
                try:
                    locationstr = retdict['result']['location']
                    tmpstr = locationstr.split(',')
                    lon = tmpstr[0]
                    lat = tmpstr[1]
                    print('content:',retdict)
                except KeyError:
                    return None
                finally:
                    return lon,lat
            except KeyError:
                return None
            finally:
                return lon,lat
        except ValueError:
            return None
        finally:
            return lon,lat

'''
响应：
content: {'result': {'citycode': '0512', 'type': '4', 'city': '苏州市', 'poi': '中国工商银行(苏州独墅湖支行)',
 'desc': '江苏省 苏州市 虎丘区 仁爱路 靠近中国工商银行(苏州独墅湖支行)', 'adcode': '320505', 
 'street': '仁爱路', 'radius': '550', 'location': '120.7394987,31.2744203', 
'road': '仁爱路', 'country': '中国', 'province': '江苏省'}, 'status': '1', 'infocode': '10000', 'info': 'OK'}
'''
