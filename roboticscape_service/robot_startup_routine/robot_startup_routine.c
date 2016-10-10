/*******************************************************************************
* wait_until_ready.c
*
* James Strawson 2016
* Hardware drivers and the bone capemanager initialize in a rather unpredictable
* schedule. This program checks everything necessary for the cape library to run
* in order.
*******************************************************************************/

#include "../../libraries/usefulincludes.h"
#include "../../libraries/roboticscape.h"
#include "../../libraries/roboticscape-defs.h"
#include "../../libraries/simple_gpio/simple_gpio.h"
#include "../../libraries/simple_pwm/simple_pwm.h"

#define TIMEOUT_S 25
#define START_LOG "/etc/roboticscape/startup_log.txt"

int is_cape_loaded();
int check_timeout();
int setup_gpio();
int setup_pwm();
int setup_pru();

uint64_t start_us;




/*******************************************************************************
* int main()
*
*******************************************************************************/
int main(){
	char buf[128];
	float time;

	// log start time 
	start_us = micros_since_epoch();
	system("echo start > " START_LOG);

	// stop a possibly running process and
	// delete old pid file if it's left over from an improper shudown
	kill_robot();

	// // check capemanager
	// while(is_cape_loaded()!=1){
	// 	if(check_timeout()){
	// 		system("echo 'timeout reached while waiting for overlay to load' >> " START_LOG);
	// 	 	return 1;
	// 	}
	// 	usleep(500000);
		
	// }
	// system("uptime -p >> " START_LOG);
	// system("echo 'cape overlay loaded' >> " START_LOG);

	// export gpio pins
	while(setup_gpio()!=0){
		if(check_timeout()){
			system("echo 'timeout reached while waiting for gpio driver' >> " START_LOG);
			printf("timeout reached while waiting for gpio driver\n");
		 	return -1;
		}
		usleep(500000);
	}
	time = (micros_since_epoch()-start_us)/1000000;
	sprintf(buf, "echo 'time (s): %5f' >> %s",time,START_LOG);
	system(buf);
	system("echo 'gpio pins exported' >> " START_LOG);

	// set up pwm at desired frequnecy
	while(setup_pwm()!=0){
		if(check_timeout()){
			system("echo 'timeout reached while waiting for pwm driver' >> " START_LOG);
			printf("timeout reached while waiting for pwm driver\n");
		 	return -1;
		}
		usleep(500000);
	}
	time = (micros_since_epoch()-start_us)/1000000;
	sprintf(buf, "echo 'time (s): %5f' >> %s",time,START_LOG);
	system(buf);
	system("echo 'pwm initialized' >> " START_LOG);


	// just check for PRU for now, don't wait since we know it doesn't work
	if(setup_pru()!=0){
		system("echo 'Failed to initialize remoteproc pru' >> " START_LOG);
		printf("echo 'Failed to initialize remoteproc pru");
	}

	// wait for pru
	// while(setup_pru()!=0){
	// 	if(check_timeout()){
	// 		system("echo 'timeout reached while waiting for remoteproc pru' >> " START_LOG);
	// 		printf("timeout reached while waiting for remoteproc pru\n");
	// 	 	return 0;
	// 	}
	// 	usleep(500000);
	// }
	// time = (micros_since_epoch()-start_us)/1000000;
	// sprintf(buf, "echo 'time (s): %5f' >> %s",time,START_LOG);
	// system(buf);
	// system("echo 'pru remoteproc initialized' >> " START_LOG);

	cleanup_cape();
	printf("startup routine complete\n");
	system("echo 'startup routine complete' >> " START_LOG);
	return 0;
}

// /******************************************************************************
// * is_cape_loaded()
// *
// * check to make sure robotics cape overlay is loaded
// * return 1 if cape is loaded
// * return 0 if cape is missing
// ******************************************************************************/
// int is_cape_loaded(){
// 	int ret = system("grep -q "CAPE_NAME" /sys/devices/platform/bone_capemgr*/slots");
// 	if(ret == 0) return 1;
// 	return 0;
// }

