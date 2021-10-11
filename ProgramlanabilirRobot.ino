/*
 Name:		ProgramlanabilirRobot.ino
 Created:	3/1/2017 11:00:28 PM
 Author:	strz35
 Program butonuna basılınca PROGRAMLAMA moduna girer ve 
 yön tuşlarına her basmayı kaydeder. Tekrar program butonu
 ile PROGRAMLAMA modundan çıkar. Start butonuna basıldığına
 hafızasındaki hareketleri uygular.
 Hareketleri EEPROM' kaydettiği için silinmes kapatıp açsanızda
 hafızada kalır. Yeniden PROGRAM moduna girdiğinizde eski programı 
 siler.

 BUTON          ARDUINO
 =============  =======
 START BUTONU     2
 PROGRAM BUTONU   3
 İLERİ BUTONU     7
 GERİ BUTONU      4
 SOL BUTONU       6
 SAĞ BUTONU       5

 MOTOR BAĞLANTILARI (MOTOR SÜRÜRCÜ L298 VEYA L293)
 L298          ARDUINO
 ====          =======
 IN1              8
 IN2              9
 ENA              10
 IN3              12
 IN4              13
 ENB              11
*/

#include <TimerOne.h>
#include <EEPROM.h>

#define BUTON_START		2   //start butonu
#define BUTON_PROGRAM	3   //program butonu
#define BUTON_GERI		4   //yön tuşları
#define BUTON_SAG		  5
#define BUTON_SOL		  6
#define BUTON_ILERI		7

// motor sürücü bağlantıları (L298 veya L293 için tasarlanmıştır.)
#define MOTOR_SAG_ILERI	8
#define MOTOR_SAG_GERI	9
#define MOTOR_SAG_PWM	10

#define MOTOR_SOL_ILERI	12
#define MOTOR_SOL_GERI	13
#define MOTOR_SOL_PWM	11

#define HAREKET_YOK		0
#define HAREKET_ILERI	1
#define HAREKET_GERI	2
#define HAREKET_SOLA	3
#define HAREKET_SAGA	4

#define MOTOR_BOS		0
#define MOTOR_ILERI		1
#define MOTOR_GERI		2
#define MOTOR_FREN		3

#define MOTOR_CALISTIR	0
#define MOTOR_DURDUR	1

#define HAREKET_SURE	1000

#define ROBOT_MOD_BOS	0
#define ROBOT_MOD_START	1
#define ROBOT_MOD_PROGRAM 2

const uint8_t YON_BUTONLARI[] = { BUTON_ILERI, BUTON_GERI, BUTON_SOL, BUTON_SAG};
const uint8_t YONLER[] = { HAREKET_ILERI, HAREKET_GERI, HAREKET_SOLA, HAREKET_SAGA };

unsigned int eepAdr = 0;
uint8_t robotMod = ROBOT_MOD_BOS;


void setup() {
	pinMode(BUTON_START, INPUT_PULLUP);
	pinMode(BUTON_PROGRAM, INPUT_PULLUP);
	pinMode(BUTON_GERI, INPUT_PULLUP);
	pinMode(BUTON_SAG, INPUT_PULLUP);
	pinMode(BUTON_SOL, INPUT_PULLUP);
	pinMode(BUTON_ILERI, INPUT_PULLUP);

	pinMode(MOTOR_SAG_ILERI, OUTPUT);
	pinMode(MOTOR_SAG_GERI, OUTPUT);
	pinMode(MOTOR_SAG_PWM, OUTPUT);

	pinMode(MOTOR_SOL_ILERI, OUTPUT);
	pinMode(MOTOR_SOL_GERI, OUTPUT);
	pinMode(MOTOR_SOL_PWM, OUTPUT);

	for (unsigned int i = 0;i < EEPROM.length() ;i++) {
		EEPROM.write(i, HAREKET_YOK);
	}

	attachInterrupt(0, startRobot, FALLING);
	attachInterrupt(1, programRobot, FALLING);
	Timer1.initialize(500000);
	Timer1.attachInterrupt(flashLed, 500000);
}


void loop() {
	if (robotMod == ROBOT_MOD_PROGRAM) {
		motorKontrol(MOTOR_DURDUR);
		sagMotorYonu(MOTOR_BOS);
		solMotorYonu(MOTOR_BOS);
		//robot programlama kodu
		if (eepAdr > 0) {		//eepromu temizle
			for (unsigned int i = 0;i < eepAdr;i++) {
				EEPROM.write(i, HAREKET_YOK);
			}
		}
		while (robotMod == ROBOT_MOD_PROGRAM)
		{
			for (uint8_t i = 0;i < 4;i++) {
				uint8_t pinButon = YON_BUTONLARI[i];
				if (digitalRead(pinButon) == LOW) {			//butona bas�ld� m�
					while (digitalRead(pinButon) == LOW);	//butonun b�rak�lmas�n� bekle
					EEPROM.write(eepAdr, YONLER[i]);
					eepAdr++;
				}
			}
		}
		robotMod = ROBOT_MOD_BOS;
	}

	if (robotMod == ROBOT_MOD_START) {
		if (eepAdr > 0) {
			boolean calisma = true;
			unsigned int epromAd = 0;
			while (calisma)
			{
				uint8_t hareket = EEPROM.read(epromAd);
				robotuCalistir(hareket);
				epromAd++;
				if (epromAd == eepAdr)
					calisma = false;
			}
			robotMod = ROBOT_MOD_BOS;
			
		}
	}
}

