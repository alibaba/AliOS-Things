
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var st7735 = function() {

  this.handle = LCD.open({height:128,width:128});
  this.fill =  function(sx,sy,ex,ey,color,isFill) {

    var lcd_fill = {};
    lcd_fill.color = color;
    lcd_fill.sx = sx;
    lcd_fill.sy = sy;
    lcd_fill.ex = ex;
    lcd_fill.ey = ey;
    lcd_fill.fill = isFill;
    LCD.fill(lcd_fill);
  };

  this.show =  function(sx,sy,str,isNewLine,color,size) {

      var lcd_show = {};
      lcd_show.color = color;
      lcd_show.x = sx;
      lcd_show.y = sy;
      lcd_show.newline = isNewLine;
      lcd_show.str = str;
      lcd_show.size = size;
      LCD.show(lcd_show);
  };

  this.rgb565 = function(r,g,b) {
    var rvalue = r >> 3;
    rvalue = rvalue << 11;
    var gvalue = g >> 2;
    gvalue = gvalue << 5;
    var bvalue = b >> 3;
    return (rvalue + gvalue + bvalue);
  };
}; 

module.exports = st7735;