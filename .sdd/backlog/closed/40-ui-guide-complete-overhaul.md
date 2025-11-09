# UI Best-Practices Guide — **VST3-плагин** (2025, фокус на JUCE/VSTGUI)

## 1) TL;DR

* Поддерживайте **ресайз и HiDPI** через `IPlugView` + `IPlugFrame::resizeView()`/`canResize()` и `IPlugViewContentScaleSupport` для Windows Per-Monitor DPI v2. Не меняйте окна хоста напрямую. ([steinbergmedia.github.io][1])
* Рендер — **без OpenGL на macOS**. OpenGL официально **deprecated** с macOS 10.14; избегайте `juce_opengl` на релизах для Mac. ([Apple Developer][2])
* Минимизируйте нагрузку на **message thread**: throttle repaints (≈20–30 Гц для бегунков/метров), рисуйте только изменившиеся области, используйте `setBufferedToImage(true)` для статичных блоков. ([JUCE][3])
* Параметры ↔ UI через **APVTS-attachments** (Slider/Button/ComboBox). Это гарантирует корректную автомацию и потокобезопасную синхронизацию. ([JUCE][4])
* **Ресайз-UX**: уголок/меню масштабирования в самом редакторе + сохранение размера в состоянии плагина, но с учётом обратной совместимости пресетов/сессий. ([iPlug2 Forum][5])
* **Доступность**: JUCE AccessibilityHandler для названий и действий контролов; как минимум — клавиатурный фокус и подсказки. ([JUCE Documentation][6])
* **Подпись/нотаризация macOS**: Developer ID App + Installer, **Hardened Runtime**, notarize **PKG/DMG/ZIP** (не `.vst3` напрямую). Автоматизируйте в CI. ([Apple Developer][7])
* Инструменты проверки: **pluginval** (кросс-хост), **auval** (AU), тесты в основных DAW.
* Новое/важное к 2025: VST3 SDK под **MIT-лицензией**; VSTGUI обновлялся в составе SDK; интерфейсы VST3 для маппинга/контента развиваются. Планируйте миграции с VST2. ([Steinberg Forums][8])
* SLO: **UI-кадры ≤16.6 мс**, repaints ≤30 Гц для meters, нулевой UB при DPI/ресайзе, *zero* host-window poking.

---

## 2) Ландшафт 2025

* **VST3 SDK → MIT**: упрощает юридию и встраивание шаблонов CMake. ([Steinberg Forums][8])
* **VSTGUI** идёт в составе SDK, регулярные фиксы (напр. 4.14.3 в 3.7.14). ([Steinberg Forums][9])
* **HiDPI (Windows Per-Monitor v2)**: корректный DPI-скейлинг обязателен; ориентируйтесь на per-monitor DPI. ([Microsoft Learn][10])
* **macOS**: обязателен **Hardened Runtime** + notarization; OpenGL — deprecated → избегайте GL в UI. ([Apple Developer][11])
* **JUCE**: есть Accessibility API; APVTS-attachments для безопасной связки UI↔параметров. ([JUCE Documentation][6])
* Альтернатива веб-UI: **vstwebview** (встроенный WebView2/WebKit); оценивайте риски размера/совместимости. ([GitHub][12])

---

## 3) UI-архитектурные паттерны

### A. **JUCE Minimal Editor** (MVP)

**Когда**: 1 окно, ≤30 контролов, без сложной графики.
**Шаги**:

1. `AudioProcessorEditor` фиксирует min/max size, `setResizable(true, true)`, `setResizeLimits`.
2. Параметры через `AudioProcessorValueTreeState` + Slider/Button-Attachments.
3. Throttle UI обновлений (Timer 20–30 Гц).
4. `setBufferedToImage(true)` для статических секций, частичные `repaint(Rectangle)`.
   **Плюсы**: минимальный код, хорошая совместимость. **Минусы**: CPU-рендер, осторожно с плотными перерисовками.
   **Позже**: кастомный LookAndFeel, темизация.

### B. **JUCE Scalable/HiDPI** (Scale-up)

**Когда**: много панелей/метров/редакторов, ресайз и DPI-миграции между мониторами.
**Шаги**:

