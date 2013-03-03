#include <Shieldbot.h>

#define BLACK LOW
#define WHITE HIGH

#define LEFT 1;
#define RIGHT 0;

enum State_type {
  Initialise,
  Advance,
  Advance_Turn_left,
  Advance_Turn_right,
  Rotate_to_time,
  backup
};

Shieldbot shieldbot = Shieldbot();

int S1,S2,S3,S4,S5;	//values to store state of sensors
int pre_S1,pre_S2,pre_S3,pre_S4,pre_S5;
int inc_minutes;
int counter;
int tick;

int last_side;


State_type State = Initialise;

void setup(){
  Serial.begin(9600);
  shieldbot.setMaxSpeed(255);//255 is max

  Serial.print("Shield bot says send me a command");
  Serial.println();
  Serial.print("r = right, l = left, f = fwd, b = back");
  Serial.println();
  tick = 0;
  counter = 0;
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

  // get time
  
  
  counter++;
  if (counter == 60)
  {
    inc_minutes = 1;
    counter = 0;
  }
  //Print the status of each sensor to the Serial console
  /*Serial.print("S5: ");
  Serial.print(S5);
  Serial.print(" S4: ");
  Serial.print(S4);
  Serial.print(" S3: ");
  Serial.print(S3);
  Serial.print(" S2: ");
  Serial.print(S2);
  Serial.print(" S1: ");
  Serial.print(S1);  */

      //drive fwd with staying centered
  switch (State) 
  {
    case Initialise:
      //rotate until hit center then fwd until 12
        State = Advance;
      break;
    case Advance:          //drive fwd with staying centered

           tick = 0;
      if (is_centered() == true)
      {
        if (last_side == 0)
        {
          if ((S5 == BLACK) & (pre_S5 == WHITE))
          {
            State = Rotate_to_time;
            last_side = 1;
          }
          else
      	    goforward(); 
          
        }
        else  // left
        {
        if (S1 == BLACK & pre_S1 == WHITE)
        {
          State = Rotate_to_time;
          last_side = 0;
        }
        else
      	goforward(); 
          
        }
        

      }
      else
      {    
      	    if (S2 == WHITE)
                State = Advance_Turn_left;
            else if (S4 == WHITE)
                State = Advance_Turn_right;
       }  
      break;
      
     case Advance_Turn_left:
                  goleft();          
                  if(is_centered()== true)
                  State = Advance;
 
     break;
 
      case Advance_Turn_right:
                  goright(); 
                  if(is_centered()== true)
                  State = Advance;
     break;
 
     case Rotate_to_time:
     
     Serial.print("rotatae to time state");   
      
       
      if (inc_minutes == 1)
      {
        if((S2 == BLACK & pre_S2 == WHITE))

           // found next mark
           inc_minutes = 0;
           tick++;

        Serial.print("tick: ");
        Serial.print(tick);
       
           if(is_centered() & tick > 10)
           {
              State = Advance; 
           }
       else 
        goright_big();       
      }
         
      break;    
    
    default: 
      break;
      // if nothing else matches, do the default
      // default is optional
  }



  delay(50);

  botstop();

  Serial.println();  
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
  Serial.print("Right turn");
}

void goright_big(){
  shieldbot.drive(20,-20);
  Serial.print("Right turn");
}

void goleft(){
  shieldbot.drive(0,32);
  Serial.print("Left turn");  
}

void goforward()
{
  shieldbot.drive(32,32);
  Serial.print("go fwd");
}

void botstop()
{
  shieldbot.drive(0,0);  
}