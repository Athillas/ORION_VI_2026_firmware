### Architektura projektowania modułów

---

## 1. Zestaw narzędzi i środowisko

Wszystkie projekty embedded w tym zespole wykorzystują **PlatformIO** jako system budowania i menedżer projektu, zintegrowany z **VS Code**.

**Dlaczego PlatformIO:**

- Zarządzanie zależnościami przez `platformio.ini` — brak konieczności ręcznej instalacji bibliotek.
- Pełne wsparcie IntelliSense poprzez rozszerzenie PlatformIO IDE.
- Łatwa obsługa wielu środowisk (multi-environment) dla różnych płytek lub flag kompilacji.
- Współdzielone biblioteki mogą być umieszczone w wspólnym katalogu `lib/` i przywoływane w różnych projektach.

Wszystkie projekty używają **framework Arduino** jako hardware abstraction layer. Standard C++ powinien być ustawiony na **C++17** lub nowszy w `platformio.ini`, aby umożliwić korzystanie z nowoczesnych funkcji języka używanych w tej architekturze.

---

## 2. Struktura Projektu

Każdy projekt zachowuje następujący układ katalogów:

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

Katalog `include/` jest podzielony na dwa logiczne podkatalogi: `Configs/` i `States/`. Odzwierciedlają one dwie fundamentalne kategorie danych opisane w Sekcji 3.

---

## 3. Dychotomia Konfiguracja–Stan

Główną zasadą architektoniczną tego kodu jest ścisła separacja między **konfiguracją** (configuration) a **stanem** (state).

### 3.1 Konfiguracje — `include/Configs/`

Konfiguracje to **stałe czasu kompilacji** (compile-time constants), które opisują niezmienne właściwości systemu: parametry sprzętowe, adresy sieciowe, stałe czasowe i identyfikatory protokołów. Nigdy nie zmieniają się w czasie pracy programu (runtime).

**Zasady:**

- Definiowane jako wartości `constexpr` wewnątrz `namespace`.
- Grupowane według domen: jeden plik na subsystem (Hardware, Network, CAN).
- Brak danych zmiennych (mutable). Brak instancji. Brak konstruktorów.
- Należy dołączyć `<cstdint>` dla przenośnych typów całkowitych.


**Konwencja nazewnictwa:** `namespace XxxConfig` wewnątrz pliku `XxxConfig.h`.

```
Configs/
├── HardwareConfig.h   — referencje ADC, baud rate, stałe kalibracji czujników
├── NetworkConfig.h    — adresy IP/MAC, tematy MQTT, timeouty, limity payloadu
└── CANConfig.h        — CAN baud rate, ID wiadomości, definicje ramek
```

Każda stała, która w innym przypadku byłaby "magic number" lub makrem `#define`, należy do pliku Config. Makra `#define` **nie są używane** dla wartości konfiguracyjnych — `constexpr` jest zawsze preferowane.

### 3.2 Stany — `include/States/`

Stany to **kontenery danych czasu wykonania** (runtime data containers) — struktury `struct`, które przechowują bieżące wartości systemu podczas jego pracy. Są one własnością `main.cpp` i są przekazywane przez referencję do każdej funkcji, która ich potrzebuje.

**Zasady:**

- Definiowane jako zwykłe `struct` z konstruktorem ustawiającym bezpieczne wartości domyślne.
- Brak logiki biznesowej. Brak metod poza inicjalizacją.
- Należy dołączyć `<cstdint>` dla przenośnych typów całkowitych.
- Pola bitowe (`uint8_t field : 1`) mogą być używane do grupowania wielu flag binarnych.

**Konwencja nazewnictwa:** `struct XxxState` wewnątrz pliku `XxxState.h`.

```
States/
├── HardwareState.h          — obiekty driverów czujników live (amperomierze, termometry, LEDy itp.)
├── HardwareCommandState.h   — przychodzące polecenia zdekodowane z sieci (co mają robić aktuatory)
├── HardwareFeedbackState.h  — wychodzące odczyty czujników do raportowania przez sieć
└── NetworkState.h           — obiekty klientów sieciowych i buchalteria połączeń
```

Rozróżnienie jest istotne: `Config` mówi systemowi _jak jest zbudowany_; `State` mówi systemowi _co się dzieje w tej chwili_.

---

## 4. Własność stanu i wstrzykiwanie zależności (Dependency Injection)

Wszystkie instancje `struct` stanu są **tworzone raz, w `main.cpp`**, w zasięgu globalnym. Są inicjalizowane poprzez ich domyślne konstruktory.

Funkcje, które muszą odczytywać lub modyfikować stan, otrzymują go jako **argument przez referencję**. Żaden `struct` stanu nie jest deklarowany jako globalny wewnątrz modułu `.cpp` — robienie tego ukrywałoby zależności i czyniło przepływ danych niejasnym.

