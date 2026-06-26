# OCR — Optical Character Recognition Desktop App

A from-scratch OCR pipeline built in C, featuring a custom neural network trained on character recognition, an image preprocessing pipeline, and a GTK3 desktop interface.

---

## Overview

This project takes an image containing printed or handwritten text and extracts it as a plain string. The full pipeline — from raw pixel to readable text — is implemented without any external OCR library. It is split into three independent modules that are compiled separately and linked together at runtime.

```
.
├── Interface/      GTK3 desktop application (UI, layout, CSS)
├── Treatment/      Image preprocessing and segmentation
├── XOR/            Neural network (XOR-based, character classifier)
├── test_ocr.sh     End-to-end test script
└── README.md
```

---

## Architecture

### `Treatment/` — Image Preprocessing & Segmentation

Responsible for turning a raw input image into isolated character bitmaps ready for classification.

| File | Role |
|---|---|
| `seg.c / seg.h` | Entry point for segmentation — splits the image into lines, words, and characters |
| `line_lib.c / .h` | Line detection and horizontal segmentation |
| `word_lib.c / .h` | Word boundary detection within lines |
| `pixel_operations.c / .h` | Low-level pixel manipulation utilities |
| `toBlackWhite.c / .h` | Binarisation — converts the image to pure black and white |
| `new_try.c / .h` | Character isolation and crop logic |
| `main.c` | Standalone test entry point for the treatment pipeline |
| `tests/` | Sample input images for development and testing |

Compiles to `libtreatment.a`.

---

### `XOR/` — Neural Network Classifier

A feedforward neural network that classifies each isolated character bitmap into its corresponding ASCII character. The name reflects the XOR problem used to validate the network architecture during development.

| File | Role |
|---|---|
| `XOR.c / XOR.h` | Neural network core — forward pass, weights, activation |
| `launch.c / launch.h` | Runs inference over a directory of character bitmaps and writes results to `finalresult.txt` |
| `main.c` | Standalone test entry point |
| `test.txt` | Sample expected output for testing |

Compiles to `libxor.a`.

---

### `Interface/` — GTK3 Desktop Application

The graphical front-end. Calls into `libtreatment.a` and `libxor.a` to run the full pipeline on a user-selected image and displays the result.

| File | Role |
|---|---|
| `4puterscanread.c` | Application logic — signal handlers, threading, image scaling |
| `4puterscanread.glade` | UI layout (GtkBuilder XML) |
| `style.css` | Dark theme stylesheet |
| `logo_ocr.png` | Application icon |
| `finalresult.txt` | OCR output written here by the pipeline, then read by the UI |
| `Makefile` | Builds the final binary |

---

## Building

Each module has its own `Makefile`. Build in order:

```bash
# 1. Build the image processing library
make -C Treatment/

# 2. Build the neural network library
make -C XOR/

# 3. Build the interface
make -C Interface/

# 4. Launch the interface
cd Interface && ./4puterscanread
```

Dependencies: `GTK+ 3`, `GLib`, `Cairo`, `GDK-PixBuf` — all standard on any GNOME-based Linux distribution.

```bash
# Debian / Ubuntu
sudo apt install libgtk-3-dev

# Arch
sudo pacman -S gtk3
```

---

## Using the Application

The interface is a single window split into two panels.

```
┌─────────────────────────────────────────────────┐
│ Open Image  Run OCR   Save Text  Save PDF  Quit │  ← header bar
├───────────────────────┬─────────────────────────┤
│                       │                         │
│    Image preview      │     Extracted text      │
│                       │                         │
├───────────────────────┴─────────────────────────┤
│  ● Ready                                        │  ← status bar
└─────────────────────────────────────────────────┘
```

**Step-by-step:**

1. **Open Image** — opens a file picker filtered to image formats. The selected image is displayed in the left panel, scaled to fit. The file picker defaults to `Treatment/tests/` where sample images are provided.

2. **Run OCR** — runs the full pipeline (segmentation → classification) in a background thread so the UI stays responsive. The spinner in the status bar animates while processing. When done, the extracted text appears in the right panel.

3. **Save Text** — saves the content of the text panel as a plain `.txt` file at a location of your choice.

4. **Save PDF** — saves the same content as a formatted A4 PDF, word-wrapped with monospace font.

5. **Quit** — closes the application.

> Opening a new image automatically clears the text panel and disables Save until OCR is run again.

---

## Test Images

Several sample images are provided in `Treatment/tests/`:

| File | Content |
|---|---|
| `Lorem_2.png` | Lorem ipsum paragraph — multi-line printed text |
| `algo.png` | Algorithmic pseudocode |
| `alphabet.png` | Printed alphabet for classifier validation |
| `written_alphabet.png` | Handwritten alphabet |
| `charAppr.png` | Character approximation test |
| `diode.png` | Mixed content with symbols |
| `nose.png` | Additional test case |

---

## End-to-End Test

A shell script is provided to run the pipeline without the GUI:

```bash
./test_ocr.sh
```

This exercises the Treatment and XOR modules directly and checks output against expected results.

## Known Limitations

This project was developed as an academic exercise. The OCR pipeline handles
standard printed text well but has known limitations:

- **Handwritten text** recognition is partial — the classifier was trained on
  a limited dataset and can struggle with stylistic variation
- **Special characters and symbols** (punctuation, accented letters, digits)
  may be misclassified
- **Low contrast or noisy images** can confuse the binarisation step, leading
  to less accurate segmentation
- **Multi-column layouts** are not supported — the line segmentation assumes
  a single left-to-right reading order

These are natural next steps for improvement rather than bugs.