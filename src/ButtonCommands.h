#ifndef BUTTON_COMMANDS_H
#define BUTTON_COMMANDS_H

#include <Arduino.h>
#include "ICommand.h"

class WritePinCommand : public ICommand {
public:
  WritePinCommand(uint8_t pin, uint8_t value)
    : _pin(pin), _value(value), _ready(false) {}

  void execute() override {
    if (!_ready) {
      pinMode(_pin, OUTPUT);
      _ready = true;
    }
    digitalWrite(_pin, _value);
  }

private:
  uint8_t _pin;
  uint8_t _value;
  bool _ready;
};

class TogglePinCommand : public ICommand {
public:
  explicit TogglePinCommand(uint8_t pin) : _pin(pin), _ready(false) {}

  void execute() override {
    if (!_ready) {
      pinMode(_pin, OUTPUT);
      _ready = true;
    }
    digitalWrite(_pin, digitalRead(_pin) == LOW ? HIGH : LOW);
  }

private:
  uint8_t _pin;
  bool _ready;
};

class ToggleCommand : public ICommand {
public:
  explicit ToggleCommand(bool& target) : _target(target) {}

  void execute() override {
    _target = !_target;
  }

private:
  bool& _target;
};

#endif