```
main.cpp posiada: NetworkState (ns), HardwareState (hs), HardwareCommandState (hcs), HardwareFeedbackState (hfs)
         │
         ├─ przekazuje ns, hcs  → Network::initNetwork()
         ├─ przekazuje hfs      → Network::sendFeedbackMessage()
         └─ przekazuje hs       → Hardware::initHardware()
```

Jest to lekka forma **wstrzykiwania zależności** (dependency injection): moduły deklarują, jakich danych potrzebują poprzez sygnatury swoich funkcji, zamiast sięgać po ukryte zmienne globalne.

Jedynym akceptowalnym wyjątkiem jest sytuacja, gdy funkcja callback ma sygnaturę narzuconą przez zewnętrzną bibliotekę i nie może przyjąć dodatkowych argumentów. W takim przypadku lokalny dla modułu wskaźnik `static` może zostać ustawiony podczas inicjalizacji, aby dać callbackowi dostęp do wymaganego stanu. Ten wzorzec musi być udokumentowany w miejscu użycia.

#### 4.1 Przykład:

C++

```
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

C++

```
// Network.h
namespace Network
{
	...
	void sendFeedbackMessage(const HardwareFeedbackState &hfs)
	...
}
```

W tym przykładzie `struct HardwareFeedbackState` (który przechowuje dane zwrotne do wysłania do bazy) jest inicjalizowany w `main.cpp` przez domyślny konstruktor, a następnie przekazywany **przez referencję** do `sendFeedbackMessage`.

C++

```
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

Następnie jest przekazywany jako jeden z argumentów do `feedbackEncHandler` w `namespace NetworkHandlers`.

C++

```
// Network.cpp
...
void Network::sendFeedbackMessage(const HardwareFeedbackState &hfs)

{
	...
	NetworkHandlers::feedbackEncHandler(doc, *ns_, hfs); // ns_- referencja do lokalnej kopii struct NetworkState
}
...
```

A później jest używany do dostępu do pól `HardwareFeedbackState` w celu wysłania danych zwrotnych w dokumencie JSON.

C++

```
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

## 5. Projektowanie Modułów

Każdy subsystem funkcjonalny (Hardware, Network, CAN) składa się z pary plików `.h`/`.cpp` w katalogach `include/` oraz `src/`.

**Zasady:**

- Eksponowane funkcje są pogrupowane w `namespace` (np. `namespace Hardware`, `namespace Network`).
- Pliki nagłówkowe deklarują tylko publiczny interfejs. Szczegóły implementacji pozostają w `.cpp`.
- Moduły nie przechowują własnych `struct` stanu — otrzymują stan **przez referencję**.
- Funkcje pomocnicze wewnątrz modułu są deklarowane jako `static` (file-local linkage) lub umieszczane w anonimowym `namespace` w pliku `.cpp`.

### Standardowe moduły w każdym projekcie

|**Moduł**|**Odpowiedzialność**|
|---|---|
|`Pins`|Stałe numerów pinów i `init_pins()`. Header-only.|
|`Hardware`|Inicjalizacja czujników/aktuatorów i okresowa logika odczytu/zapisu.|
|`Network`|Cykl życia Ethernet + MQTT: inicjalizacja, pętla rekoneksji, wysyłanie/odbieranie.|
|`NetworkHandlers`|Logika kodowania/dekodowania JSON, odseparowana od transportu.|
|`CAN` _(planowane)_|Inicjalizacja magistrali CAN, wysyłka ramek, obsługa odbioru.|

---

## 6. `Pins.h` — Definicje Pinów

Wszystkie przypisania GPIO znajdują się w jednym `namespace Pins` w `include/Pins.h`. Plik ten zawiera również funkcję `inline init_pins()`, która konfiguruje tryby wszystkich pinów i ustawia bezpieczne początkowe stany wyjściowe.

**Zasady:**

- Wszystkie numery pinów to `constexpr uint8_t`.
- Komentarze obok każdego pinu wskazują, do czego jest podłączony i czy przypisanie zostało zweryfikowane (`+`) czy jest tymczasowe (`-`).
- Piny, które są elektrycznie niebezpieczne na danej platformie (np. piny bootstrap w ESP32), są wyraźnie opisane w komentarzu na górze pliku.
- `init_pins()` musi ustawić wszystkie piny wyjściowe na **bezpieczny stan wyłączony** przed uruchomieniem jakiejkolwiek innej inicjalizacji.
---

## 7. Struktura `main.cpp`

`main.cpp` zachowuje ścisłą strukturę:

1. **Includes** — framework Arduino, następnie nagłówki modułów.
2. **Deklaracje stanów** — wszystkie `struct` stanu zadeklarowane w zasięgu globalnym przy użyciu domyślnych konstruktorów.
3. **`setup()`** — inicjalizacja Serial, następnie `Pins::init_pins()`, potem wywołania `init` modułów w kolejności zależności.
4. **`loop()`** — zadania czasowe sterowane przez porównania `millis()`. Brak blokujących opóźnień (no blocking delays). Każde okresowe zadanie śledzi swój własny znacznik czasu `lastXxx` jako lokalną zmienną `static`.

Pętla `loop` obsługuje minimum:

- **Watchdog check** czasu, który upłynął od ostatniego prawidłowego polecenia MQTT, wyzwalający akcję bezpiecznego zatrzymania (safe-stop) po przekroczeniu limitu.
- `Network::handleNetwork()` dla rekoneksji MQTT i obsługi wiadomości.
- **Publikowanie feedbacku** w ustalonym interwale przy użyciu `Network::sendFeedbackMessage()`.
- Obsługa CAN _(do dodania)_.

---

## 8. Konwencje Stylu C++

|**Praktyka**|**Zasada**|
|---|---|
|Typy całkowite|Zawsze używaj typów z `<cstdint>`: `uint8_t`, `uint16_t`, `uint32_t` itd. Nigdy zwykłego `int` dla rejestrów sprzętowych lub pól protokołu.|
|Stałe|`constexpr` w namespace. Nigdy `#define`.|
|Namespaces|Używane dla wszystkich interfejsów modułów i bloków konfiguracji.|
|Structs|Używane dla wszystkich kontenerów stanu. Styl POD z jawnym konstruktorem ustawiającym domyślne wartości.|
|Bit-fields|Akceptowalne do pakowania wielu flag logicznych w jeden bajt (np. stany komend).|
|`static` locals|Używane dla znaczników czasu `lastXxx` w `loop()` oraz dla pomocników wewnątrz modułów w plikach `.cpp`.|
|`nullptr`|Zawsze używane zamiast `NULL` lub `0` dla porównań wskaźników.|
|Funkcje inline|Akceptowalne w nagłówkach dla krótkich funkcji narzędziowych (np. `init_pins()`).|
|`Serial.println()`|Używane dla wszystkich wyjść diagnostycznych. Każda wiadomość jest poprzedzona tagiem subsystemu w nawiasach, np. `[MQTT]`, `[ETHERNET]`, `[WATCHDOG]`.|

