#include "Button.h"
#include <wiring_private.h>

#include "constants.h"

Button::Button(int pin)
{
  m_pin = pin;
  m_currentState = !digitalRead(m_pin);
  m_previousState = m_currentState; // Force new State
  m_timeChanged = millis();
}

bool
Button::newState()
{
  bool ret = false;
  
  if (millis() - m_timeChanged >= ANTIREBOND_MS)
  {
    
    int curr = getCurrentState();
    int readIn = digitalRead(m_pin);
    
    if (curr != readIn)
    {
      m_previousState = curr;
      m_currentState = readIn;
      m_timeChanged = millis();
      ret = true;
    }
  }
  
  return ret;
}

bool
Button::getRawState(){
	return digitalRead(m_pin);
}

