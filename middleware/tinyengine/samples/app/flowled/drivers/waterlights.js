function WaterLight() {
    this.handle1 = GPIO.open('led1');
    this.handle2 = GPIO.open('led2');
    this.handle3 = GPIO.open('led3');
    this.handle4 = GPIO.open('led4');
    this.handle5 = GPIO.open('led5');
};
var waterlight = new WaterLight();

waterlight.set = function(handle,status){

    var ledhandle = -1;
    console.log('handle:'+handle);
    if(1 == handle) {
        ledhandle = this.handle1;
    }else if(2 == handle){
        ledhandle = this.handle2;    
    }else if(3 == handle){
        ledhandle = this.handle3;   
    }else if(4 == handle){
        ledhandle = this.handle4;      
    }else{
        ledhandle = this.handle5; 
    }
    GPIO.write(ledhandle, status);

};

module.exports.waterlight = waterlight;