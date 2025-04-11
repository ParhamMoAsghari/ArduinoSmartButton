#ifndef SMART_BUTTON_H
#define SMART_BUTTON_H

#include <Arduino.h>
#include "ICommand.h"

enum ButtonState {
  BUTTON_IDLE,
  BUTTON_DEBOUNCE,
  BUTTON_PRESSED,
};

class SmartButton {
public:
  SmartButton(uint8_t pin);

  void update();

  // Command Setters
  void addPressCommand(ICommand* command);
  void addReleaseCommand(ICommand* command);
  void addClickCommand(ICommand* command);
  void addLongPressCommand(ICommand* command);
  void addHoldCommand(ICommand* command);

private:
  uint8_t _pin;
  ButtonState _state;

  // Timers and states for button press detection
  unsigned long _lastDebounceTime;
  unsigned long _pressStartTime;
  unsigned long _lastHoldTime;

  // Button states and tracking
  int _lastButtonState;
  int _buttonState;
  int _clickCount;

  // Command pointers for different actions
  ICommand* _pressCommand;
  ICommand* _releaseCommand;
  ICommand* _clickCommand;
  ICommand* _longPressCommand;
  ICommand* _holdCommand;

  // Internal method to change button state
  void changeState(ButtonState newState);
};

#endif
