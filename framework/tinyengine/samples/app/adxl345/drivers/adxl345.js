
function ADXL345() {
    this.isInited = 0;
    this.handle = I2C.open('adxl345');
    this.xLowReg = 0x32;
    this.yLowReg = 0x34;
    this.zLowReg = 0x36;
};

var adxl345 = new ADXL345();

adxl345.read_one = function(addr){
    var reg=[0];
    reg[0] = addr;
    I2C.write(this.handle,reg);
    var val = I2C.read(this.handle,1); 
    return val;
};

adxl345.read_two = function(addr){

    var low = this.read_one(addr);
    var high = this.read_one(addr+1);
    var mem_val = high[0] << 8;
    mem_val = mem_val | low[0];
    return mem_val;
};

adxl345.write_one = function(addr,val){
    var data = [0,0];
    data[0]=addr; 
    data[1]=val; 
    I2C.write(this.handle,data);
};

adxl345.init_config = function(){
    this.write_one(0x2D,0x08);
    this.write_one(0x31,0x0B);
    this.write_one(0x2C,0x09);
};


adxl345.getAcc = function(){

    var acc = [0,0,0];
    if(0 == this.isInited){
        this.init_config();
        this.isInited = 1;   
    }
    var gx = this.read_two(this.xLowReg);
    var gy = this.read_two(this.yLowReg);
    var gz = this.read_two(this.zLowReg);

    acc[0] = gx;
    acc[1] = gy;
    acc[2] = gz;

    return acc;
};

module.exports.adxl345 = adxl345;