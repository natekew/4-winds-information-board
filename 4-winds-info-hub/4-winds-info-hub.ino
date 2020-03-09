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
String varTempUnits = "C";		// variable to track temperature unit setting
int ActiveDateBox;
int IntnewYear, IntnewMonth, IntnewDay, IntnewHour, IntnewMinute;
String newNumber = "";
char char_array[5];

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
NexText tSetDateTime = NexText(1,3, "tSetDateTime");
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
	pMain.show();
	set_temp_units();										// set the temperature units

}

//-------------------------------------------------------
void nSetYear_Release(void *ptr) {
	ActiveDateBox = 0;
	newNumber = "";
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
	newNumber = "";
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
	newNumber = "";
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
	newNumber = "";
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
	newNumber = "";
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
	rtc.adjust(DateTime(IntnewYear, IntnewMonth, IntnewDay, IntnewHour, IntnewMinute, 0));
	refesh_date_time_Page();
}

//-------------------------------------------------------
void tSetDateTime_Release(void *ptr) {
	refesh_date_time_Page();
}

//-------------------------------------------------------
void refesh_date_time_Page() {
	newNumber = "";
	pSetDateTime.show();
	char year[5];
	char month[3];
	char day[3];
	char hour[3];
	char minute[3];

	DateTime now = rtc.now();

	int intYear = now.year();
	dtostrf(intYear, 5, 0, year);
	nSetYear.setText(year);

	int intMonth = now.month();
	dtostrf(intMonth, 3, 0, month);
	nSetMonth.setText(month);

	int intDay = now.day();
	dtostrf(intDay, 3, 0, day);
	nSetDay.setText(day);

	int intHour = now.hour();
	dtostrf(intHour, 3, 0, hour);
	nSetHour.setText(hour);

	int intMinute = now.minute();
	dtostrf(intMinute, 3, 0, minute);
	nSetMinute.setText(minute);
}

//-------------------------------------------------------
void pMain_update(){
	char char_array[2];
	varTempUnits.toCharArray(char_array, 2);
	tUnit1.setText(char_array);
	tUnit2.setText(char_array);
	read_Temperatures();
	//read_time();
}

//-------------------------------------------------------
void pMenu_update() {
	varTempUnits = read_Variable("/tempUnits.txt");
	if (varTempUnits == "C") {
		tSwapUnits.setText("Change temperature units from C to F");
	}
	if (varTempUnits == "F") {
		tSwapUnits.setText("Change temperature units from F to C");
	}
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
	if (varTempUnits == "C") {
		write_Variable("/tempUnits.txt", "F");
	}
	if (varTempUnits == "F"){
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
		read_Temperatures();
		read_time();
		//pMain_update();
		ran = !ran;
	}
	if (count == 14000000) {
		read_Temperatures();
		read_time();
		//pMain_update();
		count = 0;
	}
	count = count + 1;
}

//-------------------------------------------------------
void read_time(){
	String days[]{"Sunday, ",
				  "Monday, ",
				  "Tuesday, ",
				  "Wednesday, ",
				  "Thursday, ",
				  "Friday, ",
				  "Saturday, "};

	String months[]{"January, ",
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
	String myHour;
	String myMeridiem;
	String myMinute;
	String myTime;
	String myDay;
	String myDate;
	DateTime now = rtc.now();
	myDay = days[now.dayOfTheWeek()];
		//Serial.print("numeric day of the week: ");
		//Serial.println(now.dayOfTheWeek());
		//Serial.print("day of the week from array: ");
		//Serial.println(days[now.dayOfTheWeek()]);

	myDate = myDay;
	myDate = myDate + months[now.month()-1];
		//Serial.print("numeric month: ");
		//Serial.println(now.month());
		//Serial.print("month from the array: ");
		//Serial.println(months[now.month()-1]);

	myDate = myDate + String(now.day()) + ", ";
		//Serial.print("day number: ");
		//Serial.println(String(now.day()));

	myDate = myDate + String(now.year());
	//Serial.print("year number: ");
	//Serial.println(String(now.year()));

	int myHourInt = now.hour();
	if (myHourInt < 13) {
		if (myHourInt > 0){
			myHour = String(myHourInt);
		}
		if (myHourInt == 0){
			myHour = 12;
		}
		myMeridiem = "AM";
	}
	if (myHourInt > 12) {
		myHourInt = myHourInt - 12;
		myHour = String(myHourInt);
		myMeridiem = "PM";
	}
	int intMinute = now.minute();
		if (intMinute > 9){
			myMinute = String(intMinute);
		}
		if (intMinute < 10){
			myMinute = "0" + String(intMinute);
		}
	myTime = myHour + ":" + myMinute;
	char screenDate[myDate.length() + 1];
	char screenTime[myTime.length() + 1];
	char screenMeridiem[myMeridiem.length() + 1];
	myDate.toCharArray(screenDate, myDate.length()+1);
	myTime.toCharArray(screenTime, myTime.length()+1);
	myMeridiem.toCharArray(screenMeridiem, myMeridiem.length()+1);
	tDate.setText(screenDate);
	tTime.setText(screenTime);
	tAmPm.setText(screenMeridiem);
}

//-------------------------------------------------------
void set_temp_units() {
	String units = read_Variable("/tempUnits.txt");
	varTempUnits = units;
	if (units == "C") {
		tUnit1.setText("C");
		tUnit2.setText("C");
	}
	if (units == "F"){
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

	if (varTempUnits == "F") {
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

	if (ActiveDateBox == 0) { // year
		if (keyPressed < 10) {
			if (newNumber.length() > 4) {
				return;
			}
			newNumber += keyPressed;
		}
		if (keyPressed == 10) {
			newNumber.remove(newNumber.length()-1, 1);
		}
		IntnewYear = newNumber.toInt();
		newNumber.toCharArray(char_array, newNumber.length() + 1);
		nSetYear.setText(char_array);
	}
	if (ActiveDateBox == 1) { // month
		if (keyPressed < 10) {
			if (newNumber.length() > 2) {
				return;
			}
			newNumber += keyPressed;
		}
		if (keyPressed == 10) {
			newNumber.remove(newNumber.length()-1, 1);
		}
		IntnewMonth = newNumber.toInt();
		newNumber.toCharArray(char_array, newNumber.length() + 1);
		nSetMonth.setText(char_array);
	}
	if (ActiveDateBox == 2) { // day
		if (keyPressed < 10) {
			if (newNumber.length() > 2) {
				return;
			}
			newNumber += keyPressed;
		}
		if (keyPressed == 10) {
			newNumber.remove(newNumber.length()-1, 1);
		}
		IntnewDay = newNumber.toInt();
		newNumber.toCharArray(char_array, newNumber.length() + 1);
		nSetDay.setText(char_array);
	}
	if (ActiveDateBox == 3) { // hour
		if (keyPressed < 10) {
			if (newNumber.length() > 2) {
				return;
			}
			newNumber += keyPressed;
		}
		if (keyPressed == 10) {
			newNumber.remove(newNumber.length()-1, 1);
		}
		IntnewHour = newNumber.toInt();
		newNumber.toCharArray(char_array, newNumber.length() + 1);
		nSetHour.setText(char_array);
	}
	if (ActiveDateBox == 4) { // minute
		if (keyPressed < 10) {
			if (newNumber.length() > 2) {
				return;
			}
			newNumber += keyPressed;
		}
		if (keyPressed == 10) {
			newNumber.remove(newNumber.length()-1, 1);
		}
		IntnewMinute = newNumber.toInt();
		newNumber.toCharArray(char_array, newNumber.length() + 1);
		nSetMinute.setText(char_array);
	}
}
