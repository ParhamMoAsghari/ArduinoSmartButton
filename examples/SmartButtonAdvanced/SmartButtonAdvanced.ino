#include <SmartButton.h>

SmartButton button(2);

void multiClicked(SmartButton& btn) {
  Serial.print("Clicks: ");
  Serial.println(btn.clickCount());
}

void setup() {
  Serial.begin(9600);

  button.begin()
        .maxClicks(5)
        .onMultiClick(multiClicked)
        .togglePinOnClick(LED_BUILTIN);

  Serial.println("Click the button. Serial: e = enable, d = disable, i = invert");
}

void loop() {
  if (button.update()) {
    if (button.triggered(ButtonEvent::LongPress)) {
      Serial.print("Held for ");
      Serial.print(button.pressedFor());
      Serial.println(" ms");
    }
    if (button.triggered(ButtonEvent::Release)) {
      Serial.print("Released after ");
      Serial.print(button.lastPressDuration());
      Serial.println(" ms");
    }
  }

  if (Serial.available()) {
    const char command = Serial.read();
    if (command == 'd') {
      button.disable();
      Serial.println("Disabled");
    } else if (command == 'e') {
      button.enable();
      Serial.println("Enabled");
    } else if (command == 'i') {
      button.invert(!button.isInverted());
      Serial.println(button.isInverted() ? "Inverted" : "Normal");
    }
  }
}
