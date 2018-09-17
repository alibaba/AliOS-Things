
function GY30() {
    this.isInited = 0;
    this.handle = I2C.open('gy30');
};

var gy30 = new GY30();

gy30.read_two = function(){
    var val = I2C.read(this.handle,2); 
    var result = val[0] << 8;
    result = result | val[1];
    return result;
};

gy30.write_one = function(val){
    var data = [0];
    data[0]=val; 
    I2C.write(this.handle,data);
};

gy30.init_config = function(){
    this.write_one(0x01);
    this.write_one(0x10);
};


gy30.get_light = function(){

    if(0 == this.isInited){
        this.init_config();
        this.isInited = 1;   
    }
    var val = this.read_two();

    return val;
};

module.exports.gy30 = gy30;