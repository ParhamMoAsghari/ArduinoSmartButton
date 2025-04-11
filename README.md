# SmartButton

An advanced, event-driven button debouncing library for Arduino. It uses a non-blocking FSM (finite state machine) and supports **click**, **double click**, **long press**, and **custom callback commands** using the Command design pattern.

## 🔧 Features

- Clean, modular design with OOP principles
- Fully non-blocking: perfect for responsive applications
- Customizable callback support via `ICommand`
- FSM-based internal logic for robust state handling
- Click / Double Click / Long Press / Hold support
- `examples/SmartButtonDemo.ino` provided

## 📦 Installation

**Option 1**: Download and extract to `Documents/Arduino/libraries/SmartButton/`

**Option 2**: Add to Arduino IDE using `.zip` file via:
```
Sketch > Include Library > Add .ZIP Library...
```

## 🚀 Getting Started

```cpp
#include <SmartButton.h>
#include <CallbackCommand.h>

SmartButton button(2);

void onClick() {
  Serial.println("Clicked!");
}

void setup() {
  Serial.begin(9600);
  button.setClickCommand(new CallbackCommand(onClick));
}

void loop() {
  button.update();
}
```

## 📁 File Structure

```
SmartButton/
├── src/
│   ├── SmartButton.h / .cpp
│   ├── ICommand.h
│   └── CallbackCommand.h
├── examples/
│   └── SmartButtonDemo/
│       └── SmartButtonDemo.ino
├── library.properties
├── README.md
└── LICENSE
```

## 🧠 Powered By

- Finite State Machines (FSM)
- Command Design Pattern
- Time-based debouncing (non-blocking `millis()` logic)

## 📄 License

MIT
