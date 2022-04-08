import utime   # 延时函数在utime

ITER_NUM = 100


class tap_detection:
    def __init__(self, mpu6886Dev):
        self.mpu6886Dev = mpu6886Dev
        self.ax_offset = 0.0
        self.ay_offset = 0.0
        self.az_offset = 0.0
        self.triggercount = 0
        self.untriggercount = 0
        self.tapcount = 0

    def calibrate(self):
        ax_sum = 0.0
        ay_sum = 0.0
        az_sum = 0.0
        for num in range(0, ITER_NUM):
            data = self.get_mpu6886_data()        # 读取传感器信息值
            ax_sum += data[0][0]
            ay_sum += data[0][1]
            az_sum += data[0][2]
        self.ax_offset = ax_sum/ITER_NUM
        self.ay_offset = ay_sum/ITER_NUM
        self.az_offset = az_sum/ITER_NUM

        self.accelerator_low_threshold = 0.0
        for num in range(0, ITER_NUM):
            data = self.get_mpu6886_data()        # 读取传感器信息值
            ax = data[0][0]
            ay = data[0][1]
            az = data[0][2]
            ax = (ax-self.ax_offset)
            ay = (ay-self.ay_offset)
            az = (az-self.az_offset)
            Raw_accelerationChange = pow(pow(ax, 2)+pow(ay, 2)+pow(az, 2), 0.5)
            # Multiplied by 10 to values are between 0 to 1
            self.accelerator_low_threshold += Raw_accelerationChange * 10
        self.accelerator_low_threshold = (
            self.accelerator_low_threshold / ITER_NUM)*2

        print('Now you can start tap detection!')

    def enable_tap_detection(
        self,
        tap_detect_count: int = 2,
        accelerator_up_threshold: float = 30,
        latency: int = 150,
    ):
        self.tap_detect_count = tap_detect_count
        self.accelerator_up_threshold = accelerator_up_threshold
        self.latency = latency

    # 通过MPU6886传感器读取加速度、角速度信息
    def get_mpu6886_data(self):
        acc = self.mpu6886Dev.acceleration
        gyro = self.mpu6886Dev.gyro
        # print(acc)
        # print(gyro)
        return acc, gyro                    # 返回读取到的加速度、角速度值

    def clear_triggercount(self):
        self.triggercount = 0

    def detect_tap(self):
        fall = False
        data = self.get_mpu6886_data()                          # 读取传感器信息值
        AcX = data[0][0]
        AcY = data[0][1]
        AcZ = data[0][2]
        ax = (AcX-self.ax_offset)
        ay = (AcY-self.ay_offset)
        az = (AcZ-self.az_offset)
        # calculating accelerationChangelitute vector for 3 axis
        Raw_accelerationChange = pow(pow(ax, 2)+pow(ay, 2)+pow(az, 2), 0.5)
        # Multiplied by 10 to values are between 0 to 1
        accelerationChange = Raw_accelerationChange * 10
        if (fall == True):
            fall = False
        # Trigger function
        # if accelerationChange breaks lower threshold
        if(accelerationChange >= self.accelerator_low_threshold and accelerationChange <= self.accelerator_up_threshold):
            self.triggercount = self.triggercount+1
            if (self.triggercount % 3 == 0):  # catch one tap
                self.tapcount = self.tapcount+1
                utime.sleep_ms(100)
            if (self.tapcount == self.tap_detect_count):  # catched tap_detect_count
                self.triggercount = 0
                self.tapcount = 0
                fall = True
        else:
            self.untriggercount = self.untriggercount+1
            if (self.untriggercount == 150):
                self.triggercount = 0
                self.tapcount = 0
                self.untriggercount = 0
        return fall
