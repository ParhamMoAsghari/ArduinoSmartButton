#ifndef SMART_BUTTON_H
#define SMART_BUTTON_H

#include <Arduino.h>
#include "ICommand.h"
#include "CommandChain.h"

#ifndef SMART_BUTTON_MAX_HANDLERS
#define SMART_BUTTON_MAX_HANDLERS 2
#endif

enum class ButtonState : uint8_t {
  Idle,
  Pressed,
  WaitClick
};

enum class ButtonEvent : uint8_t {
  None = 0,
  Press,
  Release,
  Click,
  DoubleClick,
  TripleClick,
  MultiClick,
  LongPress,
  LongRelease,
  Hold,
  Count
};

class SmartButton {
public:
  explicit SmartButton(uint8_t pin, uint8_t mode = INPUT_PULLUP);

  SmartButton(const SmartButton&) = delete;
  SmartButton& operator=(const SmartButton&) = delete;

  SmartButton& begin();
  bool update();
  SmartButton& reset();

  SmartButton& enable();
  SmartButton& disable();
  bool isEnabled() const { return (_flags & kEnabled) != 0; }

  SmartButton& invert(bool inverted = true);
  bool isInverted() const { return (_flags & kInverted) != 0; }

  SmartButton& debounce(uint16_t ms);
  SmartButton& longPressTime(uint16_t ms);
  SmartButton& clickWait(uint16_t ms);
  SmartButton& holdEvery(uint16_t ms);
  SmartButton& maxClicks(uint8_t count);

  uint16_t debounce() const { return _debounceDelay; }
  uint16_t longPressTime() const { return _longPressTime; }
  uint16_t clickWait() const { return _clickWaitTime; }
  uint16_t holdEvery() const { return _holdInterval; }
  uint8_t maxClicks() const { return _maxClicks; }

  SmartButton& onPress(void (*callback)()) { return on(ButtonEvent::Press, callback); }
  SmartButton& onPress(void (*callback)(SmartButton&)) { return on(ButtonEvent::Press, callback); }
  SmartButton& onPress(void (*callback)(void*), void* context) { return on(ButtonEvent::Press, callback, context); }
  SmartButton& onPress(ICommand* command) { return on(ButtonEvent::Press, command); }

  SmartButton& onRelease(void (*callback)()) { return on(ButtonEvent::Release, callback); }
  SmartButton& onRelease(void (*callback)(SmartButton&)) { return on(ButtonEvent::Release, callback); }
  SmartButton& onRelease(void (*callback)(void*), void* context) { return on(ButtonEvent::Release, callback, context); }
  SmartButton& onRelease(ICommand* command) { return on(ButtonEvent::Release, command); }

  SmartButton& onClick(void (*callback)()) { return on(ButtonEvent::Click, callback); }
  SmartButton& onClick(void (*callback)(SmartButton&)) { return on(ButtonEvent::Click, callback); }
  SmartButton& onClick(void (*callback)(void*), void* context) { return on(ButtonEvent::Click, callback, context); }
  SmartButton& onClick(ICommand* command) { return on(ButtonEvent::Click, command); }

  SmartButton& onDoubleClick(void (*callback)()) { return on(ButtonEvent::DoubleClick, callback); }
  SmartButton& onDoubleClick(void (*callback)(SmartButton&)) { return on(ButtonEvent::DoubleClick, callback); }
  SmartButton& onDoubleClick(void (*callback)(void*), void* context) { return on(ButtonEvent::DoubleClick, callback, context); }
  SmartButton& onDoubleClick(ICommand* command) { return on(ButtonEvent::DoubleClick, command); }

  SmartButton& onTripleClick(void (*callback)()) { return on(ButtonEvent::TripleClick, callback); }
  SmartButton& onTripleClick(void (*callback)(SmartButton&)) { return on(ButtonEvent::TripleClick, callback); }
  SmartButton& onTripleClick(void (*callback)(void*), void* context) { return on(ButtonEvent::TripleClick, callback, context); }
  SmartButton& onTripleClick(ICommand* command) { return on(ButtonEvent::TripleClick, command); }

  SmartButton& onMultiClick(void (*callback)()) { return on(ButtonEvent::MultiClick, callback); }
  SmartButton& onMultiClick(void (*callback)(SmartButton&)) { return on(ButtonEvent::MultiClick, callback); }
  SmartButton& onMultiClick(void (*callback)(void*), void* context) { return on(ButtonEvent::MultiClick, callback, context); }
  SmartButton& onMultiClick(ICommand* command) { return on(ButtonEvent::MultiClick, command); }

  SmartButton& onLongPress(void (*callback)()) { return on(ButtonEvent::LongPress, callback); }
  SmartButton& onLongPress(void (*callback)(SmartButton&)) { return on(ButtonEvent::LongPress, callback); }
  SmartButton& onLongPress(void (*callback)(void*), void* context) { return on(ButtonEvent::LongPress, callback, context); }
  SmartButton& onLongPress(ICommand* command) { return on(ButtonEvent::LongPress, command); }

