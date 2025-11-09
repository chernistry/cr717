# Best Practices Guide — VST/AU инструмент в 2025 (для проекта tr808-synth)

## 1. TL;DR

* Формат по умолчанию: **VST3 + AU (macOS)**. VST2 — EOL. VST3 SDK — **MIT-лицензия** → проще дистрибуция. Добавляйте **CLAP** как optional. ([Ableton Help][1])
* Фреймворк: **JUCE 7/8** или **iPlug2**. JUCE — быстрый старт и готовые модули DSP/GUI; iPlug2 — лёгкий, гибкий. ([JUCE][2])
* Реал-тайм: в аудио-потоке **никаких аллокаций/локов/IO**; гасим денормалы (**ScopedNoDenormals**). ([docs.juce.com][3])
* Автоматизация: **sample-accurate** в VST3; параметр-сглаживание (**SmoothedValue**). ([GitHub][4])
* Дистрибуция macOS: **code sign + hardened runtime + notarization**; Universal2 (arm64+x86_64). ([Moonbase][5])
* Пути установки: **VST3 →** `C:\Program Files\Common Files\VST3`, **AU →** `/Library/Audio/Plug-Ins/Components`. ([Steinberg Help Center][6])
* Совместимость DAW: Ableton Live (macOS) = **VST3 и AU**, Windows = **VST3**. Не рассчитывайте на VST2. ([JUCE][7])
* Тестирование: **pluginval** в CI + **auval/Logic** на macOS. ([GitHub][8])
* Производительность: бюджет на аудио-блок без «джиттера»; избегать денормалов, алиасинга → **Oversampling**. ([docs.juce.com][9])
* Security/Supply chain: SLSA-уровни, актуальный **OWASP Top 10 (2025 RC1)** фокус на мисконфиге; подпись/проверка артефактов. ([SLSA][10])

---

## 2. Ландшафт — что новое в 2025

* **VST3 SDK → MIT**: упрощены юридические риски и интеграции. ([Ableton Help][1])
* **JUCE**: активная поддержка OS/DAW и план **JUCE 8**; зрелая CMake-сборка. ([JUCE][11])
* **CLAP** набрал критическую массу: Bitwig, u-he, FL Studio 24, Reaper и др.; открытая MIT-лицензия; мультипоточность, MIDI 2.0-friendly. Ableton Live пока без CLAP. ([u-he][12])
* **MIDI 2.0/UMP**: CoreMIDI поддерживает Universal MIDI Packet; закладывайте адаптер. ([Apple Developer][13])
* **macOS дистрибуция**: обязательны hardened runtime + notarization (`notarytool`). ([JUCE][14])
* **Безопасность цепочки поставки**: SLSA/провананс сборок стал нормой. ([SLSA][10])

---

## 3. Архитектурные паттерны

### Pattern A — **C++ плагин (MVP): JUCE VST3 + AU**

**Когда**: быстрый выход в Ableton/Logic/Reaper; минимальный риск.
**Шаги**: JUCE Audio Plug-In → CMake → модули `juce_audio_processors`, `juce_dsp`; целевые форматы VST3/AU.
**Плюсы**: единая кодовая база, готовые DSP-утилиты, SmoothedValue/Oversampling, кросс-DAW. **Минусы**: коммерческая лицензия для коммерц. релизов. ([JUCE][2])
**Опции позже**: CLAP через clap-juce-extensions; MIDI 2.0; AAX. ([u-he][12])

### Pattern B — **Лёгкая рамка (Scale-up): iPlug2 VST3/AU/CLAP**

**Когда**: нужен лёгкий рантайм, тонкая настройка GUI/конкурентности, WebAssembly-ветка.
**Миграция из A**: перенести DSP ядро и параметры, переписать GUI слой. ([GitHub][15])

---

## 4. Priority 1 — Реал-тайм аудио и корректность DSP

**Почему**: стабильный аудио-поток без дропов — ключевая цель.
**Scope**: процессинг, параметры, автоматизация, денормалы, антиалиасинг.
**Решения**:

* Без аллокаций, lock-free, без IO/логов в аудио-потоке.
* Смягчение параметров: `juce::SmoothedValue` для всех управляемых параметров, особенно частот/гейнов. ([JUCE][16])
* Sample-accurate автоматизация в VST3: обрабатывайте event-лист со смещениями. ([GitHub][4])
* Денормалы: `juce::ScopedNoDenormals` в `processBlock`. ([docs.juce.com][3])
* Антиалиасинг нелинейностей: `juce::dsp::Oversampling` (выбор FIR/IIR по латентности/фазе). ([docs.juce.com][9])
  **Имплементация (шаги)**:

