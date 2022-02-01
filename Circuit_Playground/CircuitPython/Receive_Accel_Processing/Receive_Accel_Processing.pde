////////////GLOBAL VARIABLES/////////////
int numVars = 3;
float[] received_data = new float[numVars];
float roll;
float pitch;

////////VIEWPORT VARIABLES//////////////////
PFont font;

///////////SERIAL VARIABLS
//In order to talk to the Arduino
import processing.serial.*;
Serial myport;
char[]inLine = new char[60];

///////////////DATALOGGING VARIABLES//////////
PrintWriter file;

////FLAGS
void setup() 
{  
  //SETUP VIEW WINDOM
  size(400,400, P3D); //set view size with 3D processing
  surface.setResizable(true);

  //BACKGROUND
  // I put the background in setup not draw this causes corners to be cut off on the cube
  background(#000000);
 
  ////CREATE A FONT
  font = createFont("Courier", 18); //built in processing command
 
  try {
    myport = new Serial(this,"/dev/ttyACM0",115200);
  } catch (RuntimeException e) {
    print("Error Opening Serial Port");
  }

  //Initialize all receive vars to zero
  for (int i = 0;i<numVars;i++){
    received_data[i] = 0;
  }
  
  println("Ready");
}

//Alright here is our draw loop
void draw() {
  
  //Set the view port color
  background(#000000); // I kept this here if we want to revert the background to original 
  //fill(#FFFFFF);
    
  ////Check for Response EveryLoop
  //H:nnnnnnn\r<mutiplenumbers>\n
  if (myport.available()>0) {
    //print("Something is in the pipe\n");
    SerialGetNumbers();
  }
  
  //Convert to G's
  float G = 9.81;
  float ax = received_data[0]/G;
  float ay = received_data[1]/G;
  float az = received_data[2]/G;
  
  //Normalize
  float anorm = sqrt(ax*ax + ay*ay + az*az);
  ax /= anorm;
  ay /= anorm;
  az /= anorm;
  
  //Compute Roll and pitch
  roll = atan2(ay,az);
  pitch = atan2(ax,az);
  println(roll*180/PI,pitch*180/PI);
  
  drawCube();
  
  //GRID 2,1 - PTP
  //stroke(255);
  //textFont(font, 8*((width+height)/2.0)/200.0);  //set the textfont to Courier and size 20
  //textAlign(LEFT, TOP); //set the test to left and top
  //for (int i = 0;i<numVars;i++) {
  //  text(received_data[i] + "\n",width/3.0,height*i/numVars);
  //}
}

/////ROUTINE TO ROTATE A BOX ON THE SCREEN
void drawCube() {  
  pushMatrix();  //sets the view port window - these are openGL commands
  translate(width/2.0, height/2.0, 0); //translate the cube
  float c = (1.0/5.0)*(1.0/9.0)*0.8;
  //x is to the right and y is down
  scale(width*c,height*c,(width+height)/2.0*c); //scale the cube
  // a demonstration of the following is at 
  // http://www.varesano.net/blog/fabio/ahrs-sensor-fusion-orientation-filter-3d-graphical-rotating-cube
  rotateZ(pitch); //phi 
  rotateX(roll); //theta
  rotateY(0); //psi in that order for standard Aerospace sequences
  buildBoxShape(); //use QUADS to draw the faces in different colors
  popMatrix(); //restore the orientation window 
}

/////ROUTINE TO DRAW A BOX
void buildBoxShape() {
  noStroke();
  beginShape(QUADS); //again openGL commands to draw a cube

  //Z+ (to the drawing area)
  fill(#00ff00); //Cube is in different colors
  vertex(-5, -5, 5);
  vertex(5, -5, 5);
  vertex(5, 5, 5);
  vertex(-5, 5, 5);

  //Z-
  fill(#0000ff); 
  vertex(-5, -5, -5);
  vertex(5, -5, -5);
  vertex(5, 5, -5);
  vertex(-5, 5, -5);

  //X-
  fill(#ff0000);
  vertex(-5, -5, -5);
  vertex(-5, -5, 5);
  vertex(-5, 5, 5);
  vertex(-5, 5, -5);

  //X+
  fill(#ffff00);
  vertex(5, -5, -5);
  vertex(5, -5, 5);
  vertex(5, 5, 5);
  vertex(5, 5, -5);

  //Y-
  fill(#ff00ff);
  vertex(-5, -5, -5);
  vertex(5, -5, -5);
  vertex(5, -5, 5);
  vertex(-5, -5, 5);

  //Y+
  fill(#00ffff);
  vertex(-5, 5, -5);
  vertex(5, 5, -5);
  vertex(5, 5, 5);
  vertex(-5, 5, 5);

  endShape(); //done with shape and quit
}
 

void SerialGetNumbers() {
  String inBuffer = myport.readString(); 
  String[] list = new String[3];
  list = split(inBuffer,' ');
  try {
    received_data[0] = float(list[0]);
    received_data[1] = float(list[1]);
    received_data[2] = float(list[2]);
  } catch (RuntimeException e) {
    print("Error Opening Serial Port");
  }
  //print(list);
}
