#include "SmartButton.h"

namespace {
  const uint16_t kDefaultDebounceDelay = 50;
  const uint16_t kDefaultLongPressTime = 800;
  const uint16_t kDefaultClickWait = 400;
  const uint16_t kDefaultHoldInterval = 200;
  const uint8_t kDefaultMaxClicks = 3;
  const uint8_t kAbsoluteMaxClicks = 10;
}

SmartButton::SmartButton(uint8_t pin, uint8_t mode)
  : _pin(pin),
    _mode(mode),
    _activeLevel(mode == INPUT_PULLUP ? LOW : HIGH),
    _lastReading(mode == INPUT_PULLUP ? HIGH : LOW),
    _flags(kEnabled),
    _clickCount(0),
    _lastClickCount(0),
    _holdCount(0),
    _maxClicks(kDefaultMaxClicks),
    _togglePin(kUnusedPin),
    _pressPin(kUnusedPin),
    _pressValue(HIGH),
    _releasePin(kUnusedPin),
    _releaseValue(LOW),
    _state(ButtonState::Idle),
    _lastEvent(ButtonEvent::None),
    _eventFlags(0),
    _attached(0),
    _debounceDelay(kDefaultDebounceDelay),
    _longPressTime(kDefaultLongPressTime),
    _clickWaitTime(kDefaultClickWait),
    _holdInterval(kDefaultHoldInterval),
    _lastDebounceTime(0),
    _pressStartTime(0),
    _releaseTime(0),
    _lastHoldTime(0),
    _lastPressDuration(0),
    _toggleFlag(nullptr)
{
  for (uint8_t row = 0; row < kSlotRows; row++) {
    for (uint8_t col = 0; col < SMART_BUTTON_MAX_HANDLERS; col++) {
      _slots[row][col].type = SlotEmpty;
      _slots[row][col].fn = nullptr;
      _slots[row][col].extra = nullptr;
    }
  }
}

void SmartButton::refreshActiveLevel() {
  uint8_t level = (_mode == INPUT_PULLUP) ? LOW : HIGH;
  if (_flags & kInverted) {
    level = (level == LOW) ? HIGH : LOW;
  }
  _activeLevel = level;
}

void SmartButton::configurePins() {
  if (_togglePin != kUnusedPin) {
    pinMode(_togglePin, OUTPUT);
  }
  if (_pressPin != kUnusedPin) {
    pinMode(_pressPin, OUTPUT);
  }
  if (_releasePin != kUnusedPin) {
    pinMode(_releasePin, OUTPUT);
  }
}

SmartButton& SmartButton::begin() {
  pinMode(_pin, _mode);
  configurePins();
  _lastReading = static_cast<uint8_t>(digitalRead(_pin));
  _flags |= kBegun;
  reset();
  return *this;
}

SmartButton& SmartButton::reset() {
  _state = ButtonState::Idle;
  _clickCount = 0;
  _holdCount = 0;
  _flags &= static_cast<uint8_t>(~kLongPress);
  _lastDebounceTime = millis();
  if (_flags & kBegun) {
    _lastReading = static_cast<uint8_t>(digitalRead(_pin));
  }
  return *this;
}

SmartButton& SmartButton::enable() {
  _flags |= kEnabled;
  return *this;
}

SmartButton& SmartButton::disable() {
  _flags &= static_cast<uint8_t>(~kEnabled);
  reset();
  return *this;
}

SmartButton& SmartButton::invert(bool inverted) {
  if (inverted) {
    _flags |= kInverted;
  } else {
    _flags &= static_cast<uint8_t>(~kInverted);
  }
  refreshActiveLevel();
  return *this;
}

SmartButton& SmartButton::debounce(uint16_t ms) {
  _debounceDelay = ms;
  return *this;
}

SmartButton& SmartButton::longPressTime(uint16_t ms) {
  _longPressTime = ms;
  return *this;
}

