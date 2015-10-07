/*Test 1.2 Date 30/07/2015
  
  Test for converting camera path values into commands for stepper motors.
  Web camera will sends values via the serial comm. line on the USB cable to the processor.
  These values will be reinterpreted as x and y values for the stepper motors.
  Once commands are complete, the controller will hand control back to the PC to send more commands. 

*/


#include <Stepper.h>

#define BUFFER_SIZE 500

typedef struct Point
{
  int x;
  int y;
} Point;
    
int path_size = 0;
String inData;
Point path[BUFFER_SIZE];
int j = 0;


const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor




// initialize the stepper library on pins 8 through 11:
Stepper stepperX(stepsPerRevolution, 8, 9, 10, 11);

// initialize the stepper library on pins 4 through 7:
Stepper stepperY(stepsPerRevolution, 4, 5, 6, 7);

int switchX1 = 22;
boolean buttonStateX1 = true;   
int switchX2 = 23;
boolean buttonStateX2 = true; 
int switchY1 = 24;
boolean buttonStateY1= true; 
int switchY2 = 25;
boolean buttonStateY2 = true; 

void setup() {
  // put your setup code here, to run once:

  stepperY.setSpeed(60);
  stepperX.setSpeed(60);
  
  pinMode(switchX1, INPUT_PULLUP);
  pinMode(switchX2, INPUT_PULLUP);  
  pinMode(switchY1, INPUT_PULLUP);
  pinMode(switchY2, INPUT_PULLUP);
  
  Serial.begin(115200);

  path[j].x = 0;
  j++;
  path[j].y = 0;
  j++;  
//  home();
}

void setMotorLow() /* turns off signal pins to stepper motors to avoid overheating H-bridges */
{
  int i = 4;
  for (i = 4; i < 12; i++)
  {
    digitalWrite (i, LOW);
//    Serial.println("set pin");
//    Serial.println(i);
//    Serial.println("low");
  };
};

void loop() {
 // put your main code here, to run repeatedly:
    while (Serial.available() > 0)
    {
        char received = Serial.read();
        inData += received;     
        

        // Process message when new line character is recieved
        if (received == '\n')
        {
            Serial.print("Arduino Received: ");
            Serial.print(inData);

             if(j % 2 == 0){
              path[j/2].x = inData.toInt();
             }
             else {
               path[j/2].y = inData.toInt();
               path_size++;
             }
            j++;
            inData = ""; // Clear recieved buffer
        }
        else if (received == 'a') // end byte for path command
        {
          int k = 1;
          int steps = 0;
            for(k = 1; k < path_size; k++)
            {
              line(path[k].x - path[k-1].x, path[k].y - path[k-1].y);
            }
          }
    }
}

void line(int xSteps, int ySteps)
{
  Serial.println("Moving " + String(xSteps) + " steps in X and " + String(ySteps) + " steps in Y.");
  int xProgress = 0,
  yProgress = 0,
  xDifference = 0,
  yDifference = 0;

  int i;
  
  for (i = 0; i < 10; i++)
  {
    xDifference = (xSteps * i / 10) - xProgress;
    yDifference = (ySteps * i / 10) - yProgress;

    stepperX.step(xDifference);
    delay(3);
    stepperY.step(yDifference);
    delay(3);
    setMotorLow();

    xProgress += xDifference;
    yProgress += yDifference;
  }
}

void home()
{
  Serial.println("Homing...");
  
  while(digitalRead(switchX1) == HIGH)
  {
    stepperX.step(-1);
    delay(20);
  }
  setMotorLow();
  delay(100);
  Serial.println("X is 0.");  
  path[j].x = 0;
  j++;
  Serial.println("Waiting for release");  
  while (digitalRead(switchX1) == LOW)
  {

    Serial.println("Waiting...");
    Serial.print(digitalRead(switchX1));
    /*if(digitalRead(switchX1)) {
      break;
    }*/
  }
  
  while(digitalRead(switchX1) == HIGH)
  {
    stepperY.step(-1);
    delay(20);
  }
  setMotorLow();
  delay(100);
  Serial.println("Y is 0.");
  path[j].y = 0;
  j++;
    while(digitalRead(switchX1) == LOW)
  {
    delay(20);
  }
  
  Serial.println("Done homing.");
}