  SmartButton& onLongRelease(void (*callback)()) { return on(ButtonEvent::LongRelease, callback); }
  SmartButton& onLongRelease(void (*callback)(SmartButton&)) { return on(ButtonEvent::LongRelease, callback); }
  SmartButton& onLongRelease(void (*callback)(void*), void* context) { return on(ButtonEvent::LongRelease, callback, context); }
  SmartButton& onLongRelease(ICommand* command) { return on(ButtonEvent::LongRelease, command); }

  SmartButton& onHold(void (*callback)()) { return on(ButtonEvent::Hold, callback); }
  SmartButton& onHold(void (*callback)(SmartButton&)) { return on(ButtonEvent::Hold, callback); }
  SmartButton& onHold(void (*callback)(void*), void* context) { return on(ButtonEvent::Hold, callback, context); }
  SmartButton& onHold(ICommand* command) { return on(ButtonEvent::Hold, command); }

  SmartButton& on(ButtonEvent event, void (*callback)());
  SmartButton& on(ButtonEvent event, void (*callback)(SmartButton&));
  SmartButton& on(ButtonEvent event, void (*callback)(void*), void* context);
  SmartButton& on(ButtonEvent event, ICommand* command);

  SmartButton& off(ButtonEvent event);
  SmartButton& off();

  SmartButton& togglePinOnClick(uint8_t pin);
  SmartButton& writePinOnPress(uint8_t pin, uint8_t value = HIGH);
  SmartButton& writePinOnRelease(uint8_t pin, uint8_t value = LOW);
  SmartButton& pulsePin(uint8_t pin);
  SmartButton& toggleOnClick(bool& flag);

  uint8_t pin() const { return _pin; }
  ButtonState state() const { return _state; }
  ButtonEvent lastEvent() const { return _lastEvent; }
  bool triggered(ButtonEvent event) const;

  bool isPressed() const { return _state == ButtonState::Pressed; }
  bool isReleased() const { return _state != ButtonState::Pressed; }
  bool isIdle() const { return _state == ButtonState::Idle; }
  bool isLongPressed() const { return _state == ButtonState::Pressed && (_flags & kLongPress) != 0; }

  unsigned long pressedFor() const;
  unsigned long lastPressDuration() const { return _lastPressDuration; }
  uint8_t clickCount() const;
  uint8_t holdCount() const { return _holdCount; }

private:
  static const uint8_t kUnusedPin = 0xFF;
  static const uint8_t kBegun = 1 << 0;
  static const uint8_t kEnabled = 1 << 1;
  static const uint8_t kInverted = 1 << 2;
  static const uint8_t kLongPress = 1 << 3;
  static const uint8_t kEventFired = 1 << 4;
  static const uint8_t kAsyncBusy = 1 << 5;

  enum SlotType : uint8_t {
    SlotEmpty = 0,
    SlotFn,
    SlotFnButton,
    SlotFnContext,
    SlotCommand
  };

  struct Slot {
    SlotType type;
    union {
      void (*fn)();
      void (*fnButton)(SmartButton&);
      void (*fnContext)(void*);
      ICommand* command;
    };
    void* extra;
  };

  static const uint8_t kSlotRows = static_cast<uint8_t>(ButtonEvent::Count) - 1;

  uint8_t _pin;
  uint8_t _mode;
  uint8_t _activeLevel;
  uint8_t _lastReading;
  uint8_t _flags;
  uint8_t _clickCount;
  uint8_t _lastClickCount;
  uint8_t _holdCount;
  uint8_t _maxClicks;
  uint8_t _togglePin;
  uint8_t _pressPin;
  uint8_t _pressValue;
  uint8_t _releasePin;
  uint8_t _releaseValue;
  ButtonState _state;
  ButtonEvent _lastEvent;
  uint16_t _eventFlags;
  uint16_t _attached;
  uint16_t _debounceDelay;
  uint16_t _longPressTime;
  uint16_t _clickWaitTime;
  uint16_t _holdInterval;
  unsigned long _lastDebounceTime;
  unsigned long _pressStartTime;
  unsigned long _releaseTime;
  unsigned long _lastHoldTime;
  unsigned long _lastPressDuration;
  bool* _toggleFlag;
  Slot _slots[kSlotRows][SMART_BUTTON_MAX_HANDLERS];

  bool valid(ButtonEvent event) const;
  uint8_t eventIndex(ButtonEvent event) const;
  uint16_t eventBit(ButtonEvent event) const;
  bool has(ButtonEvent event) const;
  Slot* freeSlot(ButtonEvent event);
  void remember(ButtonEvent event);
  void refreshActiveLevel();
  void configurePins();
  void emit(ButtonEvent event);
  void fireSlots(ButtonEvent event);
  void fireBuiltins(ButtonEvent event);
  void tickCommands();
  uint8_t maxPendingClicks() const;
  void handlePress(unsigned long now);
  void handleRelease(unsigned long now);
  void handleHold(unsigned long now);
  void firePendingClicks();
};

#endif
