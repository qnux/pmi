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
#include <math.h>
#include "Claws.h"
//#include "DueTimer.h"


Servo srv_bras;
Servo srv_umbrella;

Servo * srv_claw_left = new Servo();
Servo * srv_claw_right = new Servo();

Button btn_start = Button(BUTTON_START);
Button *btn_color;
Button btn_strat1 = Button(BUTTON_STRAT1);
Button btn_strat2 = Button(BUTTON_STRAT2);
RGB rgb = RGB(LED_RGB_R,LED_RGB_G,LED_RGB_B);
AccelStepper *stepperG = new AccelStepper(AccelStepper::DRIVER, PAPG_STEP, PAPG_DIR);
AccelStepper *stepperD = new AccelStepper(AccelStepper::DRIVER, PAPD_STEP, PAPD_DIR);
Navigation nav = Navigation(stepperG,stepperD);
Detection detection = Detection(OMRON1, OMRON2, OMRON3, OMRON4);
Claws claws = Claws(srv_claw_left,srv_claw_right);

color_t color = COLOR_UNDEF;

typedef enum{
	BEGIN,
	LIGNE1,
	LIGNE2,
	LIGNE3,
	LIGNE4,
	LIGNE5
} strat1;

typedef enum{
	HO_BEGIN,
	HO_RECALAGE_START,
	HO_LIGNE1,
	HO_SNEAK_WALL,
	HO_ACCOSTAGE_START_CABIN,
	HO_LIGNE2,
	HO_VIRAGE2,
	HO_RECUL_PUSH_START_CABIN,
	HO_DEGAGEMENT_START_CABIN,
	HO_ACCOSTAGE_DUNE_CABIN,
	HO_RECUL_PUSH_DUNE_CABIN,
	HO_LIGNE6,
	HO_VIRAGE6,
	HO_GO_DUNE, //loop for avoidance
	HO_BACK_DUNE
} state_homol_t;

strat1 state1 = BEGIN;
state_homol_t state_homol = HO_BEGIN;

int started = 0;
int ended = 0;
int die_motherfucker = 0;
unsigned long dateStart = 0;

void setup() {
	//	Timer6.attachInterrupt(motors).start(200);
	pinMode(BUTTON_START, INPUT);
	pinMode(BUTTON_COLOR, INPUT);
	btn_color = new Button(BUTTON_COLOR);
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
	srv_bras.attach(SERVO6);
	srv_umbrella.attach(SERVO8);
	srv_umbrella.write(UMBRELLA_ARMED);

	srv_claw_left->attach(SERVO1);
	srv_claw_right->attach(SERVO5);

	stepperG->setMaxSpeed(V_MAX);
	stepperD->setMaxSpeed(V_MAX);
	stepperG->setAcceleration(ACC_MAX);
	stepperD->setAcceleration(ACC_MAX);

	rgb.set(BLUE,ON);
	rgb.run();
	// bip du pauvre :
	for (int i = 0; i < 100; i++)
	{
		digitalWrite(BUZZER, HIGH);
		delayMicroseconds(350);
		digitalWrite(BUZZER, LOW);
		delayMicroseconds(350);
	}
	rgb.off();rgb.run();

	Scheduler.startLoop(loop2);
	Scheduler.startLoop(loop3);

	Serial.begin(115200);
	Serial.println("Hello world.");

	rgb.set(RED,FAST);
	srv_bras.write(95);

}

void loop() {

	if (die_motherfucker)
	{
		yield();
		return;
	}

	if (btn_strat2.getRawState())
	{
		rgb.set(YELLOW,FAST);
		test_claws();
		test_umbrella();
		yield();
		return;
	}

	if (ended)
	{
		funny_action();
		yield();
		return;
	}

	if (!started)
	{
		wait_start();
		digitalWrite(PAP_ENABLE, HIGH);
	}
	else
	{
		if (started && millis() - dateStart < DURATION_MATCH_MS)
		{
			strat_homol();
			test_inputs();
		}
		else
		{
			digitalWrite(PAP_ENABLE, HIGH);
			ended = true;
			rgb.set(CYAN,FAST);
		}
		//		square();
		//		etalon();
	}

	//	test_buttons();

	yield();
}

void loop2()
//void motors()
{
	if (started)
	{
		if (!detection.getDetectionFront() && !detection.getDetectionRear())
		{
			nav.motorRun();
		}
	}
	yield();
}

// Appel de tous les runs
void loop3()
{
	claws.run();
	detection.run();
	rgb.run();
	yield();
}

void square(){

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
			Serial.println("LIGNE4 -> LIGNE5");
			state1 = LIGNE5;
			nav.startTraj();
		}
		break;
	case LIGNE5:
		rgb.set(GREEN,FAST);
		if(nav.turn(M_PI/2))
		{
			Serial.println("LIGNE5 -> BEGIN");
			state1 = BEGIN;
			started = false;
		}
		break;
	default:;
	}
}

void test_inputs(){
	digitalWrite(LED1 , digitalRead(OMRON1));
	digitalWrite(LED2 , digitalRead(OMRON2));
	digitalWrite(LED3 , digitalRead(OMRON3));
	digitalWrite(LED4 , digitalRead(OMRON4));
}

void test_claws(){
	if (btn_strat1.newState())
	{
		if (btn_strat1.getCurrentState())
		{
			claws.open();
		}
		else
		{
			claws.close();
		}
	}
}

