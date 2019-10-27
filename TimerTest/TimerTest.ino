//Test functions

void hi() {
	Serial.println("hi");
}

void bye() {
	Serial.println("bye");
}

void stackCheck() {
	Serial.println("StackCheck gang");
}

void memCheck() {
	Serial.println("Memcheck gang");
}


//TIMERS
typedef void (*voidFuncPtr)(void);// Create a type to point to a funciton.
struct timerElement {
  voidFuncPtr func;
  int time;
};


timerElement timerElementss[5] = {{hi, 1000}, {bye, 500}, {stackCheck, 2000}, {memCheck, 2500}};
const int timerElements = 5;

long prevMillis[timerElements]; //auto sets to 0

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
	//delay(100);
	unsigned long currentMillis = millis();
	for (int i=0; i<timerElements-1; i++) {
		// Serial.println(timerElementss[i].time);
		if (currentMillis-prevMillis[i] >= timerElementss[i].time) {
			prevMillis[i] = currentMillis;
			timerElementss[i].func(); //call function
		}
	}
}