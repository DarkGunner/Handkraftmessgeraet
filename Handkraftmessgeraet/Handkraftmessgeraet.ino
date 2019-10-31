/*
 Name:		Handkraftmessgeraet.ino
 Created:	27.10.2019 22:54:56
*/


#include <Adafruit_ST77xx.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_ST7735.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <HX711_Lib.h>


#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)
#define DATA_pin 6
#define SCK_pin 7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Display variables
int controlmenu = 0;
int controlage = 0;
String controlforce = String("0");
String controlmaxforce = String("0");
int controlstrength = 0;

//Test variables
String CALC_max_str;
String CALC_force_str;
int LUTA_strength_int = 2;

//ADC
const byte gainfactor = 32;
float ADC_val_float = 0.0;

//Menu

int MENU_step_int = 1; //Eingabeschritt/ menüschritt 
int MENU_age_int = 25;  //Alter
bool MENU_sex_bool = 1; //Geschlecht
#define button_1 4
#define button_2 5
#define button_3 6
#define button_4 7
#define button_time 45 // *TimeConstant = time, before button will repeat shouting "hey, i was pressed!!"

int MENU_butt_count = 0;
int MENU_butt_select = 0;
int MENU_butt_int = 0;

int TimeConstant = 10; //=delayTime
int TimeIdle = 0;   //timer für standby
int TimeIdleThreshhold = 20000; //Zeit bis Standby-modus in 

HX711_Lib hx711;

// the setup function runs once when you press reset or power the board
void setup() {
	//Display init
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	tft.fillScreen(ST7735_WHITE);
	tft.setRotation(1);
	//text init
	tft.setTextColor(ST7735_RED);
	tft.setTextSize(2);
	tft.setCursor(30, 20);
	tft.print("Herzlich");
	tft.setCursor(23, 45);
	tft.print("Willkommen");
	tft.setCursor(24, 70);
	tft.print("bei MT17A!");
	delay(2000);


	Serial.begin(57600);
	hx711.init(DATA_pin, SCK_pin, gainfactor);
}

// the loop function runs over and over again until power down or reset
void loop() {
	MENU_butt_int = MENU_butt_fcn(); //checks, if any button is pressed

	switch (MENU_step_int)
	{
	case 1:                 //Geschlecht
		if (MENU_butt_int == 2) {
			MENU_sex_bool = 1;       //männlich
			MENU_step_int = 2;    //Nächster Schritt (Alter)
		}
		if (MENU_butt_int == 3) {
			MENU_sex_bool = 0;       //weiblich
			MENU_step_int = 2;    //Nächster Schritt (Alter)
		}
		break;
	case 2:                 //Alter
		if (MENU_butt_int == 2) {
			MENU_age_int = MENU_age_int + 1; //Alter+
		}
		if (MENU_butt_int == 3) {
			MENU_age_int = MENU_age_int - 1; //Alter-
		}
		if (MENU_butt_int == 4) {
			MENU_step_int = 3;      //Nächster Schritt (Messen)
		}
	case 3:                 //Messen
							//-------------------------------------------------------------------------
		if (MENU_butt_int == 1) {
			MENU_step_int = 1;      //zurück zu Einstellungen
		}
		if (MENU_butt_int == 4) {    //maximalkraft zurücksetzen
									 //Levin maxforce = 0;
		}
	}

	//Levin: Display


	//Check Standby Button Press2long OR IDLE/NoForce
	delay(TimeConstant); //10ms
	TimeIdle = TimeIdle + TimeConstant;
	if (~MENU_butt_int)
	{
		TimeIdle = 0;
	}
	if (TimeIdle >= TimeIdleThreshhold) {
		//Levin Standby
	}
}


