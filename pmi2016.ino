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
#include "Rod.h"
//#include "DueTimer.h"


Servo srv_umbrella;

Servo * srv_claw_left = new Servo();
Servo * srv_claw_right = new Servo();
Servo * srv_arm = new Servo();
Servo * srv_release = new Servo();

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
Rod rod = Rod(srv_arm, srv_release);

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

typedef enum{
	FISH_POSITION_FOR_RECAL,
	FISH_ENTRY_RECAL,
	FISH_STRAIGHT_RECAL,
	FISH_FISH_1,
	FISH_GO_DROP,
	FISH
} state_fish_t;

typedef enum{
	ST_1,
    ST_2,
	ST_3,
	ST_4,
	ST_5,
	ST_6,
	ST_7,
	ST_8,
	ST_9,
	ST_10,
	ST_11,
	ST_12,
	ST_13,
	ST_14,
	ST_15,
	ST_16,
	ST_17,
	ST_18,
	ST_19,
	ST_20,
	ST_1_1,
    ST_2_1,
	ST_3_1,
	ST_4_1,
	ST_5_1,
	ST_6_1,
	ST_7_1,
	ST_8_1,
	ST_9_1,
	ST_10_1,
	ST_11_1,
	ST_12_1,
	ST_13_1,
	ST_14_1,
	ST_15_1,
	ST_16_1,
	ST_17_1,
	ST_18_1,
	ST_19_1,
	ST_20_1,
} state_t;


strat1 state1 = BEGIN;
state_homol_t state_homol = HO_BEGIN;
state_t state_fish = ST_1;
state_t state_shell = ST_1;
state_t state_master = ST_1;
state_t state_cabin = ST_1;

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

	srv_umbrella.attach(SERVO8);
	srv_umbrella.write(UMBRELLA_ARMED);

	srv_claw_left->attach(SERVO1);
	srv_claw_right->attach(SERVO5);
	srv_arm->attach(SERVO6);
	srv_release->attach(SERVO2);

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
		if (btn_color->getRawState() == 0)
		{
			color = GRN;
			rgb.set(GREEN,ON);
		}
		else
		{
			color = PRP;
			rgb.set(PURPLE,ON);
		}

		rod.setColor(color);
		test_claws();
		test_umbrella();
		test_release();
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
			strat_master();
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
	if (!die_motherfucker)
	{
		claws.run();
		detection.run();
		rod.run();
	}

	rgb.run();
	yield();
}

void test_inputs(){
	digitalWrite(LED1 , digitalRead(OMRON1));
	digitalWrite(LED2 , digitalRead(OMRON2));
	digitalWrite(LED3 , digitalRead(OMRON3));
	digitalWrite(LED4 , digitalRead(OMRON4));
}

void test_claws(){
	if (btn_strat1.getRawState())
	{
		claws.open();
	}
	else
	{
		claws.close();
	}
}

void test_umbrella(){
	if (btn_strat1.getRawState())
	{
		srv_umbrella.write(UMBRELLA_RELEASED);
	}
	else
	{
		srv_umbrella.write(UMBRELLA_ARMED);
	}
}

void test_release(){
	if (btn_strat1.getRawState())
	{
		rod.fish();
	}
	else
	{
		rod.rest();
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
			rod.setColor(color);
		}
		else
		{
			color = PRP;
			rgb.set(PURPLE,ON);
			rod.setColor(color);
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
		rod.setColor(color);
	}
}

void funny_action(){
	delay(2000);
	srv_umbrella.write(UMBRELLA_RELEASED);
	rgb.set(WHITE,SLOW);
	die_motherfucker = true;
}

void inhib_fishing_color(){
	if (color == GRN)
		detection.disableLeft();
	else
		detection.disableRight();
}