void test_umbrella(){
	if (btn_strat1.newState())
	{
		if (btn_strat1.getCurrentState())
		{
			srv_umbrella.write(UMBRELLA_RELEASED);
		}
		else
		{
			srv_umbrella.write(UMBRELLA_ARMED);
		}
	}
}

void test_buttons(){
	digitalWrite(LED1 , digitalRead(BUTTON_START));
	digitalWrite(LED2 , digitalRead(BUTTON_COLOR));
	digitalWrite(LED3 , digitalRead(BUTTON_STRAT1));
	digitalWrite(LED4 , digitalRead(BUTTON_STRAT2));
}

void etalon()
{
	digitalWrite(PAP_ENABLE, LOW);
	if (btn_start.newState() && btn_start.getCurrentState() == 0)
	{
		Serial.println("START");
		started = true;
	}

	if (started)
	{
		nav.turn(4*M_PI);
	}

}

void wait_start()
{
	static bool armed = false;

	detection.disableAll();

	if (btn_color->newState())
	{
		if (btn_color->getCurrentState() == 0)
		{
			color = GRN;
			rgb.set(GREEN,ON);
		}
		else
		{
			color = PRP;
			rgb.set(PURPLE,ON);
		}
		delay(1000);
	}


	if (!armed && btn_start.newState() && btn_start.getCurrentState() == 1)
	{
		armed = true;
	}

	if (armed)
	{
		if (color == GRN)
			rgb.set(GREEN,SLOW);
		else if (color == PRP)
			rgb.set(PURPLE,SLOW);
	}
	else
	{
		rgb.set(RED,FAST);
	}

	if (armed && btn_start.newState() && btn_start.getCurrentState() == 0)
	{
		started = true;
		dateStart = millis();
		if (color == GRN)
			rgb.set(GREEN,FAST);
		else if (color == PRP)
			rgb.set(PURPLE,FAST);
	}

	if (started)
	{
		// Initialize all components with the good color
		nav.setColor(color);
	}
}

void funny_action(){
	srv_umbrella.write(UMBRELLA_RELEASED);
	delay(5000);
	rgb.set(WHITE,SLOW);
	die_motherfucker = true;
}

//     __    __    ______   .___  ___.   ______    __
//    |  |  |  |  /  __  \  |   \/   |  /  __  \  |  |
//    |  |__|  | |  |  |  | |  \  /  | |  |  |  | |  |
//    |   __   | |  |  |  | |  |\/|  | |  |  |  | |  |
//    |  |  |  | |  `--'  | |  |  |  | |  `--'  | |  `----.
//    |__|  |__|  \______/  |__|  |__|  \______/  |_______|
//

void strat_homol(){
	digitalWrite(PAP_ENABLE, LOW);

	// By default, enable avoidance
	// Is is disable after in the states
	detection.activateAll();

	switch (state_homol)
	{
	case HO_BEGIN:
		detection.disableRear();
		state_homol = HO_RECALAGE_START;
		nav.startTraj();
		break;
	case HO_RECALAGE_START:
		detection.disableRear();
		if (nav.straight(-20))
		{
			nav.setOdom(1500-X_AR, -100+Y_GAUCHE, -M_PI);
			state_homol = HO_LIGNE1;
		}
		break;
	case HO_LIGNE1:
		detection.disableRear();
		if (nav.straight(60.0))
			state_homol = HO_SNEAK_WALL;
		break;
	case HO_SNEAK_WALL:
		detection.disableRear();
		if (nav.go_s(1400,750))
			state_homol = HO_ACCOSTAGE_START_CABIN;
		break;
	case HO_ACCOSTAGE_START_CABIN:
		detection.disableRear();
		if (nav.go_s(1200,750))
			state_homol = HO_RECUL_PUSH_START_CABIN;
		break;
	case HO_RECUL_PUSH_START_CABIN:
		detection.disableRear();
		if (nav.go_s(1200,1000,MARCHE_AR))
		{
			// recal to cabin door
			nav.setOdom(nav.getX_uncolored(),1000-10-X_AR,-M_PI/2.0);
			state_homol = HO_DEGAGEMENT_START_CABIN;
			nav.startTraj();
		}
		break;
	case HO_DEGAGEMENT_START_CABIN:
		detection.disableRear();
		if (nav.straight(100))
			state_homol = HO_ACCOSTAGE_DUNE_CABIN;
		break;
	case HO_ACCOSTAGE_DUNE_CABIN:
		if (nav.go_s(900,750))
			state_homol = HO_RECUL_PUSH_DUNE_CABIN;
		break;
	case HO_RECUL_PUSH_DUNE_CABIN:
		detection.disableRear();
		if (nav.go_s(900,1000,MARCHE_AR))
		{
			// recal to cabin door
			nav.setOdom(nav.getX_uncolored(),1000-10-X_AR,-M_PI/2.0);
			state_homol = HO_LIGNE6;
		}
		break;
	case HO_LIGNE6:
		detection.disableRear();
		if (nav.go_s(900,500))
			state_homol = HO_VIRAGE6;
		break;
	case HO_VIRAGE6:
		detection.disableFront();
		if (nav.go_s(200,500,MARCHE_AR))
			state_homol = HO_LIGNE6;
		break;
	case HO_GO_DUNE:

		break;
	case HO_BACK_DUNE:

		break;
	default:;
	}
}
