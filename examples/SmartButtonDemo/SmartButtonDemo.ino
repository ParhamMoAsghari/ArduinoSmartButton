# SmartButton

An advanced, event-driven button debouncing library for Arduino. Built with a non-blocking FSM (finite state machine), it supports **press**, **release**, **click**, **long press**, **hold**, and **custom callback commands** using the Command design pattern.

## 🔧 Features

- Fully non-blocking with `millis()`-based timing
- Clean OOP architecture using design patterns
- Event-based: press, release, click, long press, and hold
- Easy command binding with `ICommand` interface
- Works with internal pull-up resistors (`INPUT_PULLUP`)
- Designed for responsiveness and modularity

## 📦 Installation

**Option 1:**  
Clone/download and extract to:  
`Documents/Arduino/libraries/SmartButton/`

**Option 2:**  
Install `.zip` via Arduino IDE:  
`Sketch > Include Library > Add .ZIP Library...`

## 🚀 Getting Started

### Minimal Example

```cpp
#include <SmartButton.h>
#include <CallbackCommand.h>

SmartButton button(2); // Button on pin 2

void onClick() {
  Serial.println("Clicked!");
}

void setup() {
  Serial.begin(9600);
  button.addClickCommand(new CallbackCommand(onClick));
}

void loop() {
  button.update();
}
```

### Full Example

```cpp
#include <SmartButton.h>
#include <CallbackCommand.h>

SmartButton button(2);

void onPress()     { Serial.println("Button Pressed!"); }
void onRelease()   { Serial.println("Button Released!"); }
void onClick()     { Serial.println("Single Click!"); }
void onLongPress() { Serial.println("Long Press!"); }
void onHold()      { Serial.println("Holding..."); }

void setup() {
  Serial.begin(9600);
  button.addPressCommand(new CallbackCommand(onPress));
  button.addReleaseCommand(new CallbackCommand(onRelease));
  button.addClickCommand(new CallbackCommand(onClick));
  button.addLongPressCommand(new CallbackCommand(onLongPress));
  button.addHoldCommand(new CallbackCommand(onHold));
}

void loop() {
  button.update();
}
```

## 🧠 Internals

- **FSM (Finite State Machine)** for clean and reliable transitions
- **Debounce** delay: `80ms`
- **Long press** threshold: `800ms`
- **Hold** events fire every `200ms` after long press

## 📁 File Structure

```
SmartButton/
├── src/
│   ├── SmartButton.h / .cpp         # Core logic
│   ├── ICommand.h                   # Command interface
│   └── CallbackCommand.h            # Function pointer adapter
├── examples/
│   └── SmartButtonDemo/
│       └── SmartButtonDemo.ino
├── library.properties
├── README.md
└── LICENSE
```

## 🧩 API Summary

```cpp
SmartButton(uint8_t pin);
void update(); // Call in loop()

// Command setters
void addPressCommand(ICommand* cmd);
void addReleaseCommand(ICommand* cmd);
void addClickCommand(ICommand* cmd);
void addLongPressCommand(ICommand* cmd);
void addHoldCommand(ICommand* cmd);
```

## 🛠 Dependencies

- Arduino core (`Arduino.h`)
- No external libraries required

## 📄 License

MIT License — free to use and modify.