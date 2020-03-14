// Compiler instructions

#include <Nextion.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "FS.h"
#include "SPIFFS.h"
#include "RTClib.h"

#define ONE_WIRE_BUS 4					// One-wire data wire is plugged TO GPIO

RTC_DS3231 rtc;

OneWire oneWire(ONE_WIRE_BUS);			// Setup a oneWire instance to communicate with devices
DallasTemperature sensors(&oneWire);	// Pass our oneWire reference to Dallas Temperature.
uint8_t sensorOutside[8] = { 0x28, 0x12, 0x88, 0xEC, 0x1B, 0x19, 0x01, 0x0E }; // sensor 1
uint8_t sensorInside[8] = { 0x28, 0x9B, 0xB0, 0xCF, 0x1B, 0x19, 0x01, 0x30 }; // sensor 2

// declare global variables
long count = 0;					// for the delay loop to call the read_Temperatures function
bool ran = false;					// used to call the read_Temperatures function on first run thru the loop
char varTempUnits[] = "C";		// variable to track temperature unit setting
int ActiveDateBox;
char newYear[5];
char newMonth[6];
char newDay[3];
char newHour[3];
char newMinute[3];
char newNumber[5];
char char_array[5];
char caseTemperature[3];

// declare Nextion objects using the format: Nex????(page, ID, "name")
// page 0
NexPage pMain = NexPage(0, 0,"page0");
NexText tTout = NexText(0, 1, "tTout");		// outside temperature
NexText tTin = NexText(0, 2, "tTin");		// inside temperature
NexText tUnit1 = NexText(0, 3, "tUnit1");	// outside temperatre units
NexText tUnit2 = NexText(0, 4, "tUnit2");	// inside tpemperature units
NexText tTime = NexText(0, 5, "tTime");		// show the time
NexText tMenu = NexText(0, 6, "tMenu");		// go to the menu page
NexText tAmPm = NexText(0, 7, "tAmPm");		// show am or pm
NexText tDate = NexText(0, 10, "tDate");
//page 1
NexPage pMenu = NexPage(1, 0,"page1");
NexText tMain = NexText(1, 1, "tMain");				// return to the main page
NexText tSwapUnits = NexText(1, 2, "tSwapUnits");	// swap temperature units
NexText tSetDateTime = NexText(1, 3, "tSetDateTime");
NexText tCaseTemp = NexText(1, 5, "tCaseTemp");
//page 2
NexPage pSetDateTime = NexPage(2, 0, "page2");
NexText tMain2 = NexText(2, 1, "tMain2");
NexText tMenu2 = NexText(2, 2, "tMenu2");
NexText nSetYear = NexText(2, 8, "nSetYear");
NexText nSetMonth = NexText(2, 9, "nSetMonth");
NexText nSetDay = NexText(2, 10, "nSetDay");
NexText nSetHour = NexText(2, 11, "nSetHour");
NexText nSetMinute = NexText(2, 12, "nSetMinute");
NexText t0 = NexText(2, 3, "t0");
NexText t1 = NexText(2, 4, "t1");
NexText t2 = NexText(2, 5, "t2");
NexText t3 = NexText(2, 6, "t3");
NexText t4 = NexText(2, 7, "t4");
NexButton bDelete = NexButton(2, 20, "bDelete");
NexButton bZero = NexButton(2, 16, "b0");
NexButton bOne = NexButton(2, 15, "b1");
NexButton bTwo = NexButton(2, 19, "b2");
NexButton bThree = NexButton(2, 23, "b3");
NexButton bFour = NexButton(2, 14, "b4");
NexButton bFive = NexButton(2, 18, "b5");
NexButton bsix = NexButton(2, 22, "b6");
NexButton bSeven = NexButton(2, 13, "b7");
NexButton bEight = NexButton(2, 17, "b8");
NexButton bNine = NexButton(2, 21, "b9");
NexButton bSet = NexButton(2, 24, "bSet");


