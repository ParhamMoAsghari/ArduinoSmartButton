#ifndef COMMAND_CHAIN_H
#define COMMAND_CHAIN_H

#include <Arduino.h>
#include "ICommand.h"

#ifndef SMART_BUTTON_CHAIN_STEPS
#define SMART_BUTTON_CHAIN_STEPS 8
#endif

class Ack {
public:
  Ack() : _resolved(false) {}

  void resolve() { _resolved = true; }
  void ack() { resolve(); }
  void reset() { _resolved = false; }
  bool resolved() const { return _resolved; }

private:
  bool _resolved;
};

class CommandChain : public ICommand {
public:
  CommandChain() : _count(0), _index(0), _running(false) {}

  CommandChain(const CommandChain&) = delete;
  CommandChain& operator=(const CommandChain&) = delete;

  CommandChain& then(void (*callback)()) {
    Step* step = allocStep();
    if (step && callback) {
      step->type = StepFn;
      step->fn = callback;
    }
    return *this;
  }

  CommandChain& then(ICommand* command) {
    Step* step = allocStep();
    if (step && command) {
      step->type = StepCmd;
      step->command = command;
    }
    return *this;
  }

  CommandChain& add(void (*callback)()) { return then(callback); }
  CommandChain& add(ICommand* command) { return then(command); }

  CommandChain& wait(unsigned long ms) {
    Step* step = allocStep();
    if (step) {
      step->type = StepDelay;
      step->ms = ms;
    }
    return *this;
  }

  CommandChain& waitFor(Ack& ack, unsigned long timeoutMs = 0) {
    Step* step = allocStep();
    if (step) {
      step->type = StepAck;
      step->ack = &ack;
      step->ms = timeoutMs;
    }
    return *this;
  }

  CommandChain& waitUntil(bool (*predicate)(), unsigned long timeoutMs = 0) {
    Step* step = allocStep();
    if (step && predicate) {
      step->type = StepPred;
      step->predicate = predicate;
      step->ms = timeoutMs;
    }
    return *this;
  }

  CommandChain& waitUntil(bool& flag, unsigned long timeoutMs = 0) {
    Step* step = allocStep();
    if (step) {
      step->type = StepFlag;
      step->flag = &flag;
      step->ms = timeoutMs;
    }
    return *this;
  }

  CommandChain& waitPin(uint8_t pin, uint8_t level, unsigned long timeoutMs = 0) {
    Step* step = allocStep();
    if (step) {
      step->type = StepPin;
      step->pin = pin;
      step->level = level;
      step->ms = timeoutMs;
    }
    return *this;
  }

  bool busy() const { return _running; }
  bool resolved() const override { return !_running; }
  bool empty() const { return _count == 0; }
  uint8_t count() const { return _count; }

  void execute() override {
    cancel();
    _index = 0;
    _running = _count > 0;
    pump(true);
  }

  void update() override {
    if (!_running) {
      return;
    }
    if (stepReady(false)) {
      _index++;
      pump(true);
    }
  }

  void cancel() override {
    if (_running && _index < _count && _steps[_index].type == StepCmd && _steps[_index].command) {
      _steps[_index].command->cancel();
    }
    _running = false;
    _index = 0;
  }

  void clear() {
    cancel();
    _count = 0;
  }

private:
  enum StepType : uint8_t {
    StepFn,
    StepCmd,
    StepDelay,
    StepAck,
    StepPred,
    StepFlag,
    StepPin
  };

  struct Step {
    StepType type;
    uint8_t pin;
    uint8_t level;
    unsigned long ms;
    unsigned long started;
    union {
      void (*fn)();
      ICommand* command;
      Ack* ack;
      bool* flag;
      bool (*predicate)();
    };

    Step() : type(StepFn), pin(0), level(0), ms(0), started(0), fn(nullptr) {}
  };

  Step* allocStep() {
    if (_count >= SMART_BUTTON_CHAIN_STEPS) {
      return nullptr;
    }
    return &_steps[_count++];
  }

  bool timedOut(const Step& step) const {
    return step.ms > 0 && (millis() - step.started) >= step.ms;
  }

  bool stepReady(bool starting) {
    if (_index >= _count) {
      return true;
    }
    Step& step = _steps[_index];
    if (starting) {
      step.started = millis();
    }

    switch (step.type) {
      case StepFn:
        if (starting && step.fn) {
          step.fn();
        }
        return true;
      case StepCmd:
        if (!step.command) {
          return true;
        }
        if (starting) {
          step.command->execute();
        } else {
          step.command->update();
        }
        return step.command->resolved();
      case StepDelay:
        return (millis() - step.started) >= step.ms;
      case StepAck:
        if (starting && step.ack) {
          step.ack->reset();
        }
        return (step.ack && step.ack->resolved()) || timedOut(step);
      case StepPred:
        return (step.predicate && step.predicate()) || timedOut(step);
      case StepFlag:
        return (step.flag && *step.flag) || timedOut(step);
      case StepPin:
        return digitalRead(step.pin) == step.level || timedOut(step);
    }
    return true;
  }

  void pump(bool starting) {
    while (_running && _index < _count) {
      if (!stepReady(starting)) {
        return;
      }
      _index++;
      starting = true;
    }
    _running = false;
  }

  Step _steps[SMART_BUTTON_CHAIN_STEPS];
  uint8_t _count;
  uint8_t _index;
  bool _running;
};

#endif
