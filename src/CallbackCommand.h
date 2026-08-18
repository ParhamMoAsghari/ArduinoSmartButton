#ifndef CALLBACK_COMMAND_H
#define CALLBACK_COMMAND_H

#include "ICommand.h"

class SmartButton;

class CallbackCommand : public ICommand {
public:
  explicit CallbackCommand(void (*callback)())
    : _callback(callback),
      _buttonCallback(nullptr),
      _contextCallback(nullptr),
      _button(nullptr),
      _context(nullptr) {}

  CallbackCommand(void (*callback)(SmartButton&), SmartButton* button)
    : _callback(nullptr),
      _buttonCallback(callback),
      _contextCallback(nullptr),
      _button(button),
      _context(nullptr) {}

  CallbackCommand(void (*callback)(void*), void* context)
    : _callback(nullptr),
      _buttonCallback(nullptr),
      _contextCallback(callback),
      _button(nullptr),
      _context(context) {}

  void execute() override {
    if (_callback) {
      _callback();
    } else if (_buttonCallback && _button) {
      _buttonCallback(*_button);
    } else if (_contextCallback) {
      _contextCallback(_context);
    }
  }

private:
  void (*_callback)();
  void (*_buttonCallback)(SmartButton&);
  void (*_contextCallback)(void*);
  SmartButton* _button;
  void* _context;
};

#endif