// declare Nextion objects using the format: Nex????(page, ID, "name")
// enumerate Nextion items to listen for
NexTouch *nex_listen_list[] = {
	&tMenu,
	&tMenu2,
	&tSwapUnits,
	&tSetDateTime,
	&tMain,
	&tMain2,
	&nSetYear,
	&nSetMonth,
	&nSetDay,
	&nSetHour,
	&nSetMinute,
	&bDelete,
	&bZero,
	&bOne,
	&bTwo,
	&bThree,
	&bFour,
	&bFive,
	&bsix,
	&bSeven,
	&bEight,
	&bNine,
	&bSet,
 	NULL
};

//***** Setup function *************************************************************************
void setup() {
	Serial.begin(115200);
	nexInit();
	SPIFFS.begin();
	tSwapUnits.attachPop(tSwapUnits_Release, &tSwapUnits);	// attach object from the listen list to a function
	tMenu.attachPop(tMenu_Release, &tMenu);
	tMain.attachPop(tMain_Release, &tMain);

	tSetDateTime.attachPop(tSetDateTime_Release, & tSetDateTime);
	nSetYear.attachPop(nSetYear_Release, &nSetYear);
	nSetMonth.attachPop(nSetMonth_Release, &nSetMonth);
	nSetDay.attachPop(nSetDay_Release, &nSetDay);
	nSetHour.attachPop(nSetHour_Release, &nSetHour);
	nSetMinute.attachPop(nSetMinute_Release, &nSetMinute);

	tMenu2.attachPop(tMenu_Release, &tMenu);
	tMain2.attachPop(tMain_Release, &tMain);
	bDelete.attachPop(bDelete_Release, &bDelete);
	bZero.attachPop(bZero_Release, &bZero);
	bOne.attachPop(bOne_Release, &bOne);
	bTwo.attachPop(bTwo_Release, &bTwo);
	bThree.attachPop(bThree_Release, &bThree);
	bFour.attachPop(bFour_Release, &bFour);
	bFive.attachPop(bFive_Release, &bFive);
	bsix.attachPop(bSix_Release, &bsix);
	bSeven.attachPop(bSeven_Release, &bSeven);
	bEight.attachPop(bEight_Release, &bEight);
	bNine.attachPop(bNine_Release, &bNine);
	bSet.attachPop(bSet_Release, &bSet);


	sensors.begin();										// Start the temperature sensors
	rtc.begin();
	// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	pMain.show();
	set_temp_units();										// set the temperature units
}

//-------------------------------------------------------
void nSetYear_Release(void *ptr) {
	ActiveDateBox = 0;
	strcpy(newNumber, "");
	nSetYear.setText("");
	t0.setText("EDIT");
	t1.setText("month");
	t2.setText("day");
	t3.setText("hour");
	t4.setText("minute");
}

//-------------------------------------------------------
void nSetMonth_Release(void *ptr) {
	ActiveDateBox = 1;
	strcpy(newNumber, "");
	nSetMonth.setText("");
	t0.setText("year");
	t1.setText("EDIT");
	t2.setText("day");
	t3.setText("hour");
	t4.setText("minute");
}

//-------------------------------------------------------
void nSetDay_Release(void *ptr) {
	ActiveDateBox = 2;
	strcpy(newNumber, "");
	nSetDay.setText("");
	t0.setText("year");
	t1.setText("month");
	t2.setText("EDIT");
	t3.setText("hour");
	t4.setText("minute");
}

//-------------------------------------------------------
void nSetHour_Release(void *ptr) {
	ActiveDateBox = 3;
	strcpy(newNumber, "");
	nSetHour.setText("");
	t0.setText("year");
	t1.setText("month");
	t2.setText("day");
	t3.setText("EDIT");
	t4.setText("minute");
}