1. Реализуйте **host-driven** и **client-driven** ресайз (см. `IPlugView`/`IPlugFrame`).
2. Реагируйте на `IPlugViewContentScaleSupport` (Windows) и логические пиксели на macOS.
3. Сохраняйте размер/скейл безопасно для backward-compat.
   **Миграция**: из A → добавляйте DPI-слои и централизованный Layout-менеджер. ([steinbergmedia.github.io][1])

---

## 4) Priority 1 — **Параметры и автомация (UI↔DSP)**

**Зачем**: детерминированность, корректная автомация в DAW, без гонок потоков.
**Скоуп**: APVTS, Attachments, модель состояний; **вне**: DSP-сглаживание.
**Решения**:

* Только **APVTS** + стандартные attachments (или свои для кастом-виджетов).
* Все параметры с постоянными **ID**; думайте о VST2→VST3 совместимости.
* UI меняет параметры **на message-потоке**, DSP читает атомарно/сглаженно.
  **Имплементация (пример JUCE)**:

```cpp
// Processor.h
AudioProcessorValueTreeState apvts {*this, nullptr, "PARAMS",
  { std::make_unique<AudioParameterFloat>("gain","Gain",NormalisableRange<float>(0.f,1.f),0.5f) }};

// Editor.h
juce::Slider gain;
std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAtt;

gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "gain", gain);
```

**Границы/SLO**: событие UI→параметр ≤1 кадр UI; автомация без пропусков.
**Сбой→восстановление**: рассинхрон/дребезг → проверить IDs/attachments; миграция пресетов VST2→VST3 → см. предупреждения JUCE по совместимым ID. ([JUCE][4])

---

## 5) Priority 1 — **HiDPI/Resizing/Кросс-хост**

**Зачем**: читаемость, предсказуемое поведение в Live/Logic/FL/REAPER.
**Скоуп**: DPI-скейл, ресайз из хоста и клиента.
**Решения**:

* Реализуйте **оба** пути: host-resize (`canResize()`/`onSize()`) и plugin-inititated (`IPlugFrame::resizeView`).
* На Windows учитывайте `IPlugViewContentScaleSupport` (масштаб логических пикселей).
* Не трогайте хост-окно напрямую; только через API.
  **Код-скелет (VST3)**:

```cpp
tresult PLUGIN_API MyView::canResize() { return kResultTrue; }
tresult PLUGIN_API MyView::onSize(ViewRect* newSize) { /* layout(); */ return kResultOk; }
// plugin-initiated:
plugFrame->resizeView(this, &newRect);
```

**Границы/SLO**: ресайз без мерцаний; корректный размер на мониторах с разным DPI.
**Сбой→восстановление**: двойной размер при переносе экрана → верифицировать логику логических пикселей и повторные `onSize()`. ([steinbergmedia.github.io][1])

---

## 6) Priority 2 — **Производительность UI**

**Зачем**: минимум CPU в DAW; нет «заиканий» аудио из-за UI.
**Решения**:

* Repaint-throttle 20–30 Гц для индикаторов; `Timer` вместо постоянного `repaint()`.
* Рисуйте **только изменившиеся регионы**; избегайте `paintOverChildren`.
* Кэшируйте статичный комплексный рендер через `setBufferedToImage(true)`.
* Избегайте OpenGL в UI на macOS; JUCE рендерит на CPU.
  **SLO**: средний кадр UI ≤16.6 мс, Long-taskов UI нет.
  **Сбой→восстановление**: «жрёт CPU» → снизить частоту repaints, применить кэш, дробить layout. ([JUCE][3])

---

## 7) Priority 3 — **Доступность и ввод**

**Зачем**: управляемость клавиатурой, screen-reader-подсказки, проф-стандарты.
**Решения**:

* JUCE Accessibility: имена/роли/действия для контролов; видимые фокусы.
* Горячие клавиши для навигации/триггеров; фокус-менеджмент.
  **Сбой→восстановление**: недоступные элементы → добавить `AccessibilityHandler`/имена/фокус. ([JUCE Documentation][6])

---

## 8) Дизайн-система (нативный скин)

Пример **токенов**:

```cpp
namespace Theme {
  constexpr int kPad  = 8, kGap = 6, kRadius = 6;
  const juce::Colour Bg0{0xff1a1a1a}, Bg1{0xff262626}, Text0{0xfff2f2f2};
  const juce::Colour Accent{0xffFF6B35}; // 808-orange
}
```