bool strat_cabin(){
	bool ret = false;
	digitalWrite(PAP_ENABLE, LOW);

	// By default, enable avoidance
	// Is is disable after in the states
	detection.activateAll();

	switch (state_cabin)
	{
	case ST_1:
		detection.disableRear();
		if (nav.go_s(1200,-200))
			state_cabin = ST_2;
		break;
	case ST_2:
		detection.disableRear();
		if (nav.go_s(1200,740))
			state_cabin = ST_3;
		break;
	case ST_3:
		detection.disableRear();
		if (nav.go_s(1200,750,MARCHE_AR))
			state_cabin = ST_4;
		break;
	case ST_4:
		nav.setSpeed(SPEED_RECAL);
		detection.disableRear();
		if (nav.go_s(1200,1050,MARCHE_AR))
		{
			// recal to cabin door
			nav.setOdom(nav.getX_uncolored(),1000-10-X_AR,-M_PI/2.0);
			state_cabin = ST_5;
			nav.startTraj();
		}
		break;
	case ST_5:
		detection.disableRear();
		if (nav.straight(200))
			state_cabin = ST_6;
		break;
	case ST_6:
		if (nav.go_s(900,750))
			state_cabin = ST_6_1;
		break;
	case ST_6_1:
		detection.disableRear();
		if (nav.go_s(900,760,MARCHE_AR))
			state_cabin = ST_7;
		break;
	case ST_7:
		detection.disableRear();
		nav.setSpeed(SPEED_RECAL);
		if (nav.go_s(900,1000,MARCHE_AR))
		{
			// recal to cabin door
			nav.setOdom(nav.getX_uncolored(),1000-10-X_AR,-M_PI/2.0);
			state_cabin = ST_8;
		}
		break;
	case ST_8:
		detection.disableRear();
		if (nav.go_s(900,900))
			state_cabin = ST_9;
		break;
	case ST_9:
		detection.disableRear();
		if (nav.go_s(1400,900))
			state_cabin = ST_10;
		break;
	case ST_10:
		ret = true;
//
//		detection.disableFront();
//		if (nav.go_s(200,500,MARCHE_AR))
//			state_cabin = ST_10;
		break;
	default:;
	}

	return ret;
}

//
// ___ _____ ___    _ _____   ___ ___ ___ _  _
/// __|_   _| _ \  /_\_   _| | __|_ _/ __| || |
//\__ \ | | |   / / _ \| |   | _| | |\__ \ __ |
//|___/ |_| |_|_\/_/ \_\_|   |_| |___|___/_||_|
//

bool
strat_fish(){
	bool ret = false;
	digitalWrite(PAP_ENABLE, LOW);

	// By default, enable avoidance
	// Is is disable after in the states
	detection.activateAll();

	switch (state_fish)
	{
	case ST_1:
		if (nav.go_s(1000,-800))
			state_fish = ST_2;
		break;
	case ST_2:
		detection.disableRear();
		if (nav.go_s(1000,-900,MARCHE_AR))
		{
			state_fish = ST_2_1;
		}
		break;
	case ST_2_1:
		detection.disableRear();
		nav.setSpeed(SPEED_RECAL);
		if (nav.go_s(1000,-1050,MARCHE_AR))
		{
			nav.setOdom(nav.getX_uncolored(), -1000+X_AR, M_PI/2.0);
			state_fish = ST_3;
		}
		break;
	case ST_3:
		detection.disableRear();
		if (nav.straight(DISTANCE_TO_WALL_WHILE_FISHING-X_AR))
			state_fish = ST_4;
		break;
	case ST_4:
		if (nav.go_s(990,-1000+DISTANCE_TO_WALL_WHILE_FISHING))
			state_fish = ST_5;
		break;
	case ST_5:
		rod.fish();
		inhib_fishing_color();
		delay(FISH_DURATION_MS+100);
		state_fish = ST_6;
		break;
	case ST_6:
		rod.fish();
		inhib_fishing_color();
		nav.setSpeed(SPEED_FISHING);
		if (nav.go_s(800,-1000+DISTANCE_TO_WALL_WHILE_FISHING))
		{
			state_fish = ST_7;
			nav.setSpeed(0);
		}
		break;
	case ST_7:
		rod.travelFish();
		inhib_fishing_color();
		if (nav.go_s(650,-1000+DISTANCE_TO_WALL_WHILE_DROPING))
			state_fish = ST_8;
		break;
	case ST_8:
		rod.travelFish();
		inhib_fishing_color();
		if (nav.go_s(500,-1000+DISTANCE_TO_WALL_WHILE_DROPING))
			state_fish = ST_9;
		break;
	case ST_9:
		rod.travelFish();
		inhib_fishing_color();
		if (nav.go_s(400,-1000+DISTANCE_TO_WALL_WHILE_FISHING))
			state_fish = ST_10;
		break;
	case ST_10:
		rod.travelFish();
		inhib_fishing_color();
		if (nav.go_s(390,-1000+DISTANCE_TO_WALL_WHILE_FISHING))
			state_fish = ST_11;
		break;
	case ST_11:
		for (int i=0; i < RETRY_DROP; i++){
			rod.drop();
			delay(500);
			rod.positionDrop();
			delay(500);
		}
		state_fish = ST_12;
		break;
	case ST_12:
		rod.travelFish();
		inhib_fishing_color();
		ret = true;
		break;
	default:;
	}

	return ret;
}


