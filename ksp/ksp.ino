#include "KerbalSimpit.h"

KerbalSimpit ksp(Serial);

const int stageButtonPin = 2;
const int stageArmTumblerPin = 4;
const int gearsTumblerPin = 7;
const int brakesTumblerPin = 8;

int stageButtonState;
int lastStageButtonState = LOW;
int lastGearsState = HIGH; // In most cases gears are down by default.
int lastBrakesState = HIGH; // Same goes for brakes.

bool isStageArmed = false;
bool isGearDown = true;
bool isBrakesToggled = true;

unsigned long lastStageButtonDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup()
{
    Serial.begin(115200); // Must match baud setting in KSP plugin!
    
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(stageButtonPin, INPUT);
    pinMode(stageArmTumblerPin, INPUT);
    pinMode(gearsTumblerPin, INPUT);
    pinMode(brakesTumblerPin, INPUT);
    
    Connect();
}

void Connect()
{
    digitalWrite(LED_BUILTIN, LOW);
    while (!ksp.init())
    {
        delay(100); // There is nothing to do until Arduino connects to plugin.
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    Stage();
    Gears();
    Brakes();
}

void Stage()
{
    int armReading = digitalRead(stageArmTumblerPin);
    isStageArmed = armReading == HIGH;

    if (!isStageArmed)
        return;
    
    int stageReading = digitalRead(stageButtonPin);

    if (stageReading != lastStageButtonState)
    {
        lastStageButtonDebounceTime = millis();
    }
    
    if ((millis() - lastStageButtonDebounceTime) > debounceDelay)
    {
        if (stageButtonState != stageReading)
        {
            stageButtonState = stageReading;

            if (isStageArmed && stageButtonState == HIGH)
                ksp.activateAction(STAGE_ACTION);
        }
    }

    lastStageButtonState = stageReading;
}

void Gears()
{
    int gearsReading = digitalRead(gearsTumblerPin);
    
    if (lastGearsState == gearsReading)
        return;

    lastGearsState = gearsReading;
    isGearDown = gearsReading == HIGH;

    if (isGearDown)
    {
        ksp.activateAction(GEAR_ACTION);
    }
    else
    {
        ksp.deactivateAction(GEAR_ACTION);
    }
}

void Brakes()
{
    int brakesReading = digitalRead(brakesTumblerPin);

    if (lastBrakesState == brakesReading)
        return;
        
    lastBrakesState = brakesReading;
    isBrakesToggled = brakesReading == HIGH;
    
    if (isBrakesToggled)
    {
        ksp.activateAction(BRAKES_ACTION);
    }
    else
    {
        ksp.deactivateAction(BRAKES_ACTION);
    }
}
