function BMP280() {
    this.isInited = 0;
    this.handle = I2C.open('bmp280');
    this.lowTempReg = 0xfa;
    this.lowPressReg = 0xf7;
    this.dig_T1 = 0;
    this.dig_T2 = 0;
    this.dig_T3 = 0;
    this.dig_P1 = 0;
    this.dig_P2 = 0;
    this.dig_P3 = 0;
    this.dig_P4 = 0;
    this.dig_P5 = 0;
    this.dig_P6 = 0;
    this.dig_P7 = 0;
    this.dig_P8 = 0;
    this.dig_P9 = 0;
    this.t_fine = 0;

};


var bmp280 = new BMP280();

bmp280.read_one = function(addr){
    var reg=[0];
    reg[0] = addr;
    I2C.write(this.handle,reg);
    var val = I2C.read(this.handle,1); 
    return val;
};

bmp280.read_two = function(addr){

    var low = this.read_one(addr);
    var high = this.read_one(addr+1);
    var mem_val = low[0] << 8;
    mem_val = mem_val | high[0];
    return mem_val;
};

bmp280.read_three = function(addr){

    var msb_value = this.read_one(addr);
    var lsb_value = this.read_one(addr+1);
    var xlsb_value = this.read_one(addr+2);
    var val_high = msb_value[0] << 12;
    var val_mid = lsb_value[0] << 4;
    var val_low = xlsb_value[0] >> 4;
    var mem_val = val_high | val_mid | val_low;
    return mem_val;
};

bmp280.write_one = function(addr,val){
    var data = [0,0];
    data[0]=addr; 
    data[1]=val; 
    I2C.write(this.handle,data);
};

bmp280.init_config = function(){

    this.write_one(0xe0, 0xb6);
    this.write_one(0xf4, 0x27);
    this.write_one(0xf5, 0x20);
    this.dig_T1 = this.read_two(0x88);
    this.dig_T2 = this.read_two(0x8A);
    this.dig_T3 = this.read_two(0x8C);
    this.dig_P1 = this.read_two(0x8E);
    this.dig_P2 = this.read_two(0x90);
    this.dig_P3 = this.read_two(0x92);
    this.dig_P4 = this.read_two(0x94);
    this.dig_P5 = this.read_two(0x96);
    this.dig_P6 = this.read_two(0x98);
    this.dig_P7 = this.read_two(0x9A);
    this.dig_P8 = this.read_two(0x9C);
    this.dig_P9 = this.read_two(0x9E);
    
};


bmp280.getPressure = function(){

    if(0 == this.isInited){
        bmp280.init_config();
        this.isInited = 1;
    }
    var temp_value = this.read_three(this.lowTempReg);
    var press_value = this.read_three(this.lowPressReg);
/*
    var v_x1_u32r = (temp_value/16384-(this.dig_T1)/1024)*(this.dig_T2);
    var v_x2_u32r = ((temp_value/131072-this.ig_T1/8192)*(temp_value/131072-this.dig_T1/8192))*this.dig_T3;
    this.t_fine = v_x2_u32r+v_x1_u32r;
    v_x1_u32r = ((this.t_fine)>>1)-64000;
    v_x2_u32r = (((v_x1_u32r >> 2* (v_x1_u32r >> 2) >> 11)))* (this.dig_P6);
    v_x2_u32r = v_x2_u32r + ((v_x1_u32r *(this.dig_P5<< 1)));  
    v_x2_u32r = (v_x2_u32r >> 2)+ ((this.dig_P4)<< 16);
    v_x1_u32r = (((this.dig_P3* (((v_x1_u32r>> 2) * (v_x1_u32r>> 2)>> 13)))>> 3)+ (((this.dig_P2) * v_x1_u32r)>> 1))>> 18;  
    v_x1_u32r = ((((32768 + v_x1_u32r)* (this.dig_P1)>> 15)));
    var comp_baro = (((1048576 - press_value)- (v_x2_u32r >> 12)))* 3125;
    if(comp_baro < 0x80000000){
        comp_baro = (comp_baro<< 1)/ (v_x1_u32r);
    }else{
        comp_baro = (comp_baro / v_x1_u32r) * 2;
    }
    v_x1_u32r = ((this.dig_P9) * ((((comp_baro >> 3)* (comp_baro >> 3))>> 13)))>> 12;
    v_x2_u32r = ((comp_baro >2)* (this.dig_P8))>> 13;  
    comp_baro = 0-comp_baro + (((v_x1_u32r + v_x2_u32r+ this.dig_P7) >> 4));
*/
    return press_value;
};

module.exports.bmp280 = bmp280;


