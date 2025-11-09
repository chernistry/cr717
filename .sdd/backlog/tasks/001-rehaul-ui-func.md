# ТЗ на UI плагина **Cherni CR-717** (VST3/AU) — полнофункциональный драм-синт (2025)

> Цель: выдать реалистичное, внедряемое ТЗ для премиального UI VST3/AU на JUCE 7.x с чёткими токенами, размерами, поведением контролов, производственными и доступностными нормами.

---

## 1) TL;DR

* Тёмная тема по умолчанию; контраст AA; цели ≥24×24 px. ([JUCE Documentation][1])
* Жёсткая модульная сетка 12-колоночная + базовая 8 px; окно 1100×620…1400×780; масштаб 0.75–1.25× с пер-мониторной DPI-поддержкой. ([Melatonin][2])
* Резайз и масштаб — через VST3 `IPlugView`/`resizeView` + `IPlugViewContentScaleSupport`; JUCE получает scaleFactor в редактор. ([Steinberg Media][3])
* OS-уровень drag-and-drop MIDI: `juce::DragAndDropContainer::performExternalDragDropOfFiles` с временным `.mid`. ([JUCE Documentation][4])
* Предустановки: VST3 Program Lists + state через `getStateInformation()`/`setStateInformation()`; мгновенная синхронизация UI↔модель. ([Steinberg Media][5])
* Производительность: 60 fps цель; коалесcинг репейнтов; кеш тяжёлых отрисовок `Component::setBufferedToImage`; без аллокаций в paint. ([JUCE][6])
* Измерение мастера: True-Peak/LUFS по ITU-R BS.1770 + EBU Tech 3341/3342; обновление 30–60 Гц. ([JUCE][7])
* Доступность JUCE: фокус, таб-порядок, описания; Keyboard nav; контраст. ([JUCE Documentation][8])
* Хост-данные: tempo/transport из VST3 `ProcessContext` для синка; UI не хранит дубли параметров. ([JUCE][9])
* Тестирование стабильности: **pluginval** + матрица DAW/OS/DPI. ([JUCE][10])

---

## 2) Контекст и ограничения

* **Движок**: JUCE 7.x; форматы VST3 (обязательно), AU (опционально).
* **DAW-минимум**: Ableton Live, Reaper, Studio One.
* **Производительность**: 16 ms бюджет кадра; repaint по грязным прямоугольникам; метры ≤60 Гц. ([JUCE][6])
* **HiDPI**: корректная работа при перемещении между мониторами с разной шкалой; учитывать кейсы Windows Per-Monitor-V2. ([Steinberg Media][11])

---

## 3) Дизайн-система (Tokens) и JUCE-маппинг

### 3.1 Цвета (HSL, dark default)

```json
{
  "color": {
    "bg/0": "hsl(0 0% 10%)",
    "bg/1": "hsl(0 0% 13%)",
    "bg/2": "hsl(0 0% 18%)",
    "panel": "hsl(0 0% 14%)",
    "text/hi": "hsl(0 0% 98%)",
    "text/lo": "hsl(0 0% 65%)",
    "accent/808": "hsl(18 100% 56%)",
    "accent/yellow": "hsl(44 100% 54%)",
    "accent/red": "hsl(2 88% 60%)",
    "accent/cyan": "hsl(188 80% 54%)",
    "ok": "hsl(135 70% 52%)",
    "warn": "hsl(32 95% 56%)",
    "err": "hsl(0 80% 58%)",
    "border": "hsl(0 0% 28%)",
    "meter/peak": "hsl(0 84% 56%)",
    "meter/rms": "hsl(160 70% 48%)"
  }
}
```

* Контраст текста ≥4.5:1 (AA). ([JUCE Documentation][1])
* **JUCE**: хранить в `juce::Colour` из sRGB; токены генерировать в C++ header при сборке.

### 3.2 Типографика и размеры

* Шрифт: системный sans (macOS SF-Pro, Win Segoe UI).
* Размеры: 11/12/14/16/20 pt для UI; межстрочный 1.2–1.35.
* База интервалов: 8 px; радиусы 4/8 px; тени очень умеренные.

### 3.3 Сетка и минимальные габариты

* Окно: **1100×620** (min), **1200×680** (реком.), **1400×780** (max без вкладок).
* Колонки: 12; поля 16 px; гаттер 12 px.
* Масштаб: 0.75–1.25×, пер-мониторная смена scale-factor при переносе окна. Реализация — см. §10. ([Steinberg Media][11])