//display function
uint8_t display(void)
{
	switch (MENU_step_int)                                        //Menu selection
	{
	case 1:                                                     //Age menu
		if (controlmenu != MENU_step_int)                         //control for non complete refresh
		{
			controlmenu = MENU_step_int;                        //set controlmenu = MENU_step_int for non complete refresh

																//Built Menu
			tft.fillRect(0, 0, 160, 128, ST7735_WHITE);
			tft.setTextSize(2);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(20, 15);
			tft.print("Waehlen Sie");
			tft.setCursor(28, 35);
			tft.print("ihr Alter:");

			//Age output
			tft.setTextSize(3);
			tft.setCursor(64, 65);
			tft.print(MENU_age_int);

			//Draw option lines
			tft.drawLine(0, 108, 160, 108, ST7735_RED);
			tft.drawLine(40, 108, 40, 160, ST7735_RED);
			tft.drawLine(80, 108, 80, 160, ST7735_RED);
			tft.drawLine(120, 108, 120, 160, ST7735_RED);

			//Write option text
			tft.setTextSize(2);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(16, 109);
			tft.print("x");
			tft.setCursor(56, 111);
			tft.print("+");
			tft.setCursor(96, 111);
			tft.print("-");
			tft.setCursor(136, 111);
			tft.print("=");
		}

		else                                                        //option for refresh parts of the display
		{
			if (controlage != MENU_age_int)
			{
				controlage = MENU_age_int;
				tft.fillRect(60, 65, 60, 30, ST7735_WHITE);
				tft.setTextSize(3);                                   //Age output
				tft.setCursor(64, 65);
				tft.print(MENU_age_int);
			}
		}
		break;

	case 2:                                                       //Sex selection
		if (controlmenu != MENU_step_int)                           //control for non complete refresh
		{
			controlmenu = MENU_step_int;                          //set controlmenu for non complete refresh
			tft.fillRect(0, 0, 160, 128, ST7735_WHITE);
			tft.setTextSize(2);
			tft.setTextColor(ST7735_BLACK);

			//Menu text
			tft.setCursor(2, 15);
			tft.print("Bitte waehlen");
			tft.setCursor(33, 40);
			tft.print("Sie das");
			tft.setCursor(18, 65);
			tft.print("Geschlecht:");

			//Draw lines
			tft.drawLine(0, 108, 160, 108, ST7735_RED);
			tft.drawLine(40, 108, 40, 160, ST7735_RED);
			tft.drawLine(80, 108, 80, 160, ST7735_RED);
			tft.drawLine(120, 108, 120, 160, ST7735_RED);

			//Write option text
			tft.setTextSize(2);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(16, 109);
			tft.print("x");
			tft.setCursor(56, 111);
			tft.print("M");
			tft.setCursor(96, 111);
			tft.print("W");
			tft.setCursor(136, 111);
			tft.print("=");
		}
		break;

	case 3:                                                      //force measurement
		if (controlmenu != MENU_step_int)                          //control for non complete refresh
		{
			controlmenu = MENU_step_int;                         //set controlmenu = MENU_step_int for non complete refresh

																 //MENU text 
			tft.fillRect(0, 0, 160, 128, ST7735_WHITE);
			tft.setTextSize(2);
			tft.setTextColor(ST7735_RED);
			tft.setCursor(5, 5);
			tft.print("Handkraft-");
			tft.setCursor(5, 20);
			tft.print("messung [kg]");
			tft.setTextSize(2);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(5, 40);
			tft.print("max:");
			tft.setCursor(5, 60);
			tft.print(CALC_max_str);                             //Write maximum handforce
			tft.setTextSize(2);
			tft.setCursor(5, 89);
			tft.print("mom:");
			tft.setTextSize(3);
			tft.setCursor(75, 82);
			tft.print(CALC_force_str);                           //Write current handforce

			switch (LUTA_strength_int)                                //strength
			{
			case 1:                                                //strong
				tft.setTextSize(2);
				tft.setCursor(75, 60);
				tft.print("Stark");
				tft.setTextSize(2);
				break;
			case 2:                                                //normal
				tft.setTextSize(2);
				tft.setCursor(75, 60);
				tft.print("Normal");
				tft.setTextSize(2);
				break;
			case 3:                                                //weak
				tft.setTextSize(2);
				tft.setCursor(75, 60);
				tft.print("Schwach");
				tft.setTextSize(2);
				break;
			}

			//Draw lines
			tft.drawLine(0, 108, 160, 108, ST7735_RED);
			tft.drawLine(40, 108, 40, 160, ST7735_RED);
			tft.drawLine(80, 108, 80, 160, ST7735_RED);
			tft.drawLine(120, 108, 120, 160, ST7735_RED);

			//Write select text
			tft.setTextSize(2);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(16, 109);
			tft.print("x");
			tft.setCursor(136, 111);
			tft.print("=");
		}

		else                                                        //option for refresh parts of the display
		{
			if (CALC_max_str != controlmaxforce)                     //refresh maximum handforce
			{
				controlmaxforce = CALC_max_str;
				tft.fillRect(5, 60, 30, 20, ST7735_WHITE);
				tft.setTextSize(2);
				tft.setCursor(5, 60);
				tft.print(CALC_max_str);
			}

			if (CALC_force_str != controlforce)                      //refresh current handforce
			{
				controlforce = CALC_force_str;
				tft.fillRect(75, 82, 45, 25, ST7735_WHITE);
				tft.setTextSize(3);
				tft.setCursor(75, 82);
				tft.print(CALC_force_str);
			}

			if (LUTA_strength_int != controlstrength)                //refresh strength
			{
				controlstrength = LUTA_strength_int;
				tft.fillRect(75, 60, 60, 20, ST7735_WHITE);

				switch (LUTA_strength_int)
				{
				case 1:                                             //strong
					tft.setTextSize(2);
					tft.setCursor(75, 60);
					tft.print("Stark");
					tft.setTextSize(2);
					break;
				case 2:                                             //normal
					tft.setTextSize(2);
					tft.setCursor(75, 60);
					tft.print("Normal");
					tft.setTextSize(2);
					break;
				case 3:                                             //weak
					tft.setTextSize(2);
					tft.setCursor(75, 60);
					tft.print("Schwach");
					tft.setTextSize(2);
					break;
				}
			}
		}
		break;
	}
}