bool
strat_startup(){
	bool ret = false;
	static int state = 0;
	digitalWrite(PAP_ENABLE, LOW);

	// By default, enable avoidance
	// Is is disable after in the states
	detection.activateAll();

	switch (state)
	{
	case 0:
		detection.disableRear();
		state = 1;
		nav.startTraj();
		break;
	case 1:
		detection.disableRear();
		if (nav.straight(-20))
		{
			nav.setOdom(1500-X_AR, -100+Y_GAUCHE, -M_PI);
			nav.startTraj();
			state = 2;
		}
		break;
	case 2:
		detection.disableRear();
		if (nav.straight(60.0))
			state = 3;
		break;
	case 3:
		ret = true; // ready to go!
		break;
	default:;
	}

	return ret;
}


// ___ _  _ ___ _    _
/// __| || | __| |  | |
//\__ \ __ | _|| |__| |__
//|___/_||_|___|____|____|
//

bool strat_shell(){

	// By default, enable avoidance
	// Is is disable after in the states
	detection.activateAll();

	bool ret = false;
	switch (state_shell){
	case ST_1:
		detection.disableRear();
		if (nav.straight(150))
			state_shell = ST_1_1;
		break;
	case ST_1_1:
		detection.disableRear();
		if (nav.go_s(1050,-430))
			state_shell = ST_2;
		break;
	case ST_2:
		detection.disableFront();
		claws.open();
		if (nav.go_s(1300,-380,MARCHE_AR))
			state_shell = ST_3;
		break;
	case ST_3:
		claws.open();
		detection.disableFront();
		if (nav.go_s(1350,80,MARCHE_AR))
			state_shell = ST_4;
		break;
	case ST_4:
		claws.open();
		detection.disableRear();
		if (nav.go_s(1100,-700))
			state_shell = ST_5;
		break;
	case ST_5:
		claws.open();
		detection.disableFront();
		if (nav.go_s(1300,-680,MARCHE_AR))
			state_shell = ST_6;
		break;
	case ST_6:
		claws.open();
		detection.disableFront();
		if (nav.go_s(1350,0,MARCHE_AR))
			state_shell = ST_7;
		break;
	case ST_7:
		claws.open();
		if (nav.go_s(1350,-200))
			state_shell = ST_8;
		break;
	case ST_8:
		claws.close();
		detection.disableRear();
		if (nav.go_s(1450,-200,MARCHE_AR))
		{
			state_shell = ST_8_1;
		}
		break;
	case ST_8_1:
		claws.close();
		detection.disableRear();
		nav.setSpeed(SPEED_RECAL);
		if (nav.go_s(1600,-200,MARCHE_AR))
		{
			nav.setOdom(1500-X_AR, nav.getY(), -M_PI);
			state_shell = ST_9;
		}
		break;
	case ST_9:
		claws.close();
		detection.disableRear();
		if (nav.straight(150))
			state_shell = ST_10;
		break;
	case ST_10:
		ret = true;
		break;
	default:;
	}

	return ret;
}

//
// __  __   _   ___ _____ ___ ___
//|  \/  | /_\ / __|_   _| __| _ \
//| |\/| |/ _ \\__ \ | | | _||   /
//|_|  |_/_/ \_\___/ |_| |___|_|_\
//

void strat_master(){
	digitalWrite(PAP_ENABLE, LOW);

	switch (state_master)
	{
	case ST_1:
		if (strat_startup())
			state_master = ST_2;
		break;
	case ST_2:
		if (strat_shell())
			state_master = ST_3;
		break;
	case ST_3:
		if (strat_cabin())
			state_master = ST_5;
		break;
	case ST_4:
		if (strat_fish())
			state_master = ST_5;
		break;
	case ST_5:
		break; // do nothing
	default:;
	}
}