void flashLed() {
	if(robotMod == ROBOT_MOD_PROGRAM)
		digitalWrite(13, digitalRead(13) ^ 1);
}

void programRobot() {
	delay(500);
	if (robotMod != ROBOT_MOD_PROGRAM)
		robotMod = ROBOT_MOD_PROGRAM;
	else
		robotMod = ROBOT_MOD_BOS;
}

void startRobot() {
	delay(500);
	if (robotMod != ROBOT_MOD_PROGRAM)
		robotMod = ROBOT_MOD_START;
}

void robotuCalistir(uint8_t hareket) {
	switch (hareket)
	{
	case HAREKET_ILERI:
		robotIleri(HAREKET_SURE);
		break;
	case HAREKET_GERI:
		robotGeri(HAREKET_SURE);
		break;
	case HAREKET_SOLA:
		robotSola(HAREKET_SURE);
		break;
	case HAREKET_SAGA:
		robotSaga(HAREKET_SURE);
		break;
	default:	
		break;
	}
}

void robotIleri(unsigned int sure) {
	solMotorYonu(MOTOR_ILERI);
	sagMotorYonu(MOTOR_ILERI);
	motorKontrol(MOTOR_CALISTIR);
	delay(sure);
	motorKontrol(MOTOR_DURDUR);
}

void robotGeri(unsigned int sure) {
	solMotorYonu(MOTOR_GERI);
	sagMotorYonu(MOTOR_GERI);
	motorKontrol(MOTOR_CALISTIR);
	delay(sure);
	motorKontrol(MOTOR_DURDUR);
}

void robotSola(unsigned int sure) {
	solMotorYonu(MOTOR_GERI);
	sagMotorYonu(MOTOR_ILERI);
	motorKontrol(MOTOR_CALISTIR);
	delay(sure);
	motorKontrol(MOTOR_DURDUR);
}

void robotSaga(unsigned int sure) {
	solMotorYonu(MOTOR_ILERI);
	sagMotorYonu(MOTOR_GERI);
	motorKontrol(MOTOR_CALISTIR);
	delay(sure);
	motorKontrol(MOTOR_DURDUR);
}

void solMotorYonu(uint8_t yon) {
	switch (yon)
	{
	case MOTOR_BOS:
		digitalWrite(MOTOR_SOL_ILERI, LOW);
		digitalWrite(MOTOR_SOL_GERI, LOW);
		break;
	case MOTOR_ILERI:
		digitalWrite(MOTOR_SOL_ILERI, HIGH);
		digitalWrite(MOTOR_SOL_GERI, LOW);
		break;
	case MOTOR_GERI:
		digitalWrite(MOTOR_SOL_ILERI, LOW);
		digitalWrite(MOTOR_SOL_GERI, HIGH);
		break;
	case MOTOR_FREN:
		digitalWrite(MOTOR_SOL_ILERI, HIGH);
		digitalWrite(MOTOR_SOL_GERI, HIGH);
		break;
	default:
		digitalWrite(MOTOR_SOL_ILERI, LOW);
		digitalWrite(MOTOR_SOL_GERI, LOW);
		break;
	}
}

void sagMotorYonu(uint8_t yon) {
	switch (yon)
	{
	case MOTOR_BOS:
		digitalWrite(MOTOR_SAG_ILERI, LOW);
		digitalWrite(MOTOR_SAG_GERI, LOW);
		break;
	case MOTOR_ILERI:
		digitalWrite(MOTOR_SAG_ILERI, HIGH);
		digitalWrite(MOTOR_SAG_GERI, LOW);
		break;
	case MOTOR_GERI:
		digitalWrite(MOTOR_SAG_ILERI, LOW);
		digitalWrite(MOTOR_SAG_GERI, HIGH);
		break;
	case MOTOR_FREN:
		digitalWrite(MOTOR_SAG_ILERI, HIGH);
		digitalWrite(MOTOR_SAG_GERI, HIGH);
		break;
	default:
		digitalWrite(MOTOR_SAG_ILERI, LOW);
		digitalWrite(MOTOR_SAG_GERI, LOW);
		break;
	}
}

void motorKontrol(uint8_t motorDurum) {
	if (motorDurum == MOTOR_CALISTIR) {
		digitalWrite(MOTOR_SAG_PWM, HIGH);
		digitalWrite(MOTOR_SOL_PWM, HIGH);
	}
	else
	{
		digitalWrite(MOTOR_SAG_PWM, LOW);
		digitalWrite(MOTOR_SOL_PWM, LOW);
	}
}
