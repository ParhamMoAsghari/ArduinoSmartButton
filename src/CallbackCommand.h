#ifndef CALLBACK_COMMAND_H
#define CALLBACK_COMMAND_H

#include "ICommand.h"

class CallbackCommand : public ICommand {
public:
  CallbackCommand(void (*callback)()) : _callback(callback) {}
  void execute() override {
    if (_callback) _callback();
  }

private:
  void (*_callback)();
};

#endif
