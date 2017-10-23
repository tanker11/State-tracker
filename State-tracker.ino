/*
 * Kommenteket megírni
 * Timer beindítását tesztelni, főleg a nextState nélkülit, amikor visszaugrik az előzőhöz
 * És úgy is tesztelni, hogy marad az aktuális állapotban.
 * Ezek mind logikusan kövessék az állapotváltást és a prevState állapotai is frissüljenek.
 * Pl ha nem vált, akkor írja ki, hogy no state change, mert most az nem megy
 * 
 * 
 */


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
    int nextState;
    unsigned long timeWindow;
    unsigned long currentTime;
    bool timerRunning;
    void begin ( int reqState );
    void run (void);
    bool getTimerRunning();
    void forceState (int reqState);
    void setTimerState(unsigned long reqTime, int reqState, int reqNextState); //timer in ms, state to force, next state after timer
    void setTimerStateBack(unsigned long reqTime, int reqState); //timer in ms, state to force (next state is back to the previous
    bool timerDone ();
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
  timerRunning = false;
}

void StateTracker::run (void) {
  if (timerRunning && timerDone()) {
    Serial.println("TimerDone");
    setCurrentState(nextState);

  }
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

bool StateTracker::timerDone () {
  if (timerRunning) {
    unsigned long now = millis();

    if (currentTime <= now)
    {
      if ( (unsigned long)(now - currentTime )  < timeWindow )
        return false;
    }
    else
    {
      if ( (unsigned long)(currentTime - now) < timeWindow )
        return false;
    }
    setCurrentState(nextState);
    timerRunning = false;
    return true;
  }

}

bool StateTracker::getTimerRunning() {
  return timerRunning;
}

void StateTracker::setTimerState(unsigned long reqTime, int reqState, int reqNextState) {
  timeWindow = reqTime;
  currentTime = millis();
  setCurrentState(reqState);
  nextState = reqNextState;
  timerRunning = true;

}

void StateTracker::setTimerStateBack(unsigned long reqTime, int reqState) {

}

void StateTracker::action (int id) {
  switch (id) {
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

void StateTracker::goToPrevState() {
  Serial.print("GOTOPREV");
  int tempPrevState = getPrevState();

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
  stateABC.setTimerState(3000, stateABC.STATE_BC, stateABC.STATE_D);
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

  stateABC.run();

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


