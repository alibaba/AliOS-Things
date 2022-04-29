ITER_NUM = 50
ACCELERATION_LOW_THREADHOLD = 4  # 加速度变化下限阈值，越大越灵敏
ACCELERATION_UP_THREADHOLD = 12  # 加速度变化上限阈值，越小越灵敏
ANGULAR_VELOCITY_LOW_THREADHOLD = 1  # 角速度变化下限阈值，越小越灵敏
ANGULAR_VELOCITY_UP_THREADHOLD = 40  # 角速度变化下限阈值，越大越灵敏


class fall_detection:
    def __init__(self, getData):
        self.ax_offset = 0.0
        self.ay_offset = 0.0
        self.az_offset = 0.0
        self.gx_offset = 0.0
        self.gy_offset = 0.0
        self.gz_offset = 0.0
        self.trigger1count = 0
        self.trigger2count = 0
        self.trigger1 = False
        self.trigger2 = False
        self.getData = getData
        self.calibrate()

    def calibrate(self):
        ax_sum = 0.0
        ay_sum = 0.0
        az_sum = 0.0
        gx_sum = 0.0
        gy_sum = 0.0
        gz_sum = 0.0
        for num in range(0, ITER_NUM):
            data = self.getData()        # 读取传感器信息值
            ax_sum += data[0][0]
            ay_sum += data[0][1]
            az_sum += data[0][2]
            gx_sum += data[1][0]
            gy_sum += data[1][1]
            gz_sum += data[1][2]
        self.ax_offset = ax_sum/ITER_NUM
        self.ay_offset = ay_sum/ITER_NUM
        self.az_offset = az_sum/ITER_NUM
        self.gx_offset = gx_sum/ITER_NUM
        self.gy_offset = gy_sum/ITER_NUM
        self.gz_offset = gz_sum/ITER_NUM
        print('Now you can start fall detection!')

    def detect(self):
        global ACCELERATION_LOW_THREADHOLD
        global ACCELERATION_UP_THREADHOLD
        global ANGULAR_VELOCITY_LOW_THREADHOLD
        global ANGULAR_VELOCITY_UP_THREADHOLD
        fall = False
        data = self.getData()                          # 读取传感器信息值
        AcX = data[0][0]
        AcY = data[0][1]
        AcZ = data[0][2]
        GyX = data[1][0]
        GyY = data[1][1]
        GyZ = data[1][2]
        ax = (AcX-self.ax_offset)
        ay = (AcY-self.ay_offset)
        az = (AcZ-self.az_offset)
        gx = (GyX-self.gx_offset)
        gy = (GyY-self.gy_offset)
        gz = (GyZ-self.gz_offset)
        if hasattr(self, 'sensitivity'):
            if 'ACCELERATION_LOW_THREADHOLD' in self.sensitivity:
                ACCELERATION_LOW_THREADHOLD = float(self.sensitivity['ACCELERATION_LOW_THREADHOLD'])
            if 'ACCELERATION_UP_THREADHOLD' in self.sensitivity:
                ACCELERATION_UP_THREADHOLD = float(self.sensitivity['ACCELERATION_UP_THREADHOLD'])
            if 'ANGULAR_VELOCITY_LOW_THREADHOLD' in self.sensitivity:
                ANGULAR_VELOCITY_LOW_THREADHOLD = float(self.sensitivity['ANGULAR_VELOCITY_LOW_THREADHOLD'])
            if 'ANGULAR_VELOCITY_UP_THREADHOLD' in self.sensitivity:
                ANGULAR_VELOCITY_UP_THREADHOLD = float(self.sensitivity['ANGULAR_VELOCITY_UP_THREADHOLD'])
        # calculating accelerationChangelitute vector for 3 axis
        Raw_accelerationChange = pow(pow(ax, 2)+pow(ay, 2)+pow(az, 2), 0.5)
        # Multiplied by 10 to values are between 0 to 1
        accelerationChange = Raw_accelerationChange * 10
        # Trigger1 function
        # if accelerationChange breaks lower threshold
        if(accelerationChange <= ACCELERATION_LOW_THREADHOLD and self.trigger2 == False):
            self.trigger1 = True
            # print("TRIGGER 1 ACTIVATED")
        if (self.trigger1 == True):
            self.trigger1count = self.trigger1count + 1
            # if accelerationChange breaks upper threshold
            if (accelerationChange >= ACCELERATION_UP_THREADHOLD):
                self.trigger2 = True
                # print("TRIGGER 2 ACTIVATED")
                self.trigger1 = False
                self.trigger1count = 0
        if (self.trigger2 == True):
            self.trigger2count = self.trigger2count+1
            angleChange = pow(pow(gx, 2)+pow(gy, 2)+pow(gz, 2), 0.5)
            # Trigger2 function
            # //if orientation changes by between 1-40 degrees
            if (angleChange >= ANGULAR_VELOCITY_LOW_THREADHOLD and angleChange <= ANGULAR_VELOCITY_UP_THREADHOLD):
                self.trigger2 = False
                self.trigger2count = 0
                fall = True
                return fall
        if (fall == True):  # //in event of a fall detection
            fall = False
        if (self.trigger2count >= 50):  # //allow time for orientation change
            self.trigger2 = False
            self.trigger2count = 0
            # print("TRIGGER 2 DECACTIVATED")
        if (self.trigger1count >= 5):  # //allow time for accelerationChange to break upper threshold
            self.trigger1 = False
            self.trigger1count = 0
            # print("TRIGGER 1 DECACTIVATED")
        return fall