---

## 4) Информационная архитектура и макеты

### 4.1 Верхняя панель **Transport & Global**

* **Элементы**: Play/Stop, Host Sync индикатор, BPM (host/локально), Swing, Preset Browser, Bank A–H, Pattern ops (Copy/Paste/Clear), MIDI Drag.
* **Высота**: 72 px.
* **Поведение**:

  * Если хост даёт темпо/позицию (`ProcessContext`), переключатель **Host** активен; иначе — внутренний BPM. ([JUCE][9])
  * Кнопка **Drag MIDI** инициирует OS-DND временного `.mid`. ([JUCE Documentation][4])

### 4.2 Центральная область (две вкладки)

* **KIT**: 12 модулей голосов в 3×4; каждый — Knobs/Envs/Sends/Mixer.
* **SEQ** (опционально в v1): 16-шаговый грид с заголовком 1–16 и группами по 4; акцент, чок-группы, длина паттерна.

### 4.3 Правая колонка **Mixer/FX**

* Пер-голосовые `Level`, `Pan`, `Out` (стерео/мульти-аут), `Snd A/B`, M/S/Solo; мастер-метры TP/RMS/LUFS; минималистичный Comp/Limiter.

### 4.4 Нижняя панель **Status/Help**

* Подсказки по модификаторам (Shift=Fine, Alt=Coarse, Ctrl=Reset), состояние синка, CPU UI %, подсветка проблем.

---

## 5) Компоненты: геометрия, состояния, жесты

### 5.1 Кнопка (Primary/Toggle/Segmented)

* Размеры: **Primary** 28×28/28×96; **Toggle** 24×24; **Segmented** высота 28.
* Состояния: default/hover/pressed/disabled; фокус-ринг 2 px.
* Горячие клавиши: Space/Enter активирует фокусный контрол.

### 5.2 Ручка (Knob)

* Диаметр **32 px** (мал.), **40 px** (ср.), **56 px** (больш.).
* Угол хода: 300°. Инкремент: 0.5–1.0% на 10 px, Shift=×0.2, Alt=×3.
* Двойной клик — сброс к default; правый клик — контекст со вводом значения.
* Отрисовка с кешем (`setBufferedToImage(true)`) на idle/resize, не в real-time. ([JUCE][6])

### 5.3 Фейдер (верт./гор.)

* Ширина 14–18 px; минимальная высота 120 px.
* Мелкие деления каждые 1 dB/ед.; крупные подписи шаг 6 dB.

### 5.4 Пэд/шаг (Sequencer Pad)

* Квадрат **28–32 px** при 1200 px ширине окна; сжатие до 24 px при 1100 px.
* Состояния: off / on / accent / current-step; current-step подсвечивается без мерцания; обновление в 60 fps.
* Мультиредактирование: drag-paint, Alt=erase, двойной клик=accent.

### 5.5 Метр (Master/Voice)

* Режимы: True-Peak и RMS/LUFS; баллистика по EBU Tech 3341; шкалы -60…0 dBFS, TP предупреждение -1 dBTP. ([Tech EBU][12])
* Частота обновления: 30–60 Гц; без блокирующих аллокаций. ([JUCE][6])

### 5.6 Браузер пресетов

* Левая колонка: Styles (UKG/House/Techno/…); центр: список с превью; справа: описание/теги.
* Действия: Load/Save/Rename/Duplicate/Star.
* VST3 ProgramLists интегрируются, чтобы хост мог показывать список. ([Steinberg Media][5])

---

## 6) Модули голосов (12 шт.) — параметры и UI

Каждый модуль: заголовок (код цвета), **3–5 ручек**, переключатели, Sends A/B, M/S/Solo.

* **BD**: Level, Tone, Decay, Tune, Click; FineTune, Pan; Send A/B.
* **SD**: Level, Tone, Snappy, Tune; FineTune, Pan.
* **LT/MT/HT**: Level, Decay, Tune; FineTune, Pan.
* **RS/CP/CB**: Level (+Tone для CP), Pan.
* **CH/OH**: Level, Decay; Choke group (OH чокает CH).
* **CY/RD**: Level, Tone, Decay; Pan.

Сетка модулей: карточка **(W 260 × H 160)** при 1200 px ширине; адаптация по сетке 3×4/2×6.

---

## 7) Шаговый секвенсор (вкладка **SEQ**)

