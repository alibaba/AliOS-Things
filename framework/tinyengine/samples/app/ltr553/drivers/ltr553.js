
function LTR553() {
    this.isInited = 0;
    this.handle = I2C.open('ltr553');
    this.status_reg = 0x8c;
    this.alsChan0_reg = 0x8a;
    this.alsChan1_reg = 0x88;
    this.low_ps_reg = 0x8d;
 
};

var ltr553 = new LTR553();

ltr553.read_one = function(addr){
    var reg=[0];
    reg[0] = addr;
    I2C.write(this.handle,reg);
    var val = I2C.read(this.handle,1); 
    return val;
};

ltr553.read_two = function(addr){

    var low = this.read_one(addr);
    var high = this.read_one(addr+1);
    var mem_val = high[0] << 8;
    mem_val = mem_val | low[0];
    return mem_val;
};

ltr553.write_one = function(addr,val){
    var data = [0,0];
    data[0]=addr; 
    data[1]=val; 
    I2C.write(this.handle,data);
};

ltr553.init_config = function(){

    this.write_one(0x82,0x1F);
    this.write_one(0x84,0x00);
    this.write_one(0x85,0x12);
    this.write_one(0x97,1000&0xFF);  
    this.write_one(0x98,1000>>8);  
    this.write_one(0x99,1000&0xFF);  
    this.write_one(0x9a,1000>>8);  
    this.write_one(0x90,1000&0xFF);  
    this.write_one(0x91,1000>>8);  
    this.write_one(0x92,0);  
    this.write_one(0x93,0);  
    this.write_one(0x80,0x01); 
    this.write_one(0x81,0x03); 
};

 
ltr553.getPsAls = function(){

    var val = [-1,-1];
    var i = 0;
    var status = 0;
    if(0 == this.isInited){
        this.init_config(); 
        this.isInited = 1;   
    }
    for(i=0;i<10;i++){
        status = this.read_one(this.status_reg);  
        console.log('status:'+status[0]);
        if(0x05 == (status[0] & 0x05)){
            break;
        }  
    }
    if(0x05 != (status[0] & 0x05)){
        return val;
    }
    var chan1 = this.read_two(this.alsChan1_reg);
    var chan0 = this.read_two(this.alsChan0_reg);
    var ps = this.read_two(this.low_ps_reg);
    val[0] = (chan1 + chan0)/2;
    val[1] = ps;
    return val;

};

module.exports.ltr553 = ltr553;