SmartButton& SmartButton::clickWait(uint16_t ms) {
  _clickWaitTime = ms;
  return *this;
}

SmartButton& SmartButton::holdEvery(uint16_t ms) {
  _holdInterval = ms;
  return *this;
}

SmartButton& SmartButton::maxClicks(uint8_t count) {
  if (count < 1) {
    count = 1;
  } else if (count > kAbsoluteMaxClicks) {
    count = kAbsoluteMaxClicks;
  }
  _maxClicks = count;
  return *this;
}

bool SmartButton::valid(ButtonEvent event) const {
  const uint8_t index = static_cast<uint8_t>(event);
  return index >= static_cast<uint8_t>(ButtonEvent::Press) &&
         index < static_cast<uint8_t>(ButtonEvent::Count);
}

uint8_t SmartButton::eventIndex(ButtonEvent event) const {
  return static_cast<uint8_t>(event) - 1;
}

uint16_t SmartButton::eventBit(ButtonEvent event) const {
  return static_cast<uint16_t>(1) << static_cast<uint8_t>(event);
}

bool SmartButton::has(ButtonEvent event) const {
  return (_attached & eventBit(event)) != 0;
}

void SmartButton::remember(ButtonEvent event) {
  _attached |= eventBit(event);
}

SmartButton::Slot* SmartButton::freeSlot(ButtonEvent event) {
  if (!valid(event)) {
    return nullptr;
  }
  Slot* row = _slots[eventIndex(event)];
  for (uint8_t i = 0; i < SMART_BUTTON_MAX_HANDLERS; i++) {
    if (row[i].type == SlotEmpty) {
      return &row[i];
    }
  }
  return nullptr;
}

SmartButton& SmartButton::on(ButtonEvent event, void (*callback)()) {
  Slot* slot = freeSlot(event);
  if (slot && callback) {
    slot->type = SlotFn;
    slot->fn = callback;
    slot->extra = nullptr;
    remember(event);
  }
  return *this;
}

SmartButton& SmartButton::on(ButtonEvent event, void (*callback)(SmartButton&)) {
  Slot* slot = freeSlot(event);
  if (slot && callback) {
    slot->type = SlotFnButton;
    slot->fnButton = callback;
    slot->extra = nullptr;
    remember(event);
  }
  return *this;
}

SmartButton& SmartButton::on(ButtonEvent event, void (*callback)(void*), void* context) {
  Slot* slot = freeSlot(event);
  if (slot && callback) {
    slot->type = SlotFnContext;
    slot->fnContext = callback;
    slot->extra = context;
    remember(event);
  }
  return *this;
}

SmartButton& SmartButton::on(ButtonEvent event, ICommand* command) {
  Slot* slot = freeSlot(event);
  if (slot && command) {
    slot->type = SlotCommand;
    slot->command = command;
    slot->extra = nullptr;
    remember(event);
  }
  return *this;
}

SmartButton& SmartButton::off(ButtonEvent event) {
  if (!valid(event)) {
    return *this;
  }
  Slot* row = _slots[eventIndex(event)];
  for (uint8_t i = 0; i < SMART_BUTTON_MAX_HANDLERS; i++) {
    if (row[i].type == SlotCommand && row[i].command) {
      row[i].command->cancel();
    }
    row[i].type = SlotEmpty;
    row[i].fn = nullptr;
    row[i].extra = nullptr;
  }
  _attached &= static_cast<uint16_t>(~eventBit(event));
  return *this;
}

SmartButton& SmartButton::off() {
  for (uint8_t event = static_cast<uint8_t>(ButtonEvent::Press);
       event < static_cast<uint8_t>(ButtonEvent::Count);
       event++) {
    off(static_cast<ButtonEvent>(event));
  }
  _flags &= static_cast<uint8_t>(~kAsyncBusy);
  return *this;
}