---

## 9. Referencja Konfiguracji i Stanu

### Pliki Config

|**Plik**|**Zawartość**|
|---|---|
|`HardwareConfig.h`|Specyficzne konfiguracje sprzętowe, na przykład: napięcie referencyjne ADC, kalibracja mV-na-amper, rozdzielczość ADC, baud rate portu szeregowego itp.|
|`NetworkConfig.h`|Konfiguracje sieciowe: adres MAC, statyczny IP, IP i port brokera MQTT, nazwy tematów, limit rozmiaru payloadu, timeouty bezpieczeństwa i rekoneksji, interwał publikacji feedbacku|
|`CANConfig.h`|CAN baud rate, ID wiadomości, offsety pól w ramkach|

### Pliki State

|**Plik**|**Kluczowe pola**|
|---|---|
|`HardwareState.h`|Instancje obiektów driverów czujników (amperomierz, termometr, NeoPixel itp.) — jeden na podłączone urządzenie|
|`HardwareCommandState.h`|Jedno pole na każdy sterowalny aktuator, ustawiane na podstawie przychodzących poleceń MQTT. Flagi logiczne używają pól bitowych.|
|`HardwareFeedbackState.h`|Jedno pole na każdy odczyt czujnika do raportowania. Wypełniane przez logikę `Hardware`, konsumowane przez `Network`.|
|`NetworkState.h`|Klient Ethernet, klient MQTT, obiekt adresu IP, znacznik czasu ostatniego poprawnego polecenia|

---

## 10. Dodawanie Nowego Modułu

Gdy wprowadzany jest nowy subsystem (np. magistrala CAN, nowy typ czujnika):

1. Dodaj stałe pinów do `Pins.h` i skonfiguruj je w `init_pins()`.
2. Dodaj parametry czasu kompilacji do odpowiedniego pliku w `Configs/` lub utwórz nowy (np. `CANConfig.h`).
3. Dodaj pola czasu wykonania do odpowiedniego `struct` w `States/` lub utwórz nowy.
4. Utwórz parę plików `ModuleName.h` / `ModuleName.cpp` zgodnie z wzorcem modułu opartego na namespace.
5. Stwórz instancje wymaganych `struct` stanu w `main.cpp` i przekaż je do funkcji `init` nowego modułu w `setup()`.
6. Dodaj okresowe wywołania w `loop()` wedle potrzeb, korzystając z timingu opartego na `millis()`.

Żadne istniejące pliki nie powinny wymagać restrukturyzacji, aby pomieścić nowy moduł. Architektura jest zaprojektowana tak, aby rosła poprzez dodawanie, a nie modyfikację rdzennego wzorca.