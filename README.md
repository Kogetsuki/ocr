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
├── tests/          Unit and end-to-end test suite
├── ocr_e2e.sh      End-to-end test script
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
| `ocr.c` | Application logic — signal handlers, threading, image scaling |
| `ocr.glade` | UI layout (GtkBuilder XML) |
| `style.css` | Dark theme stylesheet |
| `finalresult.txt` | OCR output written here by the pipeline, then read by the UI |
| `Makefile` | Builds the final binary |

---

## Building
**Dependencies**:

`GTK+ 3`, `GLib`, `Cairo`, `GDK-PixBuf` — all standard on any GNOME-based Linux distribution.

```bash
# Debian / Ubuntu
sudo apt install libgtk-3-dev

# Arch
sudo pacman -S gtk3
```
**Build in order** (each module has its own `Makefile`):

```bash
# 1. Build the image processing library
make -C Treatment

# 2. Build the neural network library
make -C XOR

# 3. Build the interface
make -C Interface

# 4. Launch the interface
cd Interface && ./ocr
```
**Or run the end-to-end building script:**
```bash
./ocr_e2e.sh <option>
```
Available options:

| Option | Description |
|--------|-------------|
| `deps` | Check for and install required dependencies. |
| `compile` | Compile all project modules. |
| `run` | Launch the graphical user interface. |
| `clean` | Remove compiled files and build artifacts. |
| `all` | Execute all steps (clean, install/check dependencies, compile, and launch the application). |
| `help` | Display the script help message. |
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

## Testing
The test suite lives in `tests/` and is split into unit tests (C) and an end-to-end shell suite.

```
tests/
├── Makefile/              builds and runs everything
├── test_treatment.c       unit tests for the Treatment module
├── test_xor.c             unit tests for the XOR neural network
└── test_suite.sh          end-to-end pipeline tests (bash)
```

**Run everything:**
```bash
make -C tests
```

**Run only unit tests:**
```bash
make -C tests unit
```

**Run only the end-to-end suite:**
```bash
make -C tests e2e
```

**Run a single end-to-end test by name:**
```bash
./tests/test_suite.sh -k alphabet
```

**Verbose mode (shows the first lines of OCR output for each image):**
```bash
./tests/test_suite.sh -v
```

**What is tested**
| File | Coverage |
|---|---|
| `test_treatment.c` | `get_pixel`/`put_pixel` roundtrip and boundary behavior <br> `new_caracter`/`caracter_free` (null check, table zeroed, linked list) <br> `new_line`/`new_column`/`free_line`/`free_column` (struct initialisation) <br> `seg()` smoke tests (no crash, `.BaW` and `.car` produced) |
| `test_xor.c` | `Transform()` — all 59 outputs printable, covers A–Z and a–z, no duplicate mapping <br> `InitialiseTarget()` — one-hot encoding, hot index matches `i % 59` <br> `RandomInit()` — weights non-zero, in `[-0.5, 0.5]`, two calls differ <br> `launcher()` / `Run()` / `FileParser1()` integration (skips gracefully if `.car` absent) |
| `test_suite.sh` | Build artefacts presents, PNG integrity (magic bytes), full pipeline on every test image, output quality (non-empty, printable, size), interface file checks |


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