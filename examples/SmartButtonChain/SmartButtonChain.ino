#include <SmartButton.h>

SmartButton button(2);
CommandChain job;
Ack ready;

void sendRequest() {
  Serial.println("1) sent request — waiting for ACK");
}

void afterAck() {
  Serial.println("2) got ACK — running next command");
}

void finished() {
  Serial.println("3) chain finished");
}

void setup() {
  Serial.begin(9600);

  job.then(sendRequest)
     .waitFor(ready)
     .then(afterAck)
     .wait(400)
     .then(finished);

  button.begin().onClick(&job);
  Serial.println("Click the button, then send k to resolve/ACK the next step.");
}

void loop() {
  button.update();

  if (Serial.available() && Serial.read() == 'k') {
    ready.resolve();
  }
}
