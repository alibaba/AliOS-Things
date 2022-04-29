from driver import GPIO

class TTP224:
    def __init__(self,gpio1Dev=None,gpio2Dev=None,gpio3Dev=None,gpio4Dev=None):
        self._gpio1Obj = None
        self._gpio2Obj = None
        self._gpio3Obj = None
        self._gpio4Obj = None
        self._cntObj = 0
        if isinstance(gpio1Dev, GPIO):
            self._cntObj = self._cntObj + 1
            self._gpio1Obj = gpio1Dev

        if isinstance(gpio2Dev, GPIO):
            self._cntObj = self._cntObj + 1
            self._gpio2Obj = gpio2Dev

        if isinstance(gpio3Dev, GPIO):
            self._cntObj = self._cntObj + 1
            self._gpio3Obj = gpio3Dev

        if isinstance(gpio4Dev, GPIO):
            self._cntObj = self._cntObj + 1
            self._gpio4Obj = gpio4Dev

        if self._cntObj == 0:
            raise ValueError("no GPIO object")

    def getStatus(self):
        if self._cntObj == 0:
            raise ValueError("no GPIO object")

        if self._gpio1Obj is None:
            value01 = None
        else:
            value01 = self._gpio1Obj.read()

        if self._gpio2Obj is None:
            value02 = None
        else:
            value02 = self._gpio2Obj.read()

        if self._gpio3Obj is None:
            value03 = None
        else:
            value03 = self._gpio3Obj.read()

        if self._gpio4Obj is None:
            value04 = None
        else:
            value04 = self._gpio4Obj.read()

        return self._cntObj,value01,value02,value03,value04
