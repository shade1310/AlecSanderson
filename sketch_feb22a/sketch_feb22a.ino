#include <msp430.h>
//global variables
#define sizex 8
#define sizey 8
#define threshval 80
#define numcycle 3

volatile int threshold = 0;
volatile int threshcount = 0;

// the setup routine runs once when you press reset:
void setup() {
// initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
   
  
  //setting up input pins
  P2DIR |= BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
  P1DIR |= BIT6;
  
  //setting up output pin
  P1DIR &= ~BIT7;

 
  //initializing threshold value
}

// the loop routine runs over and over again forever:
void loop() {
  //for loop variables
  int i;
  int j;
    //set to the max possible value of analogRead()
   // threshold = 400;
  //value read in from the IR LEDs
  int sensorValue;
  //2D array to store IR LED values
  int frame[sizex][sizey];
  //2D array to store IR LED values as 'high' or 'low'
  int frame2[sizex][sizey];
  //these store the information in frame2 to be sent to the RGB matrix
  long spiValue[4];
  //setThreshold count and boolean values    
  

  
  //sending a 1 to the shift register
  P1OUT |= BIT6;
  //shift register loop
  for (int i = 0; i < sizex; i++)
  {
    //Shift register stores a value
    P2OUT |= BIT7;
    delay(2);
    //shift register shifts in the value stored from BIT7
    //it also shifts all other values to the left
    P2OUT |= BIT6;
    delay(2);
    //sending the shift register a 0 for next time
    P1OUT &= ~BIT6;
    //setting the 'clock' signals low for next time
    P2OUT &= ~BIT6 + ~BIT7;
    //Serial.print(threshcount);
    //multiplexer loop
    for (int j = 0; j < sizey;j++)
    {
      delay(2);
      //Sets A, B, and C to low
      P2OUT &= ~BIT3 + ~BIT4 + ~BIT5;
      if ((j % 2) == 1)
      {
        //sets A to high if the current multiplexer input
        //is an odd number
        P2OUT |= BIT3;
      }
      if ((j % 4) > 1)
      {
        //sets B to high if the current input is 2,3,6 or 7
        P2OUT |= BIT4;
      }
      if (j > 3)
      {
        //Sets C to high if the current input is higher than 3
        P2OUT |= BIT5;
      }
      
      //reads in 7 samples from the reciever
      delay(1);
      sensorValue = analogRead(A7);
      delay(1);
      sensorValue += analogRead(A7);
      delay(1);
      sensorValue += analogRead(A7);
      
      //averages the samples and stores them in the array
      frame[i][j] = sensorValue/3;
      delay(1);
    
    }
  }
  //sets threshhold boolean to low
  for (i = 0; i <sizex; i++)
  {
    for(j = 0; j <sizey; j++)
    {
     if (frame[i][j] < threshold && frame[i][j] > 2)
     {
         frame2[i][j] = 1;
     }
     else
     {
        //low value detected
        frame2[i][j] = 0;
     }
     //shifts through storing values in the long int
    
  }
  }
  for (i = 0; i <4; i++)
  {
    for (j = 0;j < 4; j++)
      {
        if(frame2[2*i][2*j] == 1 || frame2[2*i + 1][2*j + 1] == 1)
        {
        frame2[2*i][2*j] = 1;
        frame2[2*i + 1][2*j] = 1;
        frame2[2*i][2*j + 1] = 1;
        frame2[2*i + 1][2*j + 1] = 1;
        }
      }
    }
    for ( i = 0; i < sizex; i++)
      for (j = 0; j <sizey; j++)
      {
        if (i < 2)
        {
          spiValue[0] = frame2[i][j];
          spiValue[0] = spiValue[0] >> 1;
        }
        else if(i < 4)
        {
          spiValue[1] = frame2[i][j];
          spiValue[1] = spiValue[1] >> 1;
        }
        else if(i < 6)
        {
          spiValue[2] = frame2[i][j];
          spiValue[2] = spiValue[2] >> 1;
        }
        else
        {
          spiValue[3] = frame2[i][j];
          spiValue[3] = spiValue[3] >> 1;
        }
      }

 //if a high value was detected resets the count

  //no high values were detected, increments the count
    threshcount++;
   
   //if twenty cycles go by with no high values, calls setThreshold
  if (threshcount == numcycle)
  {
    setThreshold(frame);
    threshcount = 0;
  }
  
  //Prints the output to serial
  for (i = 0; i <sizex; i++)
  {
    for(j = 0; j <sizey; j++)
    {
      //Serial.print(frame[i][j]);
      //Serial.print("\t");
      Serial.print(frame2[i][j]);
      Serial.print("\t");
       
      
    }
    Serial.println(" ");
  }
  
 Serial.println(" "); 

//Prints the output to serial in the form of 'high'/'low'


}


//setsa new threshold value for each led based on the current value
void setThreshold(int frame[][sizey])
{
  int i;
  int j;
  int value = 0;
  int count = 0;
  for (i = 0; i <sizex; i++)
  {
    for(j = 0; j <sizey; j++)
    {
      if (frame[i][j] > threshold && frame[i][j] > 2)
      {
      value += frame[i][j];
      count++;
      }
    }
  }
  threshold = value/count - threshval;
}
//void resetThreshold()
//{
//  int i;
 // int j;
 // for (i = 0; i <sizex; i++)
 // {
 //   for(j = 0; j <sizey; j++)
 //   {
   //   threshold[i][j] = 1025;
    //}
 // }
//}