//return => Kraft-Offset
float inCalc() {
	float AD_StartValue_float;
	//---read AD, write in var
	return AD_StartValue_float;
}

float AD_StartValue_float = inCalc();

//AD_float => AD Spannungswert in mV
//return => Kraftwert
float calc(float AD_float) {
	AD_float = AD_float - AD_StartValue_float;
	//Wert angleichen, Bsp. 35mV pro Newton
	int Force_float = AD_float / 35;
	//---Wert runden
	return Force_float;
}


//Kilogramm in Pfund umrechnen
float calcLb(float Force_float) {
	float ForceLb_float = Force_float * 2.20462;
	//---Wert runden
	return ForceLb_float;
}


//Force_float, Kraftwert zwischen 0 und 90.0
//Gender_int 0 => Male, 1 => Female
//return 0 => weak, 1 => normal, 2 => strong
int cmpr(float Force_float, int Age_int, int Gender_int) {
	if (Gender_int == 0) {
		if (Age_int >= 10 && Age_int <= 34) {
			if (Age_int >= 10 && Age_int <= 11) {
				if (Force_float < 12.6) {
					return 0;
				}
				else if (Force_float >= 12.6 && Force_float <= 22.4) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 12 && Age_int <= 13) {
				if (Force_float < 19.4) {
					return 0;
				}
				else if (Force_float >= 19.4 && Force_float <= 31.2) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 14 && Age_int <= 15) {
				if (Force_float < 28.5) {
					return 0;
				}
				else if (Force_float >= 28.5 && Force_float <= 44.3) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 16 && Age_int <= 17) {
				if (Force_float < 32.6) {
					return 0;
				}
				else if (Force_float >= 32.6 && Force_float <= 52.4) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 18 && Age_int <= 19) {
				if (Force_float < 35.7) {
					return 0;
				}
				else if (Force_float >= 35.7 && Force_float <= 55.5) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 20 && Age_int <= 24) {
				if (Force_float < 36.8) {
					return 0;
				}
				else if (Force_float >= 36.8 && Force_float <= 56.6) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 25 && Age_int <= 29) {
				if (Force_float < 37.7) {
					return 0;
				}
				else if (Force_float >= 37.7 && Force_float <= 57.5) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 30 && Age_int <= 34) {
				if (Force_float < 36.0) {
					return 0;
				}
				else if (Force_float >= 36.0 && Force_float <= 55.8) {
					return 1;
				}
				else {
					return 2;
				}
			}
		}
		else if (Age_int >= 35 && Age_int <= 99) {
			if (Age_int >= 35 && Age_int <= 39) {
				if (Force_float < 35.8) {
					return 0;
				}
				else if (Force_float >= 35.8 && Force_float <= 55.6) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 40 && Age_int <= 44) {
				if (Force_float < 35.5) {
					return 0;
				}
				else if (Force_float >= 35.5 && Force_float <= 55.3) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 45 && Age_int <= 49) {
				if (Force_float < 34.7) {
					return 0;
				}
				else if (Force_float >= 34.7 && Force_float <= 54.5) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 50 && Age_int <= 54) {
				if (Force_float < 32.9) {
					return 0;
				}
				else if (Force_float >= 32.9 && Force_float <= 50.7) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 55 && Age_int <= 59) {
				if (Force_float < 30.7) {
					return 0;
				}
				else if (Force_float >= 30.7 && Force_float <= 48.5) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 60 && Age_int <= 64) {
				if (Force_float < 30.2) {
					return 0;
				}
				else if (Force_float >= 30.2 && Force_float <= 48.0) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 65 && Age_int <= 69) {
				if (Force_float < 28.2) {
					return 0;
				}
				else if (Force_float >= 28.2 && Force_float <= 44.0) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 70 && Age_int <= 99) {
				if (Force_float < 21.3) {
					return 0;
				}
				else if (Force_float >= 21.3 && Force_float <= 35.1) {
					return 1;
				}
				else {
					return 2;
				}
			}
		}
	}
	else if (Gender_int == 1) {
		if (Age_int >= 10 && Age_int <= 34) {
			if (Age_int >= 10 && Age_int <= 11) {
				if (Force_float < 11.8) {
					return 0;
				}
				else if (Force_float >= 11.8 && Force_float <= 21.6) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 12 && Age_int <= 13) {
				if (Force_float < 14.6) {
					return 0;
				}
				else if (Force_float >= 14.6 && Force_float <= 24.4) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 14 && Age_int <= 15) {
				if (Force_float < 15.5) {
					return 0;
				}
				else if (Force_float >= 15.5 && Force_float <= 27.3) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 16 && Age_int <= 17) {
				if (Force_float < 17.2) {
					return 0;
				}
				else if (Force_float >= 17.2 && Force_float <= 29.0) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 18 && Age_int <= 19) {
				if (Force_float < 19.2) {
					return 0;
				}
				else if (Force_float >= 19.2 && Force_float <= 31.0) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 20 && Age_int <= 24) {
				if (Force_float < 21.5) {
					return 0;
				}
				else if (Force_float >= 21.5 && Force_float <= 35.3) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 25 && Age_int <= 29) {
				if (Force_float < 25.6) {
					return 0;
				}
				else if (Force_float >= 25.6 && Force_float <= 41.4) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 30 && Age_int <= 34) {
				if (Force_float < 21.5) {
					return 0;
				}
				else if (Force_float >= 21.5 && Force_float <= 35.3) {
					return 1;
				}
				else {
					return 2;
				}
			}
		}
		else if (Age_int >= 35 && Age_int <= 99) {
			if (Age_int >= 35 && Age_int <= 39) {
				if (Force_float < 20.3) {
					return 0;
				}
				else if (Force_float >= 20.3 && Force_float <= 34.1) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 40 && Age_int <= 44) {
				if (Force_float < 18.9) {
					return 0;
				}
				else if (Force_float >= 18.9 && Force_float <= 32.7) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 45 && Age_int <= 49) {
				if (Force_float < 18.6) {
					return 0;
				}
				else if (Force_float >= 18.6 && Force_float <= 32.4) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 50 && Age_int <= 54) {
				if (Force_float < 18.1) {
					return 0;
				}
				else if (Force_float >= 18.1 && Force_float <= 31.9) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 55 && Age_int <= 59) {
				if (Force_float < 17.7) {
					return 0;
				}
				else if (Force_float >= 17.7 && Force_float <= 31.5) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 60 && Age_int <= 64) {
				if (Force_float < 17.2) {
					return 0;
				}
				else if (Force_float >= 17.2 && Force_float <= 31.0) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 65 && Age_int <= 69) {
				if (Force_float < 15.4) {
					return 0;
				}
				else if (Force_float >= 15.4 && Force_float <= 27.2) {
					return 1;
				}
				else {
					return 2;
				}
			}
			else if (Age_int >= 70 && Age_int <= 99) {
				if (Force_float < 14.7) {
					return 0;
				}
				else if (Force_float >= 14.7 && Force_float <= 24.5) {
					return 1;
				}
				else {
					return 2;
				}
			}
		}
	}
}

