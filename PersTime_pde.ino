#include <Time.h>
#include <Wire.h>
#include <MCP79412RTC.h>
#include <Shieldbot.h>

#define BLACK LOW
#define WHITE HIGH

#define LEFT 1
#define RIGHT 0

#define DEBUG 1;

enum State_type {
  Advance,
  Wait_Advance,
  Advance_Turn_right,
  Rotate_to_time,
  Wait_Rotate,
  backup
};

Shieldbot shieldbot = Shieldbot();

int S1,S2,S3,S4,S5;	//values to store state of sensors
int pre_S1,pre_S2,pre_S3,pre_S4,pre_S5;
int inc_minutes;
int counter;

int Secs, Mins, Hrs;
int last_side;

int hour_pos, min_pos;


time_t myTime;
int pre_secs;




State_type State = Advance;

void setup(){

//  int Secs, Mins, Hrs;

  Serial.begin(9600);
  
  shieldbot.setMaxSpeed(255);//255 is max

//  readCommand();

  myTime = now(); //RTC.get()

  Secs = second(myTime);
  Mins = 24; ////minute(myTime);
  Hrs = 12; //////////////////hour(myTime);

  last_side = LEFT;

  hour_pos = 0;
  min_pos = 0;
  counter = 0;
  
  State = Advance;
  
  pre_secs = Secs;
}

void loop(){
    
  pre_S1 = S1;
  pre_S2 = S2;
  pre_S3 = S3;
  pre_S4 = S4;
  pre_S5 = S5;

  S1 = shieldbot.readS1();
  S2 = shieldbot.readS2();
  S3 = shieldbot.readS3();
  S4 = shieldbot.readS4();
  S5 = shieldbot.readS5();

  pre_secs = Secs;

  // get time
    myTime = now();
  //Mins = minute(myTime);  
  Secs = second(myTime);
//  Hrs = hour(myTime);


  if (Secs != pre_secs)
  {

      Serial.print(Hrs, DEC);
      Serial.print(":");  
      Serial.print(Mins, DEC);
      Serial.print(":");   
      Serial.print(Secs, DEC);
      Serial.println(); 
    
    
/*    Serial.print(pre_secs, DEC);
    Serial.print(Secs, DEC);
    Serial.println();    
  */  
    counter++;
    if (counter>10)
    {
      Mins++;
  //     Serial.print(Secs, DEC);
  //    Serial.println();  
      if (Mins == 49)
        Mins = 0;
        
      counter = 0;
    }
  }


      //drive fwd with staying centered
  switch (State) 
  {    
    case Advance:          //drive fwd while staying centered

      if (is_centered() == true) // if aligned to go forward
      {
        // look for hour marker on correct side
        if (last_side == RIGHT)  // if last saw hour marker on the right so look on left
        {
          if ((S5 == BLACK) & (pre_S5 == WHITE))  // look for transition on left
          {
            last_side = LEFT;
           
            hour_pos++;   
            if(hour_pos < Hrs)
              State = Wait_Advance;  
            else
              State = Rotate_to_time;       
          }
          else // if cannot see hour marker yet then go forward
      	    goforward(); 
          
        }
        else  // last saw hour marker on left so look on right
        {
          if (S1 == BLACK & pre_S1 == WHITE)  // look for transition on right
          {
            last_side = RIGHT;            
           
            hour_pos++; 
            if(hour_pos < Hrs)
              State = Wait_Advance;
            else
              State = Rotate_to_time;
          }
          else  // if cannot see hour marker yet then go forward
      	    goforward(); 
        }
      }
      else  // if not aligned to go forward then realign
      {
         if (S2 == WHITE)  // if off black to the right
           goleft();                
         else if (S4 == WHITE)  // if off black on the left
           goright();                 
       }  
      break;
 
     case Wait_Advance:
       delay(500);
       State = Advance;
      break; 
 
     case Rotate_to_time:    // once lined up with hours mark rotate to the minutes
        
      if (min_pos < Mins)  // if time to advance minutes
      {
        if(S2 != pre_S2)  // rotate  until see  transition
        {
           // found next mark
           inc_minutes = 0;      // clear flag that need to update minutes
           min_pos++;            // increment internal count of minutes position
           State = Wait_Rotate;
       
           if(is_centered() & (min_pos > 12))  // if rotated to 0 minutes advance hours
           {
             Hrs++;                              // advance the hours
             if (Hrs > 12)                       // if hours rolls-over from 12 to 1
             {
               Hrs = 1;                          // reset the hours to match roll-over
               State = backup;                  // change state so returning to 1
             }
             else
               State = Advance;                 // change state so advancing

              min_pos = 0;                     // set internal count of minutes position to 0     
           }
        }
       else                                 // if don't yet see transition
        goright_big();                      // then continue rotating
      }
         
      break;    

     case Wait_Rotate:
       delay(1000);
       State = Rotate_to_time;
      break;

     case backup:

      if (is_centered() == true) // if aligned to go forward
      {
        // look for hour marker on correct side
        if (last_side == RIGHT)  // if last saw hour marker on the right so look on left
        {
          if ((S5 == BLACK) & (pre_S5 == WHITE))  // look for transition on left
          {
            last_side = LEFT;
           
            hour_pos--;   
            if(hour_pos == Hrs)
              State = Rotate_to_time;       
          }
          else // if cannot see hour marker yet then go forward
      	    goback(); 
          
        }
        else  // last saw hour marker on left so look on right
        {
          if (S1 == BLACK & pre_S1 == WHITE)  // look for transition on right
          {
            last_side = RIGHT;            
           
            hour_pos--; 
            if(hour_pos == Hrs)
              State = Rotate_to_time;
          }
          else  // if cannot see hour marker yet then go forward
      	    goback(); 
        }
      }
      else  // if not aligned to go forward then realign
      {
         if (S2 == WHITE)  // if off black to the right
           goleft();                
         else if (S4 == WHITE)  // if off black on the left
           goright();                 
       }  



      break;

    
    default: 
      break;
      // if nothing else matches, do the default
      // default is optional
  }



  delay(40); //50);

  botstop();

 // Serial.println();  
}


