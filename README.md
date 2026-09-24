# ✍️ C++ Neural Network from Scratch (with WebAssembly)

A handwritten digit recognizer built from scratch in C++ to demonstrate a fundamental understanding of neural network mechanics. The trained model is compiled to **WebAssembly (Wasm)**, allowing it to run at near-native speed directly in a web browser with an interactive HTML/JavaScript frontend.

![App Screenshot](https://i.postimg.cc/xTpxTyj1/Screenshot-2025-07-29-at-12-44-10-PM.png)

---

## ✨ Features

- **Neural Network From Scratch**: The entire network—including matrix operations, layers, and the feedforward process—is implemented in C++ without external ML libraries.
- **High-Performance Inference**: All prediction logic runs in a pre-compiled WebAssembly module, making it incredibly fast.
- **Interactive Canvas**: Users can draw a digit with their mouse for instant prediction.
- **C++ Training**: Backpropagation implemented from scratch in C++. Training and inference both run in pure C++ — no Python, no PyTorch.

---

## 📊 Results

- **97.43%** on the 10k MNIST test set, native C++ (no browser).
- **97.33%** through the actual browser pipeline — all 10k test digits run through the demo's own preprocessing + WASM in headless Chromium (not people's own handwriting, MNIST digits through the deployed path).
- Weights shipped as a **float32 binary (398 KB)**, down from a 1.16 MB text format — identical predictions on all 10k test digits.
- **What broke:** the live demo was silently running an *untrained* network — a clean "1" came back as "4" — then a stale `?v=1` cache tag kept serving the old script. Fixed by having the loader verify it read exactly 101,770 parameters (disabling Predict otherwise) and switching cache tags to content hashes.

---

## 🛠️ Tech Stack

- **C++ (17)**: For the core neural network implementation.
- **CMake**: To manage the C++ build process.
- **Emscripten**: The toolchain used to compile C++ to WebAssembly.
- **WebAssembly (Wasm)**: The high-performance binary format for the web.
- **HTML/CSS/JavaScript**: For the interactive user interface.

---

## 📂 Project Structure
```
cpp-neural-network/
│
├── docs/                 # Contains all files for the deployed website
│   ├── index.html
│   ├── main.js
│   ├── network.js        # (auto-generated)
│   ├── network.wasm      # (auto-generated)
│   └── network.data      # (auto-generated)
│
├── include/              # C++ header files (.h)
│   ├── Layer.h
│   ├── Matrix.h
│   ├── MNISTReader.h
│   └── Network.h
│
├── src/                  # C++ source files (.cpp)
│   ├── bindings.cpp
│   ├── Layer.cpp
│   ├── main.cpp
│   ├── Matrix.cpp
│   ├── MNISTReader.cpp
│   └── Network.cpp
├── .gitignore
└── CMakeLists.txt

```
---

## 🚀 Getting Started

Follow these instructions to get a copy of the project up and running on your local machine.

### Prerequisites

- A C++ compiler (g++, Clang, or MSVC)
- CMake (version 3.10+)
- The Emscripten SDK ([Installation Guide](https://emscripten.org/docs/getting_started/downloads.html))

### Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/YOUR_USERNAME/cpp-neural-network.git](https://github.com/jadenisaac2005/cpp-neural-network.git)
    cd cpp-neural-network
    ```
2.  **Activate the Emscripten SDK:**
    Navigate to your `emsdk` folder and run `source ./emsdk_env.sh`.

---

## 🖥️ Usage

The project has two main steps: compiling the trained C++ model to WebAssembly and running the web server.

### 1. Build and Train in C++

Build the project using CMake and run the training binary:

```bash
cmake -B build && cmake --build build
./build/main
```

### 2. Compile C++ to WebAssembly

`scripts/build_wasm.sh` rebuilds both Wasm outputs from the same four C++
sources:

- **`docs/`** — a UMD build (`MODULARIZE=1`, script-tag friendly) used by the
  GitHub Pages demo.
- **`dist/es6/`** — an ES module build (`EXPORT_ES6=1`) for bundlers, e.g. a
  Next.js client component: `import createModule from '.../network.js'`. It
  accepts a `locateFile(path, prefix)` override so the host page can serve
  `network.wasm` from wherever it likes.

```Bash
source /path/to/emsdk/emsdk_env.sh   # activate Emscripten
./scripts/build_wasm.sh
```

### 3. Run the Local Web Server

Navigate into the `/docs` folder and start a simple web server.

```Bash
cd docs
python -m http.server
```

Open your browser and go to `http://localhost:8000` to see your application live!

---

## 💡 Live Demo
https://jadenisaac2005.github.io/cpp-neural-network/

---

## 📬 Contact

Jaden Isaac – BTech AI & ML student at Amity University Bangalore. Building AI systems from scratch.

Feel free to reach out with any questions or feedback!

- **GitHub**: [github.com/jadenisaac2005](https://github.com/jadenisaac2005)
- **LinkedIn**: [linkedin.com/in/jaden-isaac](https://linkedin.com/in/jaden-isaac)
