### Module design architecture

---

## 1. Toolchain & Environment

All embedded projects in this team use **PlatformIO** as the build system and project manager, integrated with **VS Code**.

**Why PlatformIO:**
- Dependency management via `platformio.ini` — no manual library installation.
- Full IntelliSense support through the PlatformIO IDE extension.
- Easy multi-environment support for targeting different boards or build flags.
- Shared libraries can be placed in a common `lib/` directory and referenced across projects.

All projects use the **Arduino framework** as the hardware abstraction layer. The C++ standard should be set to **C++17** or later in `platformio.ini` to enable modern language features used throughout this architecture.

---

## 2. Project Structure

Every project follows this directory layout:

```
project_root/
├── platformio.ini
├── include/
│   ├── Pins.h
│   ├── Hardware.h
│   ├── Network.h
│   ├── NetworkHandlers.h
│   ├── Configs/
│   │   ├── HardwareConfig.h
│   │   ├── NetworkConfig.h
│   │   └── CANConfig.h
│   └── States/
│       ├── HardwareState.h
│       ├── HardwareCommandState.h
│       ├── HardwareFeedbackState.h
│       └── NetworkState.h
└── src/
    ├── main.cpp
    ├── Hardware.cpp
    ├── Network.cpp
    └── NetworkHandlers.cpp
```

The `include/` directory is split into two logical subdirectories: `Configs/` and `States/`. These reflect the two fundamental data categories described in Section 3.

---

## 3. The Config–State Dichotomy

The central architectural principle of this codebase is a strict separation between **configuration** and **state**.

### 3.1 Configurations — `include/Configs/`

Configurations are **compile-time constants** that describe fixed properties of the system: hardware parameters, network addresses, timing constants, and protocol identifiers. They never change at runtime.

**Rules:**
- Defined as `constexpr` values inside a `namespace`.
- Grouped by domain: one file per subsystem (Hardware, Network, CAN).
- No mutable data. No instances. No constructors.
- Include `<cstdint>` for portable integer types.

**Naming convention:** `XxxConfig` namespace inside `XxxConfig.h`.

```
Configs/
├── HardwareConfig.h   — ADC references, baud rates, sensor calibration constants
├── NetworkConfig.h    — IP/MAC addresses, MQTT topics, timeouts, payload limits
└── CANConfig.h        — CAN baud rate, message IDs, frame definitions
```

Every constant that would otherwise be a magic number or a `#define` belongs in a Config file. `#define` macros are **not used** for configuration values — `constexpr` is always preferred.

### 3.2 States — `include/States/`

States are **runtime data containers** — structs that hold the live values of the system as it executes. They are owned by `main.cpp` and passed by reference to any function that needs them.

**Rules:**
- Defined as plain `struct`s with a constructor that sets safe default values.
- No business logic. No methods beyond initialization.
- Include `<cstdint>` for portable integer types.
- Bit-fields (`uint8_t field : 1`) may be used where multiple binary flags are grouped together.

**Naming convention:** `XxxState` struct inside `XxxState.h`.

```
States/
├── HardwareState.h          — Live sensor driver objects (ammeters, thermometers, LEDs, etc.)
├── HardwareCommandState.h   — Inbound commands decoded from the network (what actuators should do)
├── HardwareFeedbackState.h  — Outbound sensor readings to be reported over the network
└── NetworkState.h           — Network client objects and connection bookkeeping
```

The distinction is important: a `Config` tells the system *how it is built*; a `State` tells the system *what is happening right now*.

---

## 4. State Ownership and Dependency Injection

All state structs are **instantiated once, in `main.cpp`**, at global scope. They are initialized via their default constructors.

Functions that need to read or modify state receive it as a **reference argument**. No state struct is declared as a global inside a `.cpp` module — doing so would hide dependencies and make the data flow opaque.

