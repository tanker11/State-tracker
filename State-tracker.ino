#include <Automaton.h>

#include "Atm_stateABC.h"

#include <MD_KeySwitch.h>
const uint8_t SWITCH_PIN = 0;       // switch connected to this pin
const uint8_t SWITCH_ACTIVE = LOW;  // digital signal when switch is pressed 'on'

MD_KeySwitch S(SWITCH_PIN, SWITCH_ACTIVE);
// Basic Arduino sketch - instantiates the state machine and nothing else

#define WIFI_LED_PIN 2

void engemhivj() {

}

//-----------------------------------------------------------------------------------------

class StateTracker {

  public:
    enum { STATE_A, STATE_B, STATE_AC, STATE_BC, STATE_D }; // STATES
    //enum { EVT_SHORT_PUSH, EVT_LONG_PUSH, ELSE }; // EVENTS
    int machineState;
    int prevState;
    void begin ( int reqState );
    void forceState (int reqState);
    void setCurrentState (int reqState);
    void setPrevState ( int reqPrevState );
    int getPrevState ();
    int getCurrentState ();
    void goToPrevState ();


  private:
    enum { ENT_STATE_A, ENT_STATE_B, ENT_STATE_AC, ENT_STATE_BC, ENT_STATE_D}; // ACTIONS
    //   int event( int id );
       void action( int id );
    
};

void StateTracker::begin (int reqState) {
  setCurrentState(reqState);
  setPrevState(reqState);
}

void StateTracker::forceState (int reqState) {
  if (reqState != machineState) {
    Serial.printf("Exiting %d state\n", machineState);
    setPrevState(machineState);
    setCurrentState(reqState);
    action (reqState);
  }
  else {
    Serial.println("No state change");
  }
}

void StateTracker::action (int id){
switch (id){
  case ENT_STATE_A:
      Serial.println("ENT STATE-A");
//         flipper.attach(0.1, flip);

  return;

   case ENT_STATE_B:
      Serial.println("ENT STATE-B");
//         flipper.attach(0.1, flip);

  return;
}
  
}

void StateTracker::setCurrentState(int reqState) {
  machineState = reqState;
  Serial.printf("Entering %d state\n", machineState);
}

void StateTracker:: setPrevState (int reqPrevState) {
  prevState = reqPrevState;
}

int StateTracker::getPrevState() {
  return prevState;
}

int StateTracker::getCurrentState() {
  return machineState;
}

void StateTracker::goToPrevState(){
  Serial.print("GOTOPREV");
  int tempPrevState=getPrevState();
  
  setPrevState(machineState);
  forceState(tempPrevState);
  
}


//-----------------------------------------------------------------------------------------

StateTracker stateABC;

unsigned long currentTime;

boolean isShortPress() {
  return true;
}

boolean isTimeout(unsigned long checkTime, unsigned long timeWindow)
{
  unsigned long now = millis();
  if (checkTime <= now)
  {
    if ( (unsigned long)(now - checkTime )  < timeWindow )
      return false;
  }
  else
  {
    if ( (unsigned long)(checkTime - now) < timeWindow )
      return false;
  }

  return true;
}

void setup() {

  Serial.begin( 115200 );
  pinMode(WIFI_LED_PIN, OUTPUT);

  currentTime = millis();
  stateABC.begin(stateABC.STATE_A);
  //KeySwitch init
  S.begin();
  S.enableDoublePress(true);
  S.enableLongPress(true);
  S.enableRepeat(false);
  S.enableRepeatResult(false);
  S.setLongPressTime(1000);
  S.setDoublePressTime(500);

  //flipper.attach(0.1, flip);
}

void loop() {

  automaton.run();
  //Serial.print(".");
  if (isTimeout(currentTime, 2000)) {
    currentTime = millis();
    Serial.println(stateABC.getPrevState());
        Serial.println(stateABC.getCurrentState());
    if (stateABC.getCurrentState() == stateABC.STATE_B)  Serial.println("BBBB!!!!");
  }

  //Handle keypresses
  switch (S.read())
  {
    case MD_KeySwitch::KS_NULL:       /* Serial.println("NULL"); */   break;
    case MD_KeySwitch::KS_PRESS:      Serial.print("\nSINGLE PRESS-"); stateABC.goToPrevState(); break;
    case MD_KeySwitch::KS_DPRESS:     Serial.print("\nDOUBLE PRESS"); break;
    case MD_KeySwitch::KS_LONGPRESS:  Serial.print("\nLONG PRESS-");   stateABC.forceState(stateABC.STATE_B); break;
    case MD_KeySwitch::KS_RPTPRESS:   Serial.print("\nREPEAT PRESS"); break;
    default:                          Serial.print("\nUNKNOWN");      break;
  }

}