* **Ряды**: 12; **колонки**: 16 с группами по 4.
* **Функции**: длина паттерна, swing (glob.), copy row/clear row, random/humanize (стиль-aware).
* **Навигация**: стрелки перемещают фокус; Enter/Space — toggle; Shift+стрелка — диапазон.
* **Синхронизация**: визуальная позиция привязана к аудио-времени хоста (ProcessContext); без таймеров, рендер в UI-таймере 60 Гц. ([JUCE][9])

---

## 8) Глобальные элементы

* **Transport**: Play/Stop; индикатор **EXT** при host sync.
* **BPM**: number + encoder; если хост активен — поле read-only. ([JUCE][9])
* **Swing**: 0–75% с шагом 1%; tool-tip со значением.
* **Pattern Bank A–H**: сегментные кнопки; Copy/Paste/Clear; удержание Alt — дублирование.
* **MIDI Drag**: кнопка с пиктограммой; при удержании — создаётся temp `.mid`, запускается `performExternalDragDropOfFiles`. Фоллбек — Save MIDI/Reveal. ([JUCE Documentation][4])

---

## 9) Доступность и UX-нормы

* Минимальные цели **≥24×24 px**, видимый фокус; контраст **AA**. ([JUCE Documentation][1])
* JUCE Accessibility: описатели ролей/имён, ориентации, значения; поддержка клавиатуры через Focus Traversal/KeyListener. ([JUCE Documentation][8])
* Текст/иконки — @2x/@3x ассеты для ретина/HiDPI. ([Apple Developer][13])

---

## 10) Масштабирование, DPI и ресайз

* **VST3**: реализовать `IPlugView::resizeView` и поддержку `IPlugViewContentScaleSupport` (`setContentScaleFactor`, `getContentScaleFactor`). ([Steinberg Media][3])
* **JUCE**: уважать `AudioProcessorEditor::setScaleFactor`; использовать `getApproximateScaleFactorForComponent()` при `resized()`. Учитывать порядок вызовов в некоторых хостах (создание редактора до `setState`). ([JUCE][14])
* **Windows**: Per-Monitor-V2 DPI; проверка после перетаскивания между дисплеями. ([Steinberg Media][11])
* **macOS**: ретина-ассеты @2x/@3x. ([Apple Developer][15])

---

## 11) Производительность UI

* **Репейнты**: только грязные регионы; объединять; тяжёлые элементы кэшировать `setBufferedToImage(true)`. ([JUCE][6])
* **Timers**: метры 30–60 Гц; грид — 60 Гц; избегать динамического аллокирования в `paint()`. ([JUCE][6])
* **Отказоустойчивость DPI**: тест кейсов из известных хостов/сред. ([JUCE][16])

---

## 12) Предустановки, состояние и хост

* **State**: всё, что влияет на звук/паттерн, хранится в процессорном состоянии; UI-только настройки — в контроллерном/своём. В JUCE — через `getStateInformation()`/`setStateInformation()`. ([JUCE Documentation][17])
* **Program Lists**: экспонировать списки для хоста (отражение пресетов зоной Browser). ([Steinberg Media][5])
* **Порядок вызовов**: учитывать хосты, где `getState → createEditor → setState`. ([JUCE][18])

---

## 13) Палитра и визуальный язык

* Вдохновлён TR-808: тёплые акценты (оранжевый/жёлтый/красный) + прохладные вспомогательные (циан); минимум «скевмо».
* Низкий визуальный шум; прямые поверхности; тонкие границы; подсветка шага — мягкое свечение 1.5–2 px.

---

## 14) Спецификация размеров (ключевые)

| Элемент                 | Размеры                          |
| ----------------------- | -------------------------------- |
| Верхняя панель          | H 72 px                          |
| Карточка голоса         | W 260 × H 160 px (при 1200 шир.) |
| Ручка малая/ср./большая | Ø 32 / 40 / 56 px                |
| Фейдер                  | W 14–18 px, H ≥ 120 px           |
| Пэд шага (SEQ)          | 24–32 px (адаптивно)             |
| Метр мастер             | W 10–14 px, H 120–160 px         |

---

## 15) Микровзаимодействия и хелпы

* Tooltip с подсказкой диапазона/единиц; `Cmd/Ctrl`-клик — ввод точного значения.
* Сброс к дефолту — двойной клик или `Alt`-клик.
* Колёсико мыши — изменение параметра; `Shift` — мелкий шаг.
* Статус-строка сообщает о синх/ошибках drag-and-drop/перегрузе метра.

