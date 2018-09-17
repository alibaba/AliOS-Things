##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

import socket
import wx
from wx.lib import plot as wxplot
import threading
import json
import time

class wifista:
    def __init__(self, mac):
        self.mac = mac
        self.rssis = {}
        self.max_rssi = -500
        self.max_ap = ""
        self.last_updated = time.time()

    def _update_max(self):
        self.max_rssi = -1000
        self.max_ap = ""
        for (mac,rssi) in self.rssis.items():
            if rssi['rssi'] < self.max_rssi:
                continue
            self.max_rssi = rssi['rssi']
            self.max_ap = mac

    def update_rssi(self, ap_mac, rssi):
        self.rssis[ap_mac] = { "rssi":rssi, "last_updated":time.time()}
        self._update_max()            
        self.last_updated = time.time()

    def get_max_ap(self):
        return self.max_ap

    def expired(self, now):
        should_update = False
        for (mac,rssi) in self.rssis.items():
            delta = now - rssi['last_updated']
            if delta < 30.0: #30 seconds
                continue
            print "sta %s to ap %s expired"%(self.mac, mac)
            del self.rssis[mac]
            should_update = True
        if should_update:
            self._update_max()            
        return len(self.rssis) <= 0

class wifiap:
    def __init__(self, mac):
        self.mac = mac
        self.stas = {}

    def add_sta(self, sta_mac):
        self.stas[sta_mac] = sta_mac

    def del_sta(self, sta_mac):
        if sta_mac in self.stas:
            del self.stas[sta_mac]

    def get_sta_num(self):
        return len(self.stas)

class wifimgr(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self, name='wifimgr')
        self.aps = {}
        self.stas = {}

    def report_rssi_obj(self, robj):
        if 'rssi' in robj:
            rssi = robj['rssi']
        else:
            return
        if 'ap' in robj:
            ap_mac = robj['ap']
        else:
            return
        if 'sta' in robj:
            sta_mac = robj['sta']
        else:
            return
        self.report_rssi(sta_mac, ap_mac, rssi)

    def sta_rssi_max_changed(self, sta_mac, old_max_mac, new_max_mac):
        if old_max_mac == new_max_mac:
            return;
        if old_max_mac in self.aps:
            ap = self.aps[old_max_mac]
            ap.del_sta(sta_mac)
        if new_max_mac in self.aps:
            ap = self.aps[new_max_mac]
            ap.add_sta(sta_mac)

    def house_keeping(self):
        now = time.time()
        for (sta_mac,sta) in self.stas.items():
            old_max_mac = sta.get_max_ap()
            expired = sta.expired(now)
            new_max_mac = sta.get_max_ap()
            self.sta_rssi_max_changed(sta_mac, old_max_mac, new_max_mac)
            if expired:
                del self.stas[sta_mac]

        self.show_map()

    def run(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.bind(('', 12345))
        while True:
            s.settimeout(5.0)
            try:
                rbuf = s.recvfrom(128)[0]
            except:
                self.house_keeping()
                continue
            robj = json.loads(rbuf)
            print "recv: ", robj
            self.report_rssi_obj(robj)
            self.house_keeping()

    def add_ap(self, mac):
        if mac in self.aps:
            return
        ap = wifiap(mac)
        self.aps[mac] = ap
        return ap

    def report_rssi(self, sta_mac, ap_mac, rssi):
        if sta_mac in self.stas:
            sta = self.stas[sta_mac]
        else:
            sta = wifista(sta_mac)
            self.stas[sta_mac] = sta

        self.add_ap(ap_mac)

        old_max_mac = sta.get_max_ap()
        sta.update_rssi(ap_mac, rssi)
        new_max_mac = sta.get_max_ap()
        self.sta_rssi_max_changed(sta_mac, old_max_mac, new_max_mac)

    def show_map(self):
        return
        print "---------show map--------------"
        for ap_mac in self.aps:
            ap = self.aps[ap_mac]
            print "ap %s:%d"%(ap.mac, ap.get_sta_num())

wmgr = wifimgr();
wmgr.start()

class Frame(wx.Frame):
    def __init__(self, title):
        wx.Frame.__init__(self, None, wx.ID_ANY, title=title, size=(800,600))
        panel = wx.Panel(self, wx.ID_ANY)
        self.panel = panel

        topSizer = wx.BoxSizer(wx.VERTICAL)
        panel.SetSizer(topSizer)

        canvas = [ wxplot.PlotCanvas(panel, size=(800, 100)) for i in range(0,4) ]
        self.canvas = canvas

        y_datas = [ [] for i in range(0,4) ]
        self.y_datas = y_datas

        for idx in range(0,4):
            sizer = wx.BoxSizer(wx.HORIZONTAL)
            m_text = wx.StaticText(panel, wx.ID_ANY, "ap%d"%idx)
            sizer.Add(m_text, 0, wx.ALL, 5)
            sizer.Add(canvas[idx], 0, wx.ALL | wx.EXPAND)
            topSizer.Add(sizer, wx.CENTER)

#        topSizer.Fit(self)

        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer)
        self.timer.Start(1000)

    def draw_one(self, idx, ap):
        canvas = self.canvas[idx]
        print 'canvas=',canvas

        # Generate some Data
        x_data = [ i for i in range(1,100) ]
        y_data = self.y_datas[idx]
        y_data.append(ap.get_sta_num())
        if len(y_data) > 100:
            del y_data[0]

        # most items require data as a list of (x, y) pairs:
        #    [[1x, y1], [x2, y2], [x3, y3], ..., [xn, yn]]
        xy_data = list(zip(x_data, y_data))

        # Create your Poly object(s).
        # Use keyword args to set display properties.
        line = wxplot.PolySpline(
            xy_data,
            colour=wx.Colour(128, 128, 0),   # Color: olive
            width=3,
        )

        # create your graphics object
        graphics = wxplot.PlotGraphics([line])
        # draw the graphics object on the canvas
        canvas.Draw(graphics)

    def OnTimer(self, event):
        idx = 0
        for (ap_mac, ap) in wmgr.aps.items():
            self.draw_one(idx, ap)
            idx += 1
        

app = wx.App(redirect=False)
top = Frame("WiFi People Monitor")
top.Show()
app.MainLoop()