/*******************************************************************************
* int check_timeout()
*
* looks and the current time to decide if the timeout has been reached.
* returns 1 if timeout has been reached.
*******************************************************************************/
int check_timeout(){
	uint64_t new_us = micros_since_epoch();
	int seconds = (new_us-start_us)/1000000;
	if(seconds>TIMEOUT_S){
		printf("TIMEOUT REACHED\n");
		system("echo 'TIMEOUT_REACHED' >> " START_LOG);
		return 1;
	}
	return 0;
}

/*******************************************************************************
* int setup_gpio()
*
* exports and sets the direction of each gpio pin
*******************************************************************************/
int setup_gpio(){
	
	//export all GPIO output pins
	if(gpio_export(RED_LED)) return -1;
	if(gpio_set_dir(RED_LED, OUTPUT_PIN)) return -1;
	gpio_export(GRN_LED);
	gpio_set_dir(GRN_LED, OUTPUT_PIN);
	gpio_export(MDIR1A);
	gpio_set_dir(MDIR1A, OUTPUT_PIN);
	gpio_export(MDIR1B);
	gpio_set_dir(MDIR1B, OUTPUT_PIN);
	gpio_export(MDIR2A);
	gpio_set_dir(MDIR2A, OUTPUT_PIN);
	gpio_export(MDIR2B);
	gpio_set_dir(MDIR2B, OUTPUT_PIN);
	gpio_export(MDIR3A);
	gpio_set_dir(MDIR3A, OUTPUT_PIN);
	gpio_export(MDIR3B);
	gpio_set_dir(MDIR3B, OUTPUT_PIN);
	gpio_export(MDIR4A);
	gpio_set_dir(MDIR4A, OUTPUT_PIN);
	gpio_export(MDIR4B);
	gpio_set_dir(MDIR4B, OUTPUT_PIN);
	gpio_export(MOT_STBY);
	gpio_set_dir(MOT_STBY, OUTPUT_PIN);
	gpio_export(PAIRING_PIN);
	gpio_set_dir(PAIRING_PIN, OUTPUT_PIN);
	gpio_export(SERVO_PWR);
	gpio_set_dir(SERVO_PWR, OUTPUT_PIN);

	//set up mode pin
	gpio_export(MODE_BTN);
	gpio_set_dir(MODE_BTN, INPUT_PIN);
	gpio_set_edge(MODE_BTN, "both");  // Can be rising, falling or both
	
	//set up pause pin
	gpio_export(PAUSE_BTN);
	gpio_set_dir(PAUSE_BTN, INPUT_PIN);
	gpio_set_edge(PAUSE_BTN, "both");  // Can be rising, falling or both

	// set up IMU interrupt pin
	//set up gpio interrupt pin connected to imu
	gpio_export(IMU_INTERRUPT_PIN);
	gpio_set_dir(IMU_INTERRUPT_PIN, INPUT_PIN);
	gpio_set_edge(IMU_INTERRUPT_PIN, "falling");

	return 0;
}


/*******************************************************************************
* int setup_pwm()
*
* exports and sets the direction of each gpio pin
*******************************************************************************/
int setup_pwm(){
	if(simple_init_pwm(1,PWM_FREQ)) return -1;
	if(simple_init_pwm(2,PWM_FREQ)) return -1;
	return 0;
}


/*******************************************************************************
* int setup_pru()
*
* makes sure remoteproc for the pru is up and running, then reboots both cores
*******************************************************************************/
int setup_pru(){

	// make sure driver is running
	if(access("/sys/bus/platform/drivers/pru-rproc/bind", F_OK)) return -1;

	system("echo 4a334000.pru0 > /sys/bus/platform/drivers/pru-rproc/unbind  > /dev/null");
	system("echo 4a334000.pru0 > /sys/bus/platform/drivers/pru-rproc/bind > /dev/null");
	system("echo 4a338000.pru1  > /sys/bus/platform/drivers/pru-rproc/unbind > /dev/null");
	system("echo 4a338000.pru1 > /sys/bus/platform/drivers/pru-rproc/bind > /dev/null");
	return 0;
}
