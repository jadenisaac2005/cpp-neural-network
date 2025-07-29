# ✍️ C++ Neural Network from Scratch (with WebAssembly)

A handwritten digit recognizer built from scratch in C++ to demonstrate a fundamental understanding of neural network mechanics. The trained model is compiled to **WebAssembly (Wasm)**, allowing it to run at near-native speed directly in a web browser with an interactive HTML/JavaScript frontend.

![App Screenshot](https://i.postimg.cc/xTpxTyj1/Screenshot-2025-07-29-at-12-44-10-PM.png)

---

## ✨ Features

- **Neural Network From Scratch**: The entire network—including matrix operations, layers, and the feedforward process—is implemented in C++ without external ML libraries.
- **High-Performance Inference**: All prediction logic runs in a pre-compiled WebAssembly module, making it incredibly fast.
- **Interactive Canvas**: Users can draw a digit with their mouse for instant prediction.
- **Hybrid Workflow**: The model is trained offline in Python using PyTorch for fast experimentation, while inference is handled by the high-performance C++ code.

---

## 🛠️ Tech Stack

- **C++ (17)**: For the core neural network implementation.
- **CMake**: To manage the C++ build process.
- **Python / PyTorch**: For training the model and generating the weights.
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
│
├── .gitignore
├── CMakeLists.txt
├── train_model.py        # Python script for training
└── weights.txt           # Output of the training script

```
---

## 🚀 Getting Started

Follow these instructions to get a copy of the project up and running on your local machine.

### Prerequisites

- A C++ compiler (g++, Clang, or MSVC)
- CMake (version 3.10+)
- Python 3.8+
- The Emscripten SDK ([Installation Guide](https://emscripten.org/docs/getting_started/downloads.html))

### Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/YOUR_USERNAME/cpp-neural-network.git](https://github.com/jadenisaac2005/cpp-neural-network.git)
    cd cpp-neural-network
    ```
2.  **Install Python dependencies:**
    ```bash
    pip install torch torchvision
    ```
3.  **Activate the Emscripten SDK:**
    Navigate to your `emsdk` folder and run `source ./emsdk_env.sh`.

---

## 🖥️ Usage

The project has three main steps: training the model, compiling the C++ to Wasm, and running the web server.

### 1. Train the Python Model

First, you need to run the training script. This will generate the `weights.txt` file that the C++ application will use.

```bash
python train_model.py
```

### 2. Compile C++ to WebAssembly

Next, run the Emscripten (`emcc`) command to compile your C++ code into `.wasm` and `.js` files and place them in the `/docs` folder.

```Bash
emcc -O3 --bind -o docs/network.js \
src/Matrix.cpp src/Layer.cpp src/Network.cpp src/bindings.cpp \
--preload-file weights.txt \
-s MODULARIZE=1 -s EXPORT_NAME="createModule" \
-s ALLOW_MEMORY_GROWTH=1 \
-s "EXPORTED_FUNCTIONS=['_malloc','_free']" \
-s "EXPORTED_RUNTIME_METHODS=['HEAPU8']" \
-I include
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
(Add your live GitHub Pages link here once deployed)

---

## 📬 Contact

Jaden Isaac – A B.Tech AI & ML student passionate about building useful projects and exploring the world of technology.

Feel free to reach out with any questions or feedback!

- **GitHub**: [github.com/jadenisaac2005](https://github.com/jadenisaac2005)
- **LinkedIn**: [linkedin.com/in/jaden-isaac](https://linkedin.com/in/jaden-isaac)
