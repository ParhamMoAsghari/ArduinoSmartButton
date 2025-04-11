#include <SmartButton.h>
#include <CallbackCommand.h>

SmartButton button(2);

void onPress()     { Serial.println("Button Pressed!"); }
void onRelease()   { Serial.println("Button Released!"); }
void onClick()     { Serial.println("Single Click!"); }
void onLongPress() { Serial.println("Long Press!"); }
void onHold()      { Serial.println("Holding..."); }

void setup() {
  Serial.begin(9600);
  button.addPressCommand(new CallbackCommand(onPress));
  button.addReleaseCommand(new CallbackCommand(onRelease));
  button.addClickCommand(new CallbackCommand(onClick));
  button.addLongPressCommand(new CallbackCommand(onLongPress));
  button.addHoldCommand(new CallbackCommand(onHold));
}

void loop() {
  button.update();
}
