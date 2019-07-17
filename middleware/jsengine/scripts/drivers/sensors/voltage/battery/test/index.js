var voltage = new battery('battery'); 
voltage.config(3.3, 12);

setInterval(function () {
    var  value = voltage.read_voltage(); 
    console.log('voltage:' + value);  
  }, 2000); 

