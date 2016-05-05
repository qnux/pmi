#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ENABLE_DETECTION 1

#define GAIN_ANALOG_TENSION_BATTERIE 0.024
#define ANTIREBOND_MS 200
#define DELAY_START_MS 1000
#define DELAY_RESTART_DETECTION_MS 500

#define GAIN_STEP_MM 8.409 // 16.818 // OK
//#define GAIN_STEP_DEG 2000.0/218.0*1.75


#define V_MAX 5000 //80000
#define ACC_MAX 15000.0
#define SPEED_RECAL 1500.0
#define SPEED_VIR 2500.0

#define DURATION_MATCH_MS 90000

// CLAWS
#define CLAW_LEFT_CLOSED 14
#define CLAW_LEFT_OPENED 160
#define CLAW_RIGHT_CLOSED 170
#define CLAW_RIGHT_OPENED 15
#define DELAY_MS_CLAWS 300

// UMBRELLA
#define UMBRELLA_ARMED 30
#define UMBRELLA_RELEASED 150

// FISHING ROD
#define ROD_ARD_REST_GREEN 88
#define ROD_ARD_REST_PURPLE 93
#define ROD_ARD_FISHING_GREEN 168
#define ROD_ARD_FISHING_PURPLE 15
#define ROD_ARD_TRAVEL_GREEN 105
#define ROD_ARD_TRAVEL_PURPLE 60
#define ROD_ARD_DROP_GREEN 150
#define ROD_ARD_DROP_PURPLE 40
#define ROD_RELEASE_REST 140
#define ROD_RELEASE_DROP 35
#define FISH_DURATION_MS 700.0
#define SPEED_FISHING 2000

// DIMENSIONS
#define VOIE 104.73
#define X_AR 34.0
#define Y_GAUCHE 60.0

#define OSCILATOR 50 //ms

// position
#define DISTANCE_TO_WALL_WHILE_FISHING 80
#define DISTANCE_TO_WALL_WHILE_DROPING 120
#define RETRY_DROP 2

typedef enum{
	COLOR_UNDEF,
	GRN,
	PRP
} color_t;


#define MARCHE_AR -1
#endif
