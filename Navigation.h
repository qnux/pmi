// Classe de gestion de la navigation (déplacements du robot sur la table)
#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <stdint.h>
#include "Arduino.h"
#include <AccelStepper.h>
#include "constants.h"

typedef enum{
	SENS_UNDEFINED,
	SENS_AV,
	SENS_AR
} sens_t;

class Navigation
{
  public:
    Navigation(AccelStepper* sG, AccelStepper* sD);

    int go_s(float x, float y, int sens = 1);
    int go_s(float x, float y, float t, int sens = 1);
    int go_c(float x, float y, float t);
    int is_finished();

    void setOdom(float x, float y, float t);

    int straight(float dist);
    int turn(float angle);


    void reposition();

    void compute_odom(float dxG,float dxD);

    void motorRun();
    int isArrived();
    void startTraj();

    sens_t getSens(){ return m_sens; }

    void setColor(color_t c){m_color = c;}
    float symX(float x);
    float symT(float t);
    float moduloPiPi(float a);

    float getX(){return m_x;}
    float getX_uncolored() {return symX(m_x);}
    float getY(){return m_y;}
    float getT(){return m_t;}
    void setSpeed(long speed);
    void setSpeedVir(float s){m_speed_virage = s;}

  private:
    float m_x;
    float m_y;
    float m_t;
    int m_inProgress;
    AccelStepper* stepperG;
    AccelStepper* stepperD;
    sens_t m_sens;
    color_t m_color;

    float m_speed_virage;

};

#endif
