# SmartButton

A non-blocking Arduino button library. You attach handlers, call `update()` in `loop()`, and it never uses `delay()`.

Repository: [https://github.com/ParhamMoAsghari/ArduinoSmartButton](https://github.com/ParhamMoAsghari/ArduinoSmartButton)

Wire a momentary button between pin 2 and GND.

```cpp
#include <SmartButton.h>

SmartButton button(2);

void setup() {
  Serial.begin(9600);
  button.begin()
        .onClick([] { Serial.println("click"); })
        .onDoubleClick([] { Serial.println("double"); })
        .onLongPress([] { Serial.println("long"); })
        .togglePinOnClick(LED_BUILTIN);
}

void loop() {
  button.update();
}
```

If your board does not accept lambdas, use a normal function: `button.onClick(clicked);`

## Everyday API

Call `onClick` again to add another independent handler. They all start when the event fires.

```cpp
button.begin()
      .debounce(50)
      .longPressTime(800)
      .onClick(toggleLed)
      .onClick(logClick)
      .onLongPress(powerOff);
```

| I want to... | Do this |
| --- | --- |
| Handle a click | `onClick(fn)` |
| Handle double / triple / many clicks | `onDoubleClick` / `onTripleClick` / `onMultiClick` |
| Handle hold | `onLongPress(fn)` then `onHold(fn)` |
| Toggle an LED | `togglePinOnClick(LED_BUILTIN)` |
| LED on while pressed | `pulsePin(LED_BUILTIN)` |
| Flip a `bool` | `toggleOnClick(armed)` |
| Read the button in `loop()` | `if (button.update() && button.triggered(ButtonEvent::Click))` |
| Pause the button | `disable()` / `enable()` |
| Remove handlers | `off(ButtonEvent::Click)` or `off()` |

Handlers can take the button, so you do not need globals:

```cpp
void clicked(SmartButton& btn) {
  Serial.println(btn.clickCount());
  Serial.println(btn.lastPressDuration());
}

button.onMultiClick(clicked);
```

## Sequential chains (wait for ACK, then next)

A `CommandChain` runs **one step at a time**. It starts the next command only after the current one resolves — a callback finishes immediately, a wait finishes when you ACK it, a flag becomes true, a pin matches, or a timeout elapses. This stays non-blocking; `update()` polls the current step.

```cpp
Ack ready;

CommandChain job;
job.then(sendRequest)
   .waitFor(ready)
   .then(handleReply)
   .wait(250)
   .then(finish);

button.onClick(&job);

void loop() {
  button.update();
  if (gotSerialOk()) {
    ready.resolve();
  }
}
```

Other waits:

```cpp
job.waitUntil(doneFlag);          // bool&
job.waitUntil(isReady);           // bool isReady()
job.waitPin(4, LOW);              // wait until pin reads LOW
job.waitFor(ready, 1000);         // ACK or 1 s timeout, then continue
```

Keep calling `button.update()` while the chain is waiting. A new click restarts the chain from the first step. Keep `CommandChain` and `Ack` in global/static storage (no `new`). See `examples/SmartButtonChain`.

## Timing

```cpp
button.debounce(50);      // ignore bounce shorter than this
button.longPressTime(800);
button.clickWait(400);    // wait for a possible second click
button.holdEvery(200);    // repeat while held, after long press
button.maxClicks(5);      // for onMultiClick
```

Active-high button (no internal pull-up): `SmartButton button(2, INPUT);`  
Flip the active level later with `button.invert();`

## Events

| Event | When |
| --- | --- |
| Press / Release | Debounced edges |
| Click | One short press (not after a long press) |
| DoubleClick / TripleClick | 2 or 3 short clicks |
| MultiClick | N clicks when a more specific handler is not set |
| LongPress | Once, after 800 ms while still held |
| LongRelease | Let go after a long press |
| Hold | Every 200 ms after long press |

If you attach double/triple/multi-click handlers, a single click waits for `clickWait()` so the library can tell them apart.

Callbacks are stored in a small fixed table (default 2 per event). `update()` only polls a command chain while one is waiting. Pin helpers do not allocate. Put `CommandChain` in global/static storage.

## Installation

**GitHub:** [ParhamMoAsghari/ArduinoSmartButton](https://github.com/ParhamMoAsghari/ArduinoSmartButton)

```
git clone https://github.com/ParhamMoAsghari/ArduinoSmartButton.git
```

Copy the folder to `Documents/Arduino/libraries/SmartButton/`, or in the IDE use **Sketch > Include Library > Add .ZIP Library...** with [this ZIP](https://github.com/ParhamMoAsghari/ArduinoSmartButton/archive/refs/heads/main.zip).

## Design

The public API is a **facade**: `onClick()` is Observer-style and **fluent** (`begin().onClick(fn)`).

Underneath, that still uses:

- **Command** — `ICommand` / `CallbackCommand` for custom actions
- **Sequential chain** — `CommandChain` runs one step, waits until it resolves/ACKs, then starts the next
- **Adapter** — function pointers, `SmartButton&` callbacks, `void*` context, and `ICommand*` all attach the same way
- **State** — a non-blocking FSM with millis() debounce

Write your own `ICommand` with `execute()`, `resolved()`, and optional `update()` if a step needs more than a function plus `waitFor`.

## License

MIT
