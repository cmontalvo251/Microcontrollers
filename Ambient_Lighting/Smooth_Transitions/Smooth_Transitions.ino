//Arduino Code: (transfer this program to the Arduino)
//Developed by Carlos Montalvo
float red=255, green=255, blue=255; //red, green and blue values
float red_old=255, green_old=255, blue_old=255; //red, green and blue values
float red_cur,green_cur,blue_cur;
int RedPin = 9;
int GreenPin = 11;
int BluePin = 10;
int NEWCOLOR = 1;

void setup()
{

}

void loop()
{
  ///Create New Random Color
  if (NEWCOLOR)
  {
    red_old = red;
    green_old = green;
    blue_old = blue;
    red = random(0,255);
    green = random(0,255);
    blue = random(0,255);
    //red = 0;
    //blue = 255;
    //green = 0;
  
    //Normalize by a certain brightness
    float total = red + green + blue;
    
    //Normalize everything by total to get a sum of brightness_
    red = 255*red/total;
    blue = 255*blue/total;
    green = 255*green/total;
    
    //Check for overflow */
    float p = max(red,blue);
    float m = max(p,green);
    
    if (m > 255)
    { 
      red = 255*red/m;
      blue = 255*blue/m;
      green = 255*green/m;
    }
    NEWCOLOR = 0;
  }
  else
  {
    //Fade in Color
    red_old = red_old*0.9 + red*0.1;
    blue_old = blue_old*0.9 + blue*0.1;
    green_old = green_old*0.9 + green*0.1;
    if (abs(red_old-red) < 2)
    {
      if (abs(blue_old-blue) < 2)
      {
        if (abs(green_old-green) < 2)
        {
          delay(2000);
          NEWCOLOR = 1;
        }
      }
    }
  }
  
  red_cur = red_old;
  blue_cur = blue_old;
  green_cur = green_old;
    
  analogWrite (RedPin, red_cur);
  analogWrite (GreenPin, green_cur);
  analogWrite (BluePin, blue_cur);

  delay(100); 
  
}

