#include "SmartButton.h"
#include <Arduino.h>

#define DEBOUNCE_DELAY 80
#define LONG_PRESS_TIME 800

SmartButton::SmartButton(uint8_t pin)
  : _pin(pin),
    _state(BUTTON_IDLE),
    _lastDebounceTime(0),
    _pressStartTime(0),
    _lastHoldTime(0),
    _lastButtonState(HIGH),
    _buttonState(HIGH),
    _clickCount(0),
    _pressCommand(nullptr),
    _releaseCommand(nullptr),
    _clickCommand(nullptr),
    _longPressCommand(nullptr),
    _holdCommand(nullptr)
{
  pinMode(pin, INPUT_PULLUP);
}

void SmartButton::addPressCommand(ICommand* command) {
  _pressCommand = command;
}

void SmartButton::addReleaseCommand(ICommand* command) {
  _releaseCommand = command;
}

void SmartButton::addClickCommand(ICommand* command) {
  _clickCommand = command;
}

void SmartButton::addLongPressCommand(ICommand* command) {
  _longPressCommand = command;
}

void SmartButton::addHoldCommand(ICommand* command) {
  _holdCommand = command;
}

void SmartButton::update() {
  int reading = digitalRead(_pin);
  unsigned long currentTime = millis();

  switch (_state) {
    case BUTTON_IDLE:
      if (reading != _lastButtonState) {
        _lastDebounceTime = currentTime;
        changeState(BUTTON_DEBOUNCE);
      }
      break;

    case BUTTON_DEBOUNCE:
      if ((currentTime - _lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading == LOW) {
          _pressStartTime = currentTime;
          _clickCount++;
          changeState(BUTTON_PRESSED);
          if (_pressCommand) _pressCommand->execute();
        } else {
          changeState(BUTTON_IDLE);
        }
      }
      break;

    case BUTTON_PRESSED:
      if (reading == HIGH) {
        if (_releaseCommand) _releaseCommand->execute();
        if ((currentTime - _pressStartTime) >= LONG_PRESS_TIME) {
          if (_longPressCommand) _longPressCommand->execute();
        }
        if (_clickCommand) _clickCommand->execute();
        changeState(BUTTON_IDLE);
      } else {
        if ((currentTime - _pressStartTime) >= LONG_PRESS_TIME) {
          if (_holdCommand && (currentTime - _lastHoldTime > 200)) {
            _lastHoldTime = currentTime;
            _holdCommand->execute();
          }
        }
      }
      break;
	  
    default:
      break;
  }

  _lastButtonState = reading;
}

void SmartButton::changeState(ButtonState newState) {
  _state = newState;
}