```
main.cpp owns: NetworkState (ns), HardwareState (hs), HardwareCommandState (hcs), HardwareFeedbackState (hfs)
         │
         ├─ passes ns, hcs  → Network::initNetwork()
         ├─ passes hfs      → Network::sendFeedbackMessage()
         └─ passes hs       → Hardware::initHardware()
```

This is a lightweight form of **dependency injection**: modules declare what data they need through their function signatures, rather than reaching for hidden globals.

The one accepted exception is when a callback function has a signature dictated by a third-party library and cannot accept additional arguments. In that case, a module-local static pointer may be set during initialization to give the callback access to the required state. This pattern must be documented at the point of use.
#### 4.1 Example: 
```cpp
//HardwareFeedbackState.h
...
struct HardwareFeedbackState
{

	...
	uint16_t BAT_1_ADC;
	
	uint16_t BAT_2_ADC;
	
	...

HardwareFeedbackState() :
	BAT_1_ADC(0), BAT_2_ADC(0), ... {}

};
```
```cpp
// Network.h
namespace Network
{
	...
	void sendFeedbackMessage(const HardwareFeedbackState &hfs)
	...
}
```

In this example, `HardwareFeedbackState` struct (which holds the feedback data to be sent to the base) is initialized in `main.cpp` via the default constructor, later passed **as reference** into the `sendFeedbackMessage`.

``` cpp
// main.cpp
...
struct HardwareFeedbackState hfs {};
...
void loop()
{
	...
	Network::sendFeedbackMessage(hfs);
	...
}
```

Then, it is passed as one of the arguments into `feedbackEncHandler` of the `NetworkHandlers` namespace.

```cpp
// Network.cpp
...
void Network::sendFeedbackMessage(const HardwareFeedbackState &hfs)

{
	...
	NetworkHandlers::feedbackEncHandler(doc, *ns_, hfs); // ns_- reference to a local copy of NetworkState struct, read the Network.cpp comments or docs [TODO]
}
...
```

And later, it's used to access the HardwareFeedbackState's fields to send feedback data in a JSON document.

```cpp
// NetworkHandlers.cpp
...
void NetworkHandlers::feedbackEncHandler(
	StaticJsonDocument<NetworkConfig::MAX_JSON_PAYLOAD> &doc,
	struct NetworkState &ns,
	const struct HardwareFeedbackState &hfs
)
{
	...
	auto payload = doc.createNestedObject("payload");
	payload["TEMP"] = hfs.TEMP;
	...
}
```

---

## 5. Module Design

Each functional subsystem (Hardware, Network, CAN) consists of a `.h`/`.cpp` pair under `include/` and `src/`.

**Rules:**
- Exposed functions are grouped in a `namespace` (e.g., `namespace Hardware`, `namespace Network`).
- Header files declare only the public interface. Implementation details stay in `.cpp`.
- Modules do not hold their own state structs — they receive state **by reference**.
- Module-internal helper functions are declared `static` (file-local linkage) or placed in an anonymous `namespace` in the `.cpp` file.

### Standard modules in every project

| Module | Responsibility |
|---|---|
| `Pins` | Pin number constants and `init_pins()`. Header-only. |
| `Hardware` | Sensor/actuator initialization and periodic read/write logic. |
| `Network` | Ethernet + MQTT lifecycle: init, reconnect loop, send/receive. |
| `NetworkHandlers` | JSON encode/decode logic, decoupled from transport. |
| `CAN` *(planned)* | CAN bus initialization, frame dispatch, receive handling. |

---

## 6. `Pins.h` — Pin Definitions

All GPIO assignments live in a single `namespace Pins` in `include/Pins.h`. The file also contains the `init_pins()` inline function that configures all pin modes and sets safe initial output states.

**Rules:**
- All pin numbers are `constexpr uint8_t`.
- Comments next to each pin indicate what it connects to and whether the assignment has been verified (`+`) or is provisional (`-`).
- Pins that are electrically unsafe on the target platform (e.g., bootstrap pins on ESP32) are called out explicitly in a comment at the top of the file.
- `init_pins()` must default all output pins to a **safe/off state** before any other initialization runs.