1. Добавьте `ScopedNoDenormals guard;` в начале `processBlock`. ([docs.juce.com][3])
2. Для каждого `AudioParameterFloat` заведите `SmoothedValue`, обновляйте в `prepareToPlay` и тикайте в цикле семплов/блоков. ([JUCE][16])
3. VST3: итерируйте `IParameterChanges` и применяйте изменения со sample offset. ([GitHub][4])
4. Для сатурации/драйва — оберните нелинейность в Oversampling x2–x4; выставьте `setLatencySamples` по `getLatencyInSamples()`. ([docs.juce.com][9])
   **Guardrails & SLOs**:

* XRuns = 0 при 48kHz/64-sample buffer; отклонение времени блока ≤25% p95.
* Аудио-поток без аллокаций; целевой CPU <10% на 8-голосном паттерне.
  **Failure modes → recovery**:
* Пики CPU при тишине → включите denormal flush; проверьте IIR-ветви. ([JUCE][17])
* Щелчки при автом. → увеличить время сглаживания или перейти на sample-accurate путь. ([JUCE][18])

---

## 5. Priority 2 — Дистрибуция и совместимость DAW

**Почему**: простой инстал, минимум тикетов поддержки.
**Scope**: форматы, пути, подписание, notarization.
**Решения**:

* Форматы: VST3 для Windows/macOS; AU для macOS; CLAP опционально. Ableton Live: VST3/AU на macOS, VST3 на Windows. ([JUCE][7])
* Пути установки: `C:\Program Files\Common Files\VST3`; `/Library/Audio/Plug-Ins/Components`. ([Steinberg Help Center][6])
* macOS: Developer ID, hardened runtime, notarization (`xcrun notarytool`). ([JUCE][14])
  **Шаги**:

1. Подписать:

```bash
codesign --deep --force --timestamp --options runtime \
  --sign "Developer ID Application: Your Co" "My808.vst3"
```

2. Упаковать в `.pkg` и отправить на нотаризацию:

```bash
xcrun notarytool submit My808.pkg --keychain-profile "AC_PROFILE" --wait
```

3. Проверка: Logic/Live видят плагин; AU проходит `auval`. ([JUCE][14])
   **SLOs**: установка <1 мин; плагин появляется в DAW после первого скана.

---

## 6. Priority 3 — MIDI 2.0 и расширения

**Почему**: долговременная совместимость и выразительность.
**Решения**:

* Спроектируйте слой событий так, чтобы легко добавить **MIDI 2.0/UMP**. На macOS CoreMIDI уже поддерживает UMP. ([Apple Developer][13])
* Рассмотрите **CLAP** для пер-нотовой модуляции и лучшей многопоточности. ([u-he][12])
  **Шаги**: абстрагируйте MIDI как «note-events + per-note params», не жестко привязываясь к MIDI 1.0.

---

## 7. Тестовая стратегия

* **Unit**: DSP-ядро отдельно от хост-API; тесты фильтров/огибающих.
* **Integration**: host-sim через **pluginval** headless. ([GitHub][8])
* **E2E**: загрузка в AudioPluginHost/Logic/Live; пресеты/автоматизация. ([JUCE][19])
* **Performance**: профилирование по блокам; тест «тихий хвост» на денормалы.
* **Security**: SBOM, зависимостям — скан; проверка подписей артефактов.

---

## 8. Observability & Ops

* **Метрики**: время обработки блока p50/p95, кол-во параметр-ивентов, отчёты об исключениях UI.
* **Логи**: только в debug, вне аудио-потока.
* **Алерты**: падения при скане хоста, провал notarization.

---

## 9. Security Best Practices

* **Подпись/нотаризация** для macOS; на Windows — код-подпись. ([JUCE][14])
* **SLSA**: как минимум L1–L2 (провананс сборок, репродьюсабельность). ([SLSA][10])
* **OWASP Top 10 2025 RC1**: акцент на Security Misconfiguration — храните пути/доступы в конфиге релиз-сборки, не используйте сетевые вызовы без необходимости. ([OWASP Foundation][20])
* **Зависимости**: фиксированные версии JUCE/iPlug2/VST3 SDK; воспроизводимые билды.

---

## 10. Performance & Cost

* **Бюджеты**: при 48 kHz / 64 samples — p95 обработка блока ≤0.5× буфера; CPU <10% на типовой машине.
* **Оптимизация**: SIMD (SSE/NEON) через JUCE dsp; избегать денормалов; разумный Oversampling (2–4×). ([docs.juce.com][9])
* **Профилирование**: стресс-автомация, «silence tail».

---

## 11. CI/CD Pipeline (минимум)

* **Сборка**: GitHub Actions `macos-14`, `windows-latest`; CMake.
* **Валидация**: `pluginval --strictness-level 8` на собранных бинарях. ([GitHub][8])
* **Подпись/Нотаризация**: шаги codesign/notarytool для macOS. ([JUCE][14])
* **Релизы**: артефакты `.vst3` + `.component` + установщики.
* Референс по CI для аудио-плагинов. ([Moonbase][5])

