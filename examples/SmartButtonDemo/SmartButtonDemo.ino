#include <SmartButton.h>

SmartButton button(2);

void clicked() {
  Serial.println("Click");
}

void doubled() {
  Serial.println("Double click");
}

void held(SmartButton& btn) {
  Serial.print("Long press (");
  Serial.print(btn.pressedFor());
  Serial.println(" ms)");
}

void setup() {
  Serial.begin(9600);

  button.begin()
        .onClick(clicked)
        .onDoubleClick(doubled)
        .onLongPress(held)
        .togglePinOnClick(LED_BUILTIN);
}

void loop() {
  button.update();
}
