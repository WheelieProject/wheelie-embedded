#include "Wheelie.h"

enum State { BOOT_STATE, ACTIVE_STATE };
void setupLowPowerWakeMode();
void setState(State newState);
void sleep();

SYSTEM_THREAD(ENABLED);

AssetTracker t;
State state = BOOT_STATE;
unsigned long lastMovementTimestampReading = 0;


void setup() {
	Serial.begin(9600);
	t.begin();
}

void loop() {

	if(state == BOOT_STATE && Particle.connected()){
		setState(ACTIVE_STATE);
		setupLowPowerWakeMode();
	}

	if(state == ACTIVE_STATE){
		unsigned long now = millis();
		int movement = t.readXYZmagnitude();

		if (movement > ACCELERATION_THRESHOLD) {
			Serial.println(String::format("Movement detected: %d", movement));
			lastMovementTimestampReading = now;
		}

		if(now - lastMovementTimestampReading > AWAKE_TIMEOUT_MS){
			Serial.printlnf("Power up timeout: %d", millis());
			sleep();
			Serial.printlnf("Woke up %d", millis());
		}
	}
}


void setupLowPowerWakeMode() {
	Serial.println("Resetting accelerometer");
	const uint8_t movementThreshold = 16;
	if (!t.setupLowPowerWakeMode(movementThreshold)) {
		Serial.println("Accelerometer not found");
	}
}


void sleep()	{
	setupLowPowerWakeMode();
	delay(500);
	// Sleep
	System.sleep(WKP, RISING, 0);
	// This delay should not be necessary, but sometimes things don't seem to work right
	// immediately coming out of sleep.
	delay(5000);
	int awake = ((t.clearAccelInterrupt() & LIS3DH_INT1_SRC_IA) != 0);
	Serial.printlnf("wokeUp=%d", awake);
}


void setState(State newState){
	Serial.println(newState);
	state = newState;
}
