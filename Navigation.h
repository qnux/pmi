// Classe de gestion de la navigation (déplacements du robot sur la table)
#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <stdint.h>
#include "Arduino.h"
#include <AccelStepper.h>

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

    int straight(float dist);
    int turn(float angle);


    void reposition();

    void compute_odom(float dxG,float dxD);

    void motorRun();
    void run();
    int isArrived();
    void startTraj();

    sens_t getSens(){ return m_sens; }


  private:
    double m_x;
    double m_y;
    double m_t;
    int m_inProgress;
    AccelStepper* stepperG;
    AccelStepper* stepperD;
    sens_t m_sens;


};

#endif
