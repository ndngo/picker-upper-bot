#pragma config(Sensor, in2,    potentio,       sensorPotentiometer)

//A global useful because the holding can be done in its own thread
int isHolding = 0;


task adjustArm(){
  while(1){
    if(isHolding == 1 && SensorValue(petentio) < armThreshold){
      motor[armMotor] = 20;
    }
  }
}

//make sure to set isHolding = 1 in the pickUp function
isHolding = 1;

//and make sure to set it to 0 in the put down function **before** actually putting it down
isHolding = 0;

//then call this in the main task

startTask(adjustArm);
