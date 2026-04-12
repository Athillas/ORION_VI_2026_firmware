# Firmware

## Opis
Ten katalog zawiera kod oprogramowania układowego Arduino w języku C++, napisany dla trzech płytek ESP32 WROOM: `Vacuum`, `Power`, `RoboticArm` and `RockerInterface`, schematy każdej płytki (patrz `schematics`) oraz dokumentację (patrz `docs`, w trakcie opracowywania).

## Otwarcie projektu

Do otwierania poszczególnych projektów należy używać rozszerzenia [PlatformIO dla VSCode](https://platformio.org/). Katalog każdego projektu zawiera plik `platformio.ini` z niezbędnymi ustawieniami, w tym informacjami o bibliotekach współdzielonych.

## !!WAŻNE!!
Przed rozpoczęciem pracy nad projektem prosimy o dokładne zapoznanie się z dokumentacją, zwłaszcza z plikami `docs/design_doc_pl.md` dla wersji po Polsku (w trakcie opracowywania) lub `docs/design_doc_en.md` dla wersji w języku Angielskim (w trakcie opracowywania). Znajdują się w nich wyjaśnienia dotyczące architektury modułów, którą należy stosować w tym projekcie.