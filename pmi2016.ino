#include "Arduino.h"
#include "pinout.h"
#include "constants.h"
#include <Servo.h>
#include <Scheduler.h>
#include "Button.h"
#include <AccelStepper.h>
#include "Navigation.h"
#include "RGB.h"
#include "Detection.h"


Servo srv_bras;
//Servo servoDroite;  // create servo object to control a servo

Button btn_start = Button(BUTTON_START);
Button btn_color = Button(BUTTON_COLOR);
Button btn_strat1 = Button(BUTTON_STRAT1);
Button btn_strat2 = Button(BUTTON_STRAT2);
RGB rgb = RGB(LED_RGB_R,LED_RGB_G,LED_RGB_B);
AccelStepper *stepperG = new AccelStepper(AccelStepper::DRIVER, PAPG_STEP, PAPG_DIR);
AccelStepper *stepperD = new AccelStepper(AccelStepper::DRIVER, PAPD_STEP, PAPD_DIR);
Navigation nav = Navigation(stepperG,stepperD);
Detection detection = Detection(OMRON1, OMRON2, OMRON3, OMRON4);

typedef enum{
	BEGIN,
	LIGNE1,
	LIGNE2,
	LIGNE3,
	LIGNE4
} strat1;

strat1 state1 = BEGIN;

int started = 0;

