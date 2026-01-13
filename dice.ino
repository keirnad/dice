#include <Wire.h>               
#include "HT_SSD1306Wire.h"

static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst

//This is a multidimensional array for reading and writing point coordinates in 2D
int verticies[8][2];

int buttonState, buttonPrev;
int randNumber;
String rng;

//These variables will be used to calculate the rotation of the cube.
float cosX, sinX, cosY, sinY;
float RotMatX0, RotMatX1, RotMatX2, RotMatY0, RotMatY1, RotMatY2;

//These are just the starting points of cube.
int origin_verts[8][3] = {
{-1,-1,1},
{1,-1,1},
{1,1,1},
{-1,1,1},
{-1,-1,-1},
{1,-1,-1},
{1,1,-1},
{-1,1,-1}
};

//The rotated points of the cube are recorded here
float rotated_3d_verts[8][3]; 

//Angle, Z-axis offset, and size parameters
float angle_deg = 0;
float z_offset = -4.0;
float cube_size = 80.0;

void setup() {
  Serial.begin(115200);

  //initializing the generator
  randomSeed(analogRead(4));
  pinMode(0, INPUT_PULLUP);
  buttonPrev = digitalRead(0);
  VextON();
  delay(100);

  display.init();

  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 20, "DICE");
  display.display();

}

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

void loop() {
  //Number generation
  randNumber = random(1, 6);
  rng = String(randNumber);

  buttonState = digitalRead(0);
  
  //Here, I'm reading a single button press.
  if (buttonState == 0 && buttonPrev == 1) {
    
    for(int i = 0; i < 40; i++){
      display.clear();
      cube_size = 90 + sin(i * 0.3)*30; //Smooth change in size value (In this case, I specified 90 and 30, which means that the value will change from 90 - 30 to 90 + 30) 

      //INCREASE ANGLE
      if (angle_deg < 90 - 5) {
        angle_deg = angle_deg + 10;
      }
      else {
        angle_deg = 0;
      }
    cosX = cos(radians(angle_deg));
    sinX = sin(radians(angle_deg));

    cosY = cos(radians(angle_deg));
    sinY = sin(radians(angle_deg));

    float matrixX[3][3] = {
      {1.0, 0, 0},
      {0, cosX, -sinX},
      {0, sinX, cosX}
    };

    float matrixY[3][3] = {
      {cosY, 0, sinY},
      {0, 1.0, 0},
      {-sinY, 0, cosY}
    };

    //this is a combined rotation matrix (2 axis rotation in the same time)
    float finalRotation[3][3] = {
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          finalRotation[i][j] = 0;
          for (int k = 0; k < 3; k++) {
            finalRotation[i][j] = finalRotation[i][j] + (matrixY[i][k] * matrixX[k][j]);
          }
        }
      } 

    for (int j = 0; j < 8; j++) {

      RotMatY0 = finalRotation[0][0] * origin_verts[j][0] + finalRotation[0][1] * origin_verts[j][1] + finalRotation[0][2] * origin_verts[j][2];
      RotMatY1 = finalRotation[1][0] * origin_verts[j][0] + finalRotation[1][1] * origin_verts[j][1] + finalRotation[1][2] * origin_verts[j][2];
      RotMatY2 = finalRotation[2][0] * origin_verts[j][0] + finalRotation[2][1] * origin_verts[j][1] + finalRotation[2][2] * origin_verts[j][2] +  z_offset;

      rotated_3d_verts [j][0] = RotMatY0;
      //Serial.println(rotated_3d_verts[j][0]);
      rotated_3d_verts [j][1] = RotMatY1;
      //Serial.println(rotated_3d_verts[j][1]);
      rotated_3d_verts [j][2] = RotMatY2;
      //Serial.println(rotated_3d_verts[j][2]);

      verticies [j][0] = round(64 + rotated_3d_verts [j][0] / rotated_3d_verts [j][2] * cube_size);
      verticies [j][1] = round(32 + rotated_3d_verts [j][1] / rotated_3d_verts [j][2] * cube_size);
    }

  
      display.drawLine(verticies[0][0], verticies[0][1], verticies[1][0], verticies[1][1]);
      display.drawLine(verticies[1][0], verticies[1][1], verticies[2][0], verticies[2][1]);
      display.drawLine(verticies[2][0], verticies[2][1], verticies[3][0], verticies[3][1]);
      display.drawLine(verticies[3][0], verticies[3][1], verticies[0][0], verticies[0][1]);

      display.drawLine(verticies[4][0], verticies[4][1], verticies[5][0], verticies[5][1]);
      display.drawLine(verticies[5][0], verticies[5][1], verticies[6][0], verticies[6][1]);
      display.drawLine(verticies[6][0], verticies[6][1], verticies[7][0], verticies[7][1]);
      display.drawLine(verticies[7][0], verticies[7][1], verticies[4][0], verticies[4][1]);
  
      display.drawLine(verticies[0][0], verticies[0][1], verticies[4][0], verticies[4][1]);
      display.drawLine(verticies[1][0], verticies[1][1], verticies[5][0], verticies[5][1]);
      display.drawLine(verticies[2][0], verticies[2][1], verticies[6][0], verticies[6][1]);
      display.drawLine(verticies[3][0], verticies[3][1], verticies[7][0], verticies[7][1]);
      display.display();
      delay(10);
    }
    display.drawString(64, 20, rng);
    display.display();
  }
  buttonPrev = digitalRead(0);
}