SmartButton& SmartButton::togglePinOnClick(uint8_t pin) {
  _togglePin = pin;
  if (_flags & kBegun) {
    pinMode(pin, OUTPUT);
  }
  return *this;
}

SmartButton& SmartButton::writePinOnPress(uint8_t pin, uint8_t value) {
  _pressPin = pin;
  _pressValue = value;
  if (_flags & kBegun) {
    pinMode(pin, OUTPUT);
  }
  return *this;
}

SmartButton& SmartButton::writePinOnRelease(uint8_t pin, uint8_t value) {
  _releasePin = pin;
  _releaseValue = value;
  if (_flags & kBegun) {
    pinMode(pin, OUTPUT);
  }
  return *this;
}

SmartButton& SmartButton::pulsePin(uint8_t pin) {
  writePinOnPress(pin, HIGH);
  writePinOnRelease(pin, LOW);
  return *this;
}

SmartButton& SmartButton::toggleOnClick(bool& flag) {
  _toggleFlag = &flag;
  return *this;
}

bool SmartButton::triggered(ButtonEvent event) const {
  if (!valid(event)) {
    return false;
  }
  return (_eventFlags & eventBit(event)) != 0;
}

unsigned long SmartButton::pressedFor() const {
  if (_state != ButtonState::Pressed) {
    return 0;
  }
  return millis() - _pressStartTime;
}

uint8_t SmartButton::clickCount() const {
  if (_state == ButtonState::Idle) {
    return _lastClickCount;
  }
  return _clickCount;
}

uint8_t SmartButton::maxPendingClicks() const {
  uint8_t maxClicks = 1;
  if (has(ButtonEvent::DoubleClick)) {
    maxClicks = 2;
  }
  if (has(ButtonEvent::TripleClick) && maxClicks < 3) {
    maxClicks = 3;
  }
  if (has(ButtonEvent::MultiClick) && maxClicks < _maxClicks) {
    maxClicks = _maxClicks;
  }
  return maxClicks;
}

void SmartButton::fireBuiltins(ButtonEvent event) {
  if (event == ButtonEvent::Click) {
    if (_togglePin != kUnusedPin) {
      digitalWrite(_togglePin, digitalRead(_togglePin) == LOW ? HIGH : LOW);
    }
    if (_toggleFlag) {
      *_toggleFlag = !*_toggleFlag;
    }
  } else if (event == ButtonEvent::Press) {
    if (_pressPin != kUnusedPin) {
      digitalWrite(_pressPin, _pressValue);
    }
  } else if (event == ButtonEvent::Release) {
    if (_releasePin != kUnusedPin) {
      digitalWrite(_releasePin, _releaseValue);
    }
  }
}

void SmartButton::fireSlots(ButtonEvent event) {
  Slot* row = _slots[eventIndex(event)];
  for (uint8_t i = 0; i < SMART_BUTTON_MAX_HANDLERS; i++) {
    Slot& slot = row[i];
    switch (slot.type) {
      case SlotFn:
        slot.fn();
        break;
      case SlotFnButton:
        slot.fnButton(*this);
        break;
      case SlotFnContext:
        slot.fnContext(slot.extra);
        break;
      case SlotCommand:
        slot.command->execute();
        if (!slot.command->resolved()) {
          _flags |= kAsyncBusy;
        }
        break;
      case SlotEmpty:
        return;
    }
  }
}

void SmartButton::emit(ButtonEvent event) {
  _lastEvent = event;
  _flags |= kEventFired;
  _eventFlags |= eventBit(event);
  fireBuiltins(event);
  if (has(event)) {
    fireSlots(event);
  }
}

void SmartButton::tickCommands() {
  bool busy = false;
  for (uint8_t row = 0; row < kSlotRows; row++) {
    for (uint8_t col = 0; col < SMART_BUTTON_MAX_HANDLERS; col++) {
      Slot& slot = _slots[row][col];
      if (slot.type != SlotCommand || !slot.command) {
        continue;
      }
      if (slot.command->resolved()) {
        continue;
      }
      slot.command->update();
      if (!slot.command->resolved()) {
        busy = true;
      }
    }
  }
  if (busy) {
    _flags |= kAsyncBusy;
  } else {
    _flags &= static_cast<uint8_t>(~kAsyncBusy);
  }
}