String roundForce(float Force_float) {
	char Temp_char[4] = "";
	dtostrf(Force_float, 1, 2, Temp_char);
	String LUTA_strength_string = Temp_char;
	return LUTA_strength_string;
}

int MENU_butt_fcn()
{
	switch (MENU_butt_select)
	{
	case 0:
		MENU_butt_count = 1;
		if (~digitalRead(button_1))
		{
			MENU_butt_select = 1;
		}
		else if (~digitalRead(button_2))
		{
			MENU_butt_select = 2;
		}
		else if (~digitalRead(button_3))
		{
			MENU_butt_select = 3;
		}
		else if (~digitalRead(button_4))
		{
			MENU_butt_select = 4;
		}
		else
		{
			MENU_butt_count = 0;
		}
		break;
	case 1:
		if (~digitalRead(button_1))
		{
			MENU_butt_count += 1;
		}
		else
		{
			MENU_butt_select = 0;
		}
		break;
	case 2:
		if (~digitalRead(button_2))
		{
			MENU_butt_count += 1;
		}
		else
		{
			MENU_butt_select = 0;
		}
		break;
	case 3:
		if (~digitalRead(button_3))
		{
			MENU_butt_count += 1;
		}
		else
		{
			MENU_butt_select = 0;
		}
		break;
	case 4:
		if (~digitalRead(button_4))
		{
			MENU_butt_count += 1;
		}
		else
		{
			MENU_butt_select = 0;
		}
		break;
	}

	if (MENU_butt_count == 1)
	{
		return(MENU_butt_select);
	}
	else if (MENU_butt_count > button_time)
	{
		MENU_butt_count = 1;
		return(MENU_butt_select);
	}
	else
	{
		return(0);
	}

}