---

## 12. Code Quality Standards

* **Стиль**: современный C++17/20, RAII, без new/delete.
* **Типы**: строгие `enum class`, `span`, `gsl` при желании.
* **Документация**: Doxygen для DSP ядра.
* **Рефакторинг**: GUI и DSP разнесены; параметры — в одном слое.

---

## 13. Reading List (даты и суть)

* Steinberg **VST3 Dev Portal** — параметры/автомация, sample-accurate. (обновляется) — основы работы с параметрами и событиями. ([Steinberg Media][21])
* Steinberg **VST3 SDK (MIT)** — исходники и примеры. (актуально 2025) — примеры плагинов. ([GitHub][4])
* **Apple**: Prepare for distribution / Hardened runtime / Notarization. (живой док) — требования macOS. ([JUCE][14])
* **JUCE Docs**: Oversampling, ScopedNoDenormals, dsp. (актуально) — готовые классы. ([docs.juce.com][9])
* **pluginval** — CLI-валидация плагинов, CI. (актуально) — тесты форматов/хостов. ([GitHub][8])
* **CLAP**: обзор/преимущества, лицензия MIT. (u-he) — когда добавлять CLAP. ([u-he][12])
* **CoreMIDI UMP/MIDI 2.0** — база под будущее MIDI. (Apple) ([Apple Developer][13])

---

## 14. Decision Log (ADR)

* **ADR-001**: *VST3+AU как baseline; CLAP — optional* вместо «только VST3», потому что Live/Logic покрываются сразу, а CLAP даёт плюсы в Bitwig/Reaper. ([JUCE][7])
* **ADR-002**: *JUCE для MVP* вместо iPlug2, т.к. нужны SmoothedValue/Oversampling и быстрый старт; iPlug2 — как альтернатива при scale. ([docs.juce.com][9])
* **ADR-003**: *Обязательная notarization macOS* из-за требований Gatekeeper. ([JUCE][14])

---

## 15. Анти-паттерны

* **Аллокации/локи в аудио-потоке** → джиттер/дропы. Делайте prealloc и lock-free очереди.
* **Отсутствие сглаживания параметров** → щелчки. Используйте SmoothedValue. ([JUCE][16])
* **Игнор denormals** → CPU-пики на тишине. Включайте ScopedNoDenormals. ([docs.juce.com][3])
* **Сырой нелинейный DSP без oversampling** → алиасинг. Применяйте x2–x4 Oversampling. ([docs.juce.com][9])
* **Неправильные пути установки** → DAW не видит плагин. Используйте стандартные директории. ([Steinberg Help Center][6])

---

## 16. Evidence & Citations

Ключевые утверждения и практики подтверждены: VST3 MIT, Dev Portal и SDK; Apple notarization/paths; Ableton форматы; CLAP обзор; MIDI 2.0/UMP; real-time правила; denormals; oversampling; pluginval. См. ссылки в тексте. ([Ableton Help][1])

---

## 17. Verification

* **Скрипт-дымок (macOS)**: собрать VST3/AU, `codesign` + `notarytool`, скопировать в стандартные пути, открыть Logic/Live, пройти `auval`, запустить `pluginval --strictness-level 8`. ([JUCE][14])
* **Бенчмарк**: 48k/64 samples — измерить p95 времени блока в DAW и на host-симуляции; тест «fade-to-silence» на денормалы. ([JUCE][17])
* **Конфиденс**: Архитектура/реал-тайм — High; Дистрибуция/подпись — High; MIDI 2.0/CLAP — Medium (зависит от хостов).

---

### Приложение — Быстрый старт (JUCE, CMake, VST3+AU)

**Сборка форматов**: включите в CMake `JUCE_BUILD_VST3_PLUGIN TRUE`, `JUCE_BUILD_AU_PLUGIN TRUE`. Документы JUCE/дорожная карта подтверждают актуальность CMake/форматов. ([JUCE][11])

**Install-paths**:

* Windows: копируйте `.vst3` в `C:\Program Files\Common Files\VST3`. ([Steinberg Help Center][6])
* macOS: `.vst3` → `/Library/Audio/Plug-Ins/VST3`, `.component` → `/Library/Audio/Plug-Ins/Components`. ([Apple Support][22])

**Проверка автоматизации**: обработайте `IParameterChanges` для sample-accurate; сглаживание — `SmoothedValue`. ([GitHub][4])

**Oversampling**: `dsp::Oversampling<float>(channels, stages, FIR|IIR)`; latency сообщайте хосту. ([docs.juce.com][9])

---

Если хочется Web-версию поверх DAW-плагина, держите DSP-ядро чистым (C++) и делайте тонкий бридж в WebAudio/WASM; но сам плагин под Ableton должен быть нативным VST3/AU. Основание: требования хостов и форматов. ([JUCE][7])

