// Classe de gestion des boutons
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include "Arduino.h"

class Button
{
  public:
    Button(int pin);
    
    bool newState();
    bool getPreviousState(){return m_previousState;};
    bool getCurrentState(){return m_currentState;};
    bool getRawState();

  private:
    int m_pin;
    bool m_previousState;
    bool m_currentState;
    unsigned long m_timeChanged;
};


#endif
// BUTTON_H
