// Compiler instructions

#include <Nextion.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "FS.h"
#include "SPIFFS.h"

// One-wire data wire is plugged TO GPIO
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);			// Setup a oneWire instance to communicate with devices
DallasTemperature sensors(&oneWire);	// Pass our oneWire reference to Dallas Temperature.
String varTempUnits = "C";				// variable to track temperature unit setting
uint8_t sensorOutside[8] = { 0x28, 0x12, 0x88, 0xEC, 0x1B, 0x19, 0x01, 0x0E }; // sensor 1
uint8_t sensorInside[8] = { 0x28, 0x9B, 0xB0, 0xCF, 0x1B, 0x19, 0x01, 0x30 }; // sensor 2


// enumerate Nextion objects using the format: Nex????(page, ID, "name")
NexPage p0 = NexPage(0, 0,"page0");
NexText t0 = NexText(0, 1, "t0");
//NexText t1 = NexText(0, 2, "t1");

NexTouch *nex_listen_list[] = { // enumerate items to listen for
	&t0,
 	NULL
};

void setup() {
	Serial.begin(115200);
	nexInit();
	t0.attachPop(t0_Release, &t0);
	sensors.begin();					// Start the sensors
	//read_Variable("/tempUnits.txt");	// read the temperature units variable

}

//***** Main Program Loop *****

void loop() {
	nexLoop(nex_listen_list);
	//readTemperatures();
}

void t0_Release(void *ptr) {
	Serial.println("The Screen was Touched");
}

void readTemperatures() {
	float temp1;
	float temp2;
	char OutTemp[6];					// one extra character required for null terminator
	char InTemp[6];						// one extra character required for null terminator
	sensors.requestTemperatures(); 		// Send the command to get temperatures

	temp1 = sensors.getTempC(sensorOutside);
	temp2 = sensors.getTempC(sensorInside);
	dtostrf(temp1, 5, 1, OutTemp);
	dtostrf(temp2, 5, 1, InTemp);
	Serial.println("Outside Temperature");
	Serial.println(OutTemp);
	Serial.println();
	Serial.println("Inside Temperature");
	Serial.println(InTemp);
	Serial.println("-------------------");
	Serial.println();
/*
	if (varTempUnits = "C") {
		temp1 = sensors.getTempC(sensorOutside);
		temp2 = sensors.getTempC(sensorInside);
	}
	if (varTempUnits = "F") {
		//temp1 = sensors.getTempF(sensorOutside);
		//temp2 = sensors.getTempF(sensorInside);
	}

*/
/*
	static char sendToNex[5];								// for dtostrf
	dtostrf(temp1, 5, 1, sendToNex);
	nexTempOut.setText(sendToNex);

	dtostrf(temp2, 5, 1, sendToNex);
	nexTempIn.setText(sendToNex);
*/
}

/*
void swar_units(void *ptr) {
	if (varTempUnits == "C") {
		varTempUnits = "F";
		nexUnits1.setText("0-F");
		nexUnits2.setText("0-F");
		nexSwapUnits.setText("Change temperature units from F to C");
		write_Variable("/tempUnits.txt", "F");
		return;
	}
	if (varTempUnits == "F"){
		varTempUnits = "C";
		nexUnits1.setText("0-C");
		nexUnits2.setText("0-C");
		nexSwapUnits.setText("Change temperature units from C to F");
		write_Variable("/tempUnits.txt", "C");
		return;
	}

	readTemperatures();
}


void read_Variable(String myFile) {
   File f = SPIFFS.open(myFile, "r");								// open the file for reading
   String line = f.readStringUntil('\n');							// read the value into the string 'line'
   f.close();														// close the file

   if (myFile == "/tempUnits.txt"){										// if the file name is maxtemp.txt
		varTempUnits = line;										// update the variable
	}

}

void write_Variable(String myFile, String myString) {
   File f = SPIFFS.open(myFile, "w");								// open the file for writing
   f.println(myString);												// print the value into the file
   f.close();														// close the file

}
*/