void setup() {

	pinMode(BUTTON_START, INPUT);
	pinMode(BUTTON_COLOR, INPUT);
	pinMode(BUTTON_STRAT1, INPUT);
	pinMode(BUTTON_STRAT2, INPUT);
	pinMode(LED_RGB_R, OUTPUT);
	pinMode(LED_RGB_G, OUTPUT);
	pinMode(LED_RGB_B, OUTPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	pinMode(PAP_ENABLE, OUTPUT);
	pinMode(BUZZER, OUTPUT);
	pinMode(OMRON1, INPUT_PULLUP);
	pinMode(OMRON2, INPUT_PULLUP);
	pinMode(OMRON3, INPUT_PULLUP);
	pinMode(OMRON4, INPUT_PULLUP);
	digitalWrite(LED1 , LOW);
	digitalWrite(LED2 , LOW);
	digitalWrite(LED3 , LOW);
	digitalWrite(LED4 , LOW);
	srv_bras.attach(SERVO1);

	stepperG->setMaxSpeed(V_MAX);
	stepperD->setMaxSpeed(V_MAX);
	stepperG->setAcceleration(ACC_MAX);
	stepperD->setAcceleration(ACC_MAX);

	rgb.set(WHITE,ON);
	// bip du pauvre :
	for (int i = 0; i < 100; i++)
	{
		digitalWrite(BUZZER, HIGH);
		delayMicroseconds(350);
		digitalWrite(BUZZER, LOW);
		delayMicroseconds(350);
	}
	delay(300);
	rgb.off();

	//Scheduler.startLoop(loop2);
	Scheduler.startLoop(loop3);

	Serial.begin(115200);
	Serial.println("Hello world.");

	rgb.set(BLUE,SLOW);
	srv_bras.write(95);

}

void loop() {

	square();

	test_inputs();


//	if (started)
//	{
//		digitalWrite(PAP_ENABLE, LOW);
//		switch (state1)
//		{
//		case BEGIN:
//			state1 = LIGNE1;
//			break;
//		case LIGNE1:
//			rgb.set(YELLOW,FAST);
//			if(nav.go_s(1000,0))
//			{
//				Serial.println("LIGNE1 -> LIGNE2");
//				delay(30000);
//				state1 = BEGIN;
//				started = false;
//			}
//			break;
//		case LIGNE2:
//			rgb.set(PURPLE,FAST);
//			if(nav.go_s(0,0))
//			{
//				Serial.println("LIGNE2 -> BEGIN");
//				state1 = BEGIN;
//				started = false;
//			}
//			break;
//		default:;
//		}
//	}
//	else
//	{
//		digitalWrite(PAP_ENABLE, HIGH);
//	}

	//motorRun();
	//if (isArrived())
	//{
	//Serial.println("HELLO !!!!!!! ");
	//delay(2000);
	//}
	/*
  if (digitalRead(BUTTON_START))
  {
    digitalWrite(PAP_ENABLE, LOW);
    //turn(360);
    moveToStraight(1000);
    digitalWrite(LED_RGB_G , LOW);
    digitalWrite(LED_RGB_B , HIGH);
    started = 1;
  }

  if (started && isArrived())
  {
    delay(500);
    digitalWrite(PAP_ENABLE, HIGH);
    digitalWrite(LED_RGB_B , LOW);
    digitalWrite(LED_RGB_G , HIGH);
  }
	 */


//	Serial.println(digitalRead(BUTTON_COLOR));

//	if (btn_start.newState())
//	{
//		Serial.println("START");
//	}
//
//	if (btn_color.newState())
//	{
//		Serial.println("COLOR");
//	}
//
//	if (btn_strat1.newState())
//	{
//		Serial.println("STRAT1");
//	}
//
//	if (btn_strat2.newState())
//	{
//		Serial.println("STRAT2");
//	}

//	rgb.set(GREEN,FAST);
//	delay(3000);
//	rgb.set(PURPLE,SLOW);
//	delay(3000);
//	rgb.set(BLUE,ON);
//	delay(3000);


	yield();


}

void loop2()
{
	digitalWrite(LED1 , HIGH);
	delay(100);
	digitalWrite(LED2 , HIGH);
	delay(100);
	digitalWrite(LED3 , HIGH);
	delay(100);
	digitalWrite(LED4 , HIGH);
	delay(100);
	digitalWrite(LED1 , LOW);
	delay(100);
	digitalWrite(LED2 , LOW);
	delay(100);
	digitalWrite(LED3 , LOW);
	delay(100);
	digitalWrite(LED4 , LOW);
	delay(100);
}

// Appel de tous les runs
void loop3()
{
	//  motorRun();
//	static int i;
//	digitalWrite(LED_RGB_R,i++%2);
	detection.run();
	nav.run();
	if (!detection.getDetectionFront())
	{
		nav.motorRun();
	}
	rgb.run();

	yield();
}

void square(){
	if (btn_start.newState() && btn_start.getCurrentState() == 0)
	{
		Serial.println("START");
		started = true;
	}


	if (started)
	{
		digitalWrite(PAP_ENABLE, LOW);
		switch (state1)
		{
		case BEGIN:
			state1 = LIGNE1;
			break;
		case LIGNE1:
			rgb.set(YELLOW,FAST);
			if(nav.go_s(500,0))
			{
				Serial.println("LIGNE1 -> LIGNE2");
				state1 = LIGNE2;
			}
			break;
		case LIGNE2:
			rgb.set(PURPLE,FAST);
			if(nav.go_s(500,500))
			{
				Serial.println("LIGNE2 -> LIGNE3");
				state1 = LIGNE3;
			}
			break;
		case LIGNE3:
			rgb.set(CYAN,FAST);
			if(nav.go_s(0,500))
			{
				Serial.println("LIGNE3 -> LIGNE4");
				state1 = LIGNE4;
			}
			break;
		case LIGNE4:
			rgb.set(GREEN,FAST);
			if(nav.go_s(0,0))
			{
				Serial.println("LIGNE4 -> BEGIN");
				state1 = BEGIN;
				started = false;
			}
			break;
		default:;
		}
	}
	else
	{
		digitalWrite(PAP_ENABLE, HIGH);
	}
}

void test_inputs(){
	digitalWrite(LED1 , digitalRead(OMRON1));
	digitalWrite(LED2 , digitalRead(OMRON2));
	digitalWrite(LED3 , digitalRead(OMRON3));
	digitalWrite(LED4 , digitalRead(OMRON4));
}