void SmartButton::handlePress(unsigned long now) {
  _pressStartTime = now;
  _lastHoldTime = now;
  _flags &= static_cast<uint8_t>(~kLongPress);
  _holdCount = 0;
  _clickCount++;
  _state = ButtonState::Pressed;
  emit(ButtonEvent::Press);
}

void SmartButton::handleHold(unsigned long now) {
  if (!(_flags & kLongPress) && (now - _pressStartTime) >= _longPressTime) {
    _flags |= kLongPress;
    _clickCount = 0;
    _lastHoldTime = now;
    emit(ButtonEvent::LongPress);
  }

  if ((_flags & kLongPress) && (now - _lastHoldTime) >= _holdInterval) {
    _lastHoldTime = now;
    _holdCount++;
    emit(ButtonEvent::Hold);
  }
}

void SmartButton::firePendingClicks() {
  _lastClickCount = _clickCount;
  if (_clickCount == 0) {
    return;
  }

  if (_clickCount == 1) {
    emit(ButtonEvent::Click);
  } else if (_clickCount == 2 && has(ButtonEvent::DoubleClick)) {
    emit(ButtonEvent::DoubleClick);
  } else if (_clickCount == 3 && has(ButtonEvent::TripleClick)) {
    emit(ButtonEvent::TripleClick);
  } else if (has(ButtonEvent::MultiClick)) {
    emit(ButtonEvent::MultiClick);
  } else if (_clickCount >= 3 && has(ButtonEvent::TripleClick)) {
    emit(ButtonEvent::TripleClick);
  } else if (has(ButtonEvent::DoubleClick)) {
    emit(ButtonEvent::DoubleClick);
  } else {
    emit(ButtonEvent::Click);
  }

  _clickCount = 0;
}

void SmartButton::handleRelease(unsigned long now) {
  _lastPressDuration = now - _pressStartTime;
  emit(ButtonEvent::Release);

  if (_flags & kLongPress) {
    emit(ButtonEvent::LongRelease);
    _clickCount = 0;
    _holdCount = 0;
    _state = ButtonState::Idle;
    return;
  }

  if (_clickCount < maxPendingClicks()) {
    _releaseTime = now;
    _state = ButtonState::WaitClick;
    return;
  }

  firePendingClicks();
  _state = ButtonState::Idle;
}

bool SmartButton::update() {
  _eventFlags = 0;
  _lastEvent = ButtonEvent::None;
  _flags &= static_cast<uint8_t>(~kEventFired);

  if (!(_flags & kBegun)) {
    begin();
  }

  if (_flags & kEnabled) {
    const unsigned long now = millis();
    const uint8_t reading = static_cast<uint8_t>(digitalRead(_pin));

    if (reading != _lastReading) {
      _lastDebounceTime = now;
      _lastReading = reading;
    } else if ((now - _lastDebounceTime) >= _debounceDelay) {
      const bool pressed = (reading == _activeLevel);

      switch (_state) {
        case ButtonState::Idle:
          if (pressed) {
            handlePress(now);
          }
          break;

        case ButtonState::Pressed:
          if (!pressed) {
            handleRelease(now);
          } else {
            handleHold(now);
          }
          break;

        case ButtonState::WaitClick:
          if (pressed) {
            handlePress(now);
          } else if ((now - _releaseTime) >= _clickWaitTime) {
            firePendingClicks();
            _state = ButtonState::Idle;
          }
          break;
      }
    }
  }

  if (_flags & kAsyncBusy) {
    tickCommands();
  }

  return (_flags & kEventFired) != 0;
}