---

## 7. `main.cpp` Structure

`main.cpp` follows a strict structure:

1. **Includes** — Arduino framework, then module headers.
2. **State declarations** — all state structs declared at global scope using default constructors.
3. **`setup()`** — serial init, then `Pins::init_pins()`, then module `init` calls in dependency order.
4. **`loop()`** — timed tasks driven by `millis()` comparisons. No blocking delays. Each periodic task tracks its own `lastXxx` timestamp as a `static` local variable.

The loop handles at minimum:
- A **watchdog check** on the time elapsed since the last valid MQTT command, triggering a safe-stop action on timeout.
- `Network::handleNetwork()` for MQTT reconnection and message dispatch.
- A **feedback publish** at a fixed interval using `Network::sendFeedbackMessage()`.
- CAN handling *(to be added)*.

---

## 8. C++ Style Conventions

| Practice | Rule |
|---|---|
| Integer types | Always use `<cstdint>` types: `uint8_t`, `uint16_t`, `uint32_t`, etc. Never plain `int` for hardware registers or protocol fields. |
| Constants | `constexpr` in a namespace. Never `#define`. |
| Namespaces | Used for all module interfaces and all config blocks. |
| Structs | Used for all state containers. POD-style with an explicit constructor setting defaults. |
| Bit-fields | Acceptable for packing multiple boolean flags into one byte (e.g., command states). |
| `static` locals | Used for `lastXxx` timestamps in `loop()` and for module-internal helpers in `.cpp` files. |
| `nullptr` | Always used instead of `NULL` or `0` for pointer comparisons. |
| Inline functions | Acceptable in headers for short utility functions (e.g., `init_pins()`). |
| `Serial.println()` | Used for all diagnostic output. Every message is prefixed with a bracketed subsystem tag, e.g., `[MQTT]`, `[ETHERNET]`, `[WATCHDOG]`. |

---

## 9. Config and State Reference

### Config files

| File               | Contents                                                                                                                                                           |
| ------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `HardwareConfig.h` | Specific hardware configurations, for example: ADC reference voltage, mV-per-ampere calibration, ADC resolution, serial baud rate etc.                             |
| `NetworkConfig.h`  | Network configurations: MAC address, static IP, MQTT broker IP and port, topic names, payload size limit, safety and reconnect timeouts, feedback publish interval |
| `CANConfig.h`      | CAN baud rate, message IDs, frame field offsets                                                                                                                    |

### State files

| File | Key fields |
|---|---|
| `HardwareState.h` | Instantiated sensor driver objects (ammeter, thermometer, NeoPixel, etc.) — one per connected device |
| `HardwareCommandState.h` | One field per controllable actuator, set from incoming MQTT commands. Boolean flags use bit-fields. |
| `HardwareFeedbackState.h` | One field per sensor reading to be reported. Populated by `Hardware` logic, consumed by `Network`. |
| `NetworkState.h` | Ethernet client, MQTT client, IP address object, timestamp of last valid command |

---

## 10. Adding a New Module

When a new subsystem is introduced (e.g., CAN bus, a new sensor type):

1. Add pin constants to `Pins.h` and configure them in `init_pins()`.
2. Add compile-time parameters to the appropriate `Configs/` file, or create a new one (`CANConfig.h`).
3. Add runtime fields to the appropriate `States/` struct, or create a new one.
4. Create a `ModuleName.h` / `ModuleName.cpp` pair following the namespace-based module pattern.
5. Instantiate required state structs in `main.cpp` and pass them to the new module's `init` function from `setup()`.
6. Add periodic calls to `loop()` as needed, with `millis()`-based timing.

No existing files should need restructuring to accommodate a new module. The architecture is designed to grow by addition, not by modification of the core pattern.