Пример LookAndFeel-правил: кастомные слайды/кнопки с единым paddings/радиусами, размер **минимум 24×24 px** для кликабельных целей. ([JUCE][13])

---

## 9) Тестирование

* **Статика**: clang-tidy, ASAN/UBSAN.
* **Хост-совместимость**: Ableton, Cubase, REAPER, FL, Logic (AU).
* **Валидация**: **pluginval**; **auval** для AU.
* **Сценарии**: автомация параметров, ресайз на разных DPI, перенос между мониторами, сохранение/загрузка пресетов, стресс-repaint.

---

## 10) Наблюдаемость/Операции

* Без фанатизма: файл-лог уровня DEBUG с выключением в релизе, счётчики repaints/длительность кадра (переменная диагностики).
* Сбоепад: сбор **crash-репортов** вне DAW-процесса, уважайте приватность.

---

## 11) Безопасность/Дистрибуция

* **macOS**: подпись `Developer ID Application` + `Developer ID Installer`, включить **Hardened Runtime**, notarize **установщик** (PKG/DMG/ZIP) и «степлить» тикет. Автоматизировать notarytool в CI. ([Apple Developer][7])
* Не трогайте сетевые разрешения без необходимости; не внедряйте авто-обновлялки внутри плагина.

---

## 12) Производительность и бюджеты

* **UI**: 60 fps цель; meters/анимации 20–30 Гц; repaint-области минимальны.
* **CPU**: UI ≤2–3% одного ядра на типовом проекте; никаких блокировок аудио-потока из-за UI.
* **Память**: кэш-буферы под `setBufferedToImage` для больших статичных секций.

---

## 13) CI/CD

* Матрица: **Win x64**, **macOS universal (arm64+x64)**.
* Шаги: build → unit-tests → pluginval → codesign (App) → package (PKG/ZIP) → notarize → staple → upload. ([Melatonin][14])

---

## 14) Качество кода

* C++20, clang-tidy профили для JUCE, форматирование clang-format.
* Правило: **UI только на message-потоке**, DSP без UI-вызовов.
* Компоненты без тяжёлой логики в `paint()`, никаких аллокаций внутри `paint()`.

---

## 15) Чёрный список анти-паттернов

* Изменение хост-окна напрямую вместо `IPlugFrame::resizeView()` → несовместимость. ([KVR Audio][15])
* OpenGL-оверлей на macOS → нестабильность и потенциальные проблемы с будущими версиями macOS. ([Apple Developer][2])
* Тотальный `repaint()` на 60 Гц → лишний CPU. Дробите и кэшируйте. ([JUCE][16])
* Игнор DPI/ContentScale на Windows → «двойной» размер при переносе окна. ([steinbergmedia.github.io][17])

---

## 16) Источники (ключевые)

* **VST3 SDK → MIT; CMake templates**. ([Steinberg Forums][8])
* **VSTGUI обновления в SDK**. ([Steinberg Forums][9])
* **IPlugView/IPlugFrame (ресайз)**; **ContentScaleSupport**. ([steinbergmedia.github.io][1])
* **Windows Per-Monitor v2 DPI**. ([Microsoft Learn][10])
* **OpenGL deprecated в macOS**. ([Apple Developer][2])
* **JUCE Accessibility**; **APVTS Attachments**. ([JUCE Documentation][6])
* **Notarization + Hardened Runtime**; **пакет для notarize**. ([Apple Developer][7])
* **pluginval**.

---

## 17) Верификация

* **Авто**: pluginval, regression-скрипт (ресайз, DPI-перенос, автомация), auval для AU.
* **Ручные чек-листы**:

  1. Ресайз из хоста и из плагина без артефактов.
  2. DPI-перенос между мониторами без «двойного» масштаба.
  3. Автомация параметров и откат пресетов без drift.
  4. Throttle repaints ≤30 Гц, кадры ≤16.6 мс.
* **Уверенность**: совместимость/ресайз — **High**; notarize/подпись — **High**; перф-гайд — **Medium** (зависит от дизайна).

---

### Приложение — скелеты кода

**JUCE: базовый editor с ресайзом и кэшем**