---

## 16) Аудио-метрология (UI нормы)

* Режимы отображения: Short-term LUFS, Integrated LUFS, True-Peak; LRA (по 3342) в браузере пресетов/аналитике. ([Tech EBU][19])
* Маркеры: 0/-1/-6/-12/-18/-24/-48 dBFS; калибровочный -18 LUFS ref. ([rosseladvertising.be][20])

---

## 17) Тест-матрица и валидация

* **Инструмент**: `pluginval` на macOS/Windows. ([JUCE][10])
* **DAW/OS/DPI**:

  * Windows 10/11, 100%/150%/200%, одно- и многомониторные кейсы; перенос окна между дисплеями. ([Steinberg Media][11])
  * macOS 13–15, Retina/не-Retina; ассеты @2x/@3x. ([Apple Developer][15])
* **Стабильность пресетов**: проверка сохранения/восстановления состояния проекта; быстрая смена пресетов и немедленное обновление UI. ([JUCE Documentation][17])
* **Drag-and-Drop**: тест OS-DND файла .mid в Live/Reaper/Studio One. ([JUCE Documentation][4])

---

## 18) Реализация (JUCE/VST3) — краткий план

1. **Токены** генерировать в C++ (namespace `ui::tokens`), маппить в `juce::Colour`, размеры — в constexpr.
2. **Сетка** — базовый контейнер с 12-колоночным лайаутом; модульность через вложенные `FlexBox`/`Grid`.
3. **Масштаб**: принимать `setScaleFactor`; при `resized()` пересчитывать геометрию, SVG растрировать с учётом `getApproximateScaleFactorForComponent()`. ([JUCE][14])
4. **DPI**: слушать смену дисплеев и корректировать масштаб (Per-Monitor-V2). ([Steinberg Media][11])
5. **PresetManager**: связать с APVTS/моделью; при загрузке пресета — пуш паттерна в SequencerModel перед `createEditor` и после `setState`. ([JUCE][18])
6. **MIDI Drag**: генерация `.mid` → `performExternalDragDropOfFiles` + кнопки Save/Reveal. ([JUCE Documentation][4])
7. **Метры**: таймер 30–60 Гц; баллистика и шкалы по EBU/ITU. ([Tech EBU][12])
8. **Оптимизация**: кеши для ручек/фона; грязные регионы; без выделений памяти в `paint()`. ([JUCE][6])

---

## 19) Набор экранов (компоновки для Figma/реализации)

* **Main / KIT** 1200×680: верхняя панель + 12 модулей + правый микшер/FX + нижний статус.
* **SEQ** 1200×680: верхняя панель + шаговый грид на всю центральную область + правый микшер/FX.
* **Preset Browser**: оверлей 960×560, клавиши навигации ↑/↓, Enter — Load, `Cmd+S` — Save.
* **Compact** 1100×620: модульная укладка 2×6 голосов, узкий микшер.

---

## 20) Антипаттерны

* Таймерный рендер шагов без учёта хост-времени/ProcessContext. ([JUCE][9])
* Внутренний Drag&Drop вместо OS-DND для вывода MIDI из окна плагина. ([JUCE Documentation][4])
* Невнятная DPI-стратегия; отсутствие реакции на смену монитора. ([Steinberg Media][11])
* Частые полные репейнты; аллокации в `paint()`; отсутствие кешей. ([JUCE][6])

---

## 21) Артефакты поставки

* JSON токены + C++ header генерация.
* Набор SVG/PNG @1x/@2x/@3x. ([Apple Developer][13])
* Тест-чеклист (pluginval, DPI-матрица, drag MIDI, пресеты). ([JUCE][10])

---

## 22) Примечания по совместимости

* Нюансы масштаба/порядка вызовов в отдельных хостах (FL, Max, Renoise) учитывать — известные кейсы. ([JUCE][18])

---

### Ссылки (ключевые)

* VST3 `IPlugView`/resize, Content Scale Support. ([Steinberg Media][3])
* JUCE Accessibility, DnD, оптимизация рисования, компонентный фокус. ([JUCE Documentation][8])
* Per-Monitor DPI v2 (Windows). ([Steinberg Media][11])
* Apple HIG: доступность/ретина/контраст. ([Apple Developer][21])
* EBU/ITU loudness/true-peak (метры). ([Tech EBU][19])
* VST3 Presets/Program Lists, JUCE state.
* pluginval.

--