//-------------------------------------------------------
void nSetMinute_Release(void *ptr) {
	ActiveDateBox = 4;
	strcpy(newNumber, "");
	nSetMinute.setText("");
	t0.setText("year");
	t1.setText("month");
	t2.setText("day");
	t3.setText("hour");
	t4.setText("EDIT");
}

//-------------------------------------------------------
void bDelete_Release(void *ptr) {
	int keyPressed = 10;
	button_press(keyPressed);
}

//-------------------------------------------------------
void bZero_Release(void *ptr) {
	int keyPressed = 0;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bOne_Release(void *ptr) {
	int keyPressed = 1;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bTwo_Release(void *ptr) {
	int keyPressed = 2;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bThree_Release(void *ptr) {
	int keyPressed = 3;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bFour_Release(void *ptr) {
	int keyPressed = 4;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bFive_Release(void *ptr) {
	int keyPressed = 5;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bSix_Release(void *ptr) {
	int keyPressed = 6;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bSeven_Release(void *ptr) {
	int keyPressed = 7;
	button_press(keyPressed);
}
//-------------------------------------------------------
void bEight_Release(void *ptr) {
	int keyPressed = 8;
	button_press(keyPressed);
}

//-------------------------------------------------------
void bNine_Release(void *ptr) {
	int keyPressed = 9;
	button_press(keyPressed);
}

//-------------------------------------------------------
void bSet_Release(void *ptr) {
	int count;
	char space[] = " ";
	char colon[] = ":";
	char newTime[10];
	char newDate[12];
	char nullSeconds[3] = "00";
	char setMonths[12][5]{	"Jan ",
							"Feb ",
							"Mar ",
							"Apr ",
							"May ",
							"Jun ",
							"Jul ",
							"Aug ",
							"Sep ",
							"Oct ",
							"Nov ",
							"Dec "};

	count = atoi(newMonth);
	count = count - 1;
	strcpy(newDate, setMonths[count]);
	strcat(newDate, newDay);
	strcat(newDate, space);
	strcat(newDate, newYear);
	strcpy(newTime, newHour);
	strcat(newTime, colon);
	strcat(newTime, newMinute);
	strcat(newTime, colon);
	strcat(newTime, nullSeconds);
	rtc.adjust(DateTime(F(newDate), F(newTime)));
	refesh_date_time_Page();
}

//-------------------------------------------------------
void tSetDateTime_Release(void *ptr) {
	DateTime now = rtc.now();
	char year[] = "YYYY";
	char month[] = "MM";
	char day[] = "DD";
	char hour[] = "hh";
	char minute[] = "mm";
	strcpy(newYear, now.toString(year));
	strcpy(newMonth, now.toString(month));
	strcpy(newDay, now.toString(day));
	strcpy(newHour, now.toString(hour));
	strcpy(newMinute, now.toString(minute));
	strcpy(newNumber, "0");
	refesh_date_time_Page();
}

//-------------------------------------------------------
void refesh_date_time_Page() {
	pSetDateTime.show();
	char year[] = "YYYY";
	char month[] = "MM";
	char day[] = "DD";
	char hour[] = "hh";
	char minute[] = "mm";

	DateTime now = rtc.now();
	strcpy(year, now.toString(year));
	strcpy(month, now.toString(month));
	strcpy(day, now.toString(day));
	strcpy(hour, now.toString(hour));
	strcpy(minute, now.toString(minute));

	nSetYear.setText(year);
	nSetMonth.setText(month);
	nSetDay.setText(day);
	nSetHour.setText(hour);
	nSetMinute.setText(minute);
}

//-------------------------------------------------------
void pMain_update(){
	//char char_array[2];
	//varTempUnits.toCharArray(char_array, 2);
	tUnit1.setText(varTempUnits);
	tUnit2.setText(varTempUnits);
	read_Temperatures();
	read_time();
}

//-------------------------------------------------------
void pMenu_update() {
	String filedUnits = read_Variable("/tempUnits.txt");
	filedUnits.toCharArray(varTempUnits, 2);

	if(strcmp(varTempUnits, "C") == 0) {
		tSwapUnits.setText("Change temperature units from C to F");
	}
	if(strcmp(varTempUnits, "F") == 0) {
		tSwapUnits.setText("Change temperature units from F to C");
	}
	int cTemp = rtc.getTemperature();

	itoa(cTemp, caseTemperature, 10);
	tCaseTemp.setText(caseTemperature);
}

//-------------------------------------------------------
void tMain_Release(void *ptr) {
	pMain.show();
	pMain_update();
}

//-------------------------------------------------------
void tMenu_Release(void *ptr) {
	pMenu.show();
	pMenu_update();
}

//-------------------------------------------------------
void tSwapUnits_Release(void *ptr) {
		if(strcmp(varTempUnits, "C") == 0)  {
		write_Variable("/tempUnits.txt", "F");
	}
		if(strcmp(varTempUnits, "F") == 0) {
		write_Variable("/tempUnits.txt", "C");
	}
	pMenu_update();
	pMain.show();
	count = 13900000;
}

//***** Main Program Loop **********************************************************************
void loop() {
	nexLoop(nex_listen_list);
	if (!ran) {
		pMain_update();
		ran = !ran;
	}
	if (count == 14000000) {
		pMain_update();
		count = 0;
	}
	count = count + 1;
}

//-------------------------------------------------------
void read_time(){
	char days[7][12]{	"Sunday, ",
				  		"Monday, ",
				  		"Tuesday, ",
				  		"Wednesday, ",
				  		"Thursday, ",
				  		"Friday, ",
				  		"Saturday, "};

	char months[12][12]{"January, ",
					  	"February, ",
					  	"March, ",
					  	"April, ",
	 				  	"May, ",
					  	"June, ",
					  	"July, ",
					  	"August, ",
					  	"September, ",
					  	"October, ",
					  	"November, ",
					  	"December, "};
	char buffy[5];
	char myHour[3];
	char myMinute[] = "mm";
	char myTime[6];
	char myDayName[12];
	char myDayNo[] = "DD";
	char myDate[30];
	char myYear[] = "YYYY";
	int myAm = 1;
	DateTime now = rtc.now();

	strcpy(myDate, days[now.dayOfTheWeek()]);
	strcat(myDate, months[now.month()-1]);
	strcat(myDate, now.toString(myDayNo));
	strcat(myDate, ", ");
	strcat(myDate, now.toString(myYear));
	int myHourInt = now.hour();
	if (myHourInt < 13) {
		if (myHourInt > 0){
			itoa(myHourInt, buffy, 10);
			strcpy(myHour, buffy);
		}
		if (myHourInt == 0){
			strcpy(myHour, "12");
		}
	}
	if (myHourInt > 12) {
		myHourInt = myHourInt - 12;
		itoa(myHourInt, buffy, 10);
		strcpy(myHour, buffy);
		myAm = 0;
	}
	strcat(myHour, ":");
	strcpy(buffy, now.toString(myMinute));
	strcat(myHour, myMinute);

	tDate.setText(myDate);
	tTime.setText(myHour);
	if (myAm == 1){
		tAmPm.setText("AM");
		return;
	}
	tAmPm.setText("PM");
}

//-------------------------------------------------------
void set_temp_units() {
	String filedUnits = read_Variable("/tempUnits.txt");
	filedUnits.toCharArray(varTempUnits, 2);
	if(strcmp(varTempUnits, "C") == 0) {

		tUnit1.setText("C"); // makes a nextion error
		tUnit2.setText("C");
	}
	if(strcmp(varTempUnits, "F") == 0) {
		tUnit1.setText("F");
		tUnit2.setText("F");
	}
}

//-------------------------------------------------------
void read_Temperatures() {
	float temp1;
	float temp2;
	char OutTemp[6];					// one extra character required for null terminator
	char InTemp[6];						// one extra character required for null terminator
	sensors.requestTemperatures();
	temp1 = sensors.getTempC(sensorOutside);
	temp2 = sensors.getTempC(sensorInside);

	if(strcmp(varTempUnits, "F") == 0) {
		temp1 = (temp1 * 9/5) + 32;
		temp2 = (temp2 * 9/5) + 32;
	}
	dtostrf(temp1, 5, 1, OutTemp);
	dtostrf(temp2, 5, 1, InTemp);
	tTout.setText(OutTemp);
	tTin.setText(InTemp);
}

//-------------------------------------------------------
String read_Variable(String myFile) {
	File f2 = SPIFFS.open(myFile, "r");
    String contents = f2.readString();		// read the file into a String variable
    f2.close();
	contents.trim();						// use trim to drop the trailing null that screws up the select units logic
	return contents;
}

//-------------------------------------------------------
void write_Variable(String myFile, String myString) {
   File f = SPIFFS.open(myFile, "w");								// open the file for writing
   f.println(myString);												// print the value into the file
   f.close();														// close the file
}

//-------------------------------------------------------
void button_press(int keyPressed) {
	char buffy[5];
	if (ActiveDateBox == 0) { // year
		if (keyPressed < 10) {
			if (strlen(newNumber) > 4) {
				return;
			}
			itoa(keyPressed, buffy, 10);
			strcat(newNumber, buffy);
		}
		if (keyPressed == 10) {
			if(strlen(newNumber)){  //if it's not empty
				newNumber[strlen(newNumber) -1] = '\0'; //replace it with a NULL
			}
		}
		strcpy(newYear, newNumber);
		nSetYear.setText(newNumber);
	}
	if (ActiveDateBox == 1) { // month
		if (keyPressed < 10) {
			if (strlen(newNumber) > 2) {
				return;
			}
			itoa(keyPressed, buffy, 10);
			strcat(newNumber, buffy);
		}
		if (keyPressed == 10) {

			if(strlen(newNumber)){  //if it's not empty
				newNumber[strlen(newNumber) -1] = '\0'; //replace it with a NULL
			}
		}
		strcpy(newMonth, newNumber);
		nSetMonth.setText(newNumber);
	}
	if (ActiveDateBox == 2) { // day
		if (keyPressed < 10) {
			if (strlen(newNumber) > 2) {
				return;
			}
			itoa(keyPressed, buffy, 10);
			strcat(newNumber, buffy);
		}
		if (keyPressed == 10) {
			if(strlen(newNumber)){  //if it's not empty
				newNumber[strlen(newNumber) -1] = '\0'; //replace it with a NULL
			}
		}
		strcpy(newDay, newNumber);
		nSetDay.setText(newNumber);
	}
	if (ActiveDateBox == 3) { // hour
		if (keyPressed < 10) {
			if (strlen(newNumber) > 2) {
				return;
			}
			itoa(keyPressed, buffy, 10);
			strcat(newNumber, buffy);
		}
		if (keyPressed == 10) {
			if(strlen(newNumber)){  //if it's not empty
				newNumber[strlen(newNumber) -1] = '\0'; //replace it with a NULL
			}
		}
		strcpy(newHour, newNumber);
		nSetHour.setText(newNumber);
	}
	if (ActiveDateBox == 4) { // minute
		if (keyPressed < 10) {
			if (strlen(newNumber) > 2) {
				return;
			}
			itoa(keyPressed, buffy, 10);
			strcat(newNumber, buffy);
		}
		if (keyPressed == 10) {
			if(strlen(newNumber)){  //if it's not empty
				newNumber[strlen(newNumber) -1] = '\0'; //replace it with a NULL
			}
		}
		strcpy(newMinute, newNumber);
		nSetMinute.setText(newNumber);
	}
}