boolean is_centered()
{

    if (S3 == BLACK & S4 == BLACK & S2 == BLACK)
     return true;
  else
     return false;
  
}



void goright(){
  shieldbot.drive(32,0);
//  Serial.print("Right turn");
}

void goright_big(){
  shieldbot.drive(18,-18);
//  Serial.print("Right turn");
}

void goleft(){
  shieldbot.drive(0,32);
//  Serial.print("Left turn");  
}

void goforward()
{
  shieldbot.drive(32,32);
//  Serial.print("go fwd");
}

void goback()
{
  shieldbot.drive(-32,-32);
//  Serial.print("go ba");
}

void botstop()
{
  shieldbot.drive(0,0);  
}



//Read command from the Arduino serial monitor to set the RTC.
//Case-sensitive and must be entered exactly as (24-hour clock):
//  Set yyyy-mm-dd hh:mm:ss
void readCommand(void)
{
    int i;
    char inChar;
    Serial.println("You have 20 seconds to set the time");

    Serial.println("format hh:mm:ss");
    
    tmElements_t tmSet;
    time_t tSet;

  // wait 20 sec for command to set time
  for (i=0;i<10;i++)
  {
   delay(1000);
 
  while (Serial.available()>0) {
 
      tmSet.Hour = Serial.parseInt();
      tmSet.Minute = Serial.parseInt();
      tmSet.Second = Serial.parseInt();
      
      Serial.print(tmSet.Hour, DEC); 
      Serial.print(tmSet.Minute, DEC);
      Serial.print(tmSet.Second, DEC); 
      Serial.println();
      
      
      if (Serial.read() == '\n') {    
      tSet = makeTime(tmSet);		 //convert to time_t
      setTime(tSet);			 //set the system time
      //    RTC.set(now());		         //set the rtc
      Serial.println("RTC set!");
      }
                   
  }
 
  }  
}