```cpp
class Editor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  Editor (Processor& p): AudioProcessorEditor (&p), proc (p) {
    setResizable (true, true);
    setResizeLimits (400, 260, 1600, 1040);
    addAndMakeVisible (gain);
    gainAtt = std::make_unique<APVTS::SliderAttachment>(proc.apvts, "gain", gain);
    staticPanel.setBufferedToImage (true); // кэш статичной панели
    startTimerHz (30); // throttle визуализаций
    setSize (520, 320);
  }
  void timerCallback() override { /* обновить лёгкие индикаторы; repaint нужных регионов */ }
  void resized() override {
    auto b = getLocalBounds().reduced (8);
    gain.setBounds (b.removeFromTop (40));
    staticPanel.setBounds (b);
  }
private:
  using APVTS = juce::AudioProcessorValueTreeState;
  Processor& proc;
  juce::Slider gain; juce::Component staticPanel;
  std::unique_ptr<APVTS::SliderAttachment> gainAtt;
};
```

**VST3: канонический ресайз (плагин инициирует)**

```cpp
// plugFrame предоставлен хостом при attach()
ViewRect r {0, 0, newW, newH};
if (plugFrame != nullptr)
  plugFrame->resizeView (this, &r); // хост затем вызовет onSize()
```

([steinbergmedia.github.io][1])

**JUCE Accessibility (идея)**

```cpp
// Для кастомного компонента можно вернуть AccessibilityHandler
std::unique_ptr<juce::AccessibilityHandler> MyButton::createAccessibilityHandler() {
  using AH = juce::AccessibilityHandler;
  auto actions = juce::AccessibilityActions().addAction (juce::AccessibilityActionType::press, [this]{ triggerClick(); });
  return std::make_unique<AH>(*this, juce::AccessibilityRole::button, actions, juce::AccessibilityText({ "Play" }, {}, {}));
}
```

([JUCE Documentation][6])

---

Если нужен вариант под **VSTGUI** или под **веб-UI (vstwebview)**, укажи целевую платформу/хосты — дам адаптированный набор правил и примеры.

[1]: https://steinbergmedia.github.io/vst3_doc/base/classSteinberg_1_1IPlugView.html "Interface Technology Basics: IPlugView Class Reference"
[2]: https://developer.apple.com/documentation/macos-release-notes/macos-mojave-10_14-release-notes "macOS Mojave 10.14 Release Notes"
[3]: https://forum.juce.com/t/solved-better-repaint-debugging/50451 "[Solved] Better Repaint Debugging"
[4]: https://forum.juce.com/t/several-accessibility-issues-on-windows/48145 "Several accessibility issues on Windows"
[5]: https://iplug2.discourse.group/t/any-way-to-save-plugin-ui-resize-factor-as-a-parameter-yes-one-approach-here/797 "Any way to save plugin UI resize factor as a Parameter? Yes"
[6]: https://docs.juce.com/master/classjuce_1_1AccessibilityHandler.html "juce::AccessibilityHandler Class Reference"
[7]: https://developer.apple.com/documentation/security/notarizing-macos-software-before-distribution "Notarizing macOS software before distribution"
[8]: https://forums.steinberg.net/t/vst-3-8-0-sdk-released/1011988 "VST 3.8.0 SDK Released"
[9]: https://forums.steinberg.net/t/vst-3-7-14-sdk-released/996702 "VST 3.7.14 SDK Released"
[10]: https://learn.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows "High DPI Desktop Application Development on Windows"
[11]: https://developer.apple.com/documentation/xcode/configuring-the-hardened-runtime "Configuring the hardened runtime"
[12]: https://github.com/rdaum/vstwebview "rdaum/vstwebview: Write user interfaces for VST3 plugins ..."
[13]: https://forum.juce.com/t/scalling-issues-on-windows-machines/64315 "Scalling issues on Windows machines"
[14]: https://melatonin.dev/blog/how-to-code-sign-and-notarize-macos-audio-plugins-in-ci/ "How to code sign and notarize macOS audio plugins in CI"
[15]: https://www.kvraudio.com/forum/viewtopic.php?t=556571 "resize window with custom GUI (no VSTGUI) how to? ..."
[16]: https://forum.juce.com/t/repaint-terrible-performance-why/59808 "Repaint() terrible performance why?"
[17]: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.6.6/IPlugViewContentScaleSupport.html "[3.6.6] PlugView Content Scaling - VST 3 Developer Portal"
