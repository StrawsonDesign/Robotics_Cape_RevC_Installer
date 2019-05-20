/**
 * @example    rc_calibrate_sbus.c
 *
 * Running the rc_calibrate_sbus example will print out raw data to
 * the console and record the min and max values for each
 * channel. These limits will be saved to disk so future sbus reads
 * will be scaled correctly.
 *
 * Make sure the transmitter and receiver are paired before
 * testing. The satellite receiver remembers which transmitter it is
 * paired to, not your BeagleBone.
 */

#include <stdio.h>
#include <rc/sbus.h>

int main()
{
	printf("Please connect a SBUS satellite receiver and make sure\n");
	printf("your transmitter is on and paired to the receiver.\n");
	printf("\n");
	printf("Press ENTER to continue or anything else to quit\n");
	getchar();

	// run the calibration routine
	rc_sbus_calibrate_routine();

	return 0;
}
