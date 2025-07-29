const canvas = document.getElementById('drawingCanvas');
const ctx = canvas.getContext('2d');
const predictButton = document.getElementById('predictButton');
const clearButton = document.getElementById('clearButton');
const resultSpan = document.getElementById('result');

ctx.lineWidth = 20;
ctx.lineCap = 'round';
ctx.strokeStyle = 'black';
let isDrawing = false;
let lastX = 0, lastY = 0;

function startDrawing(e) { isDrawing = true; [lastX, lastY] = [e.offsetX, e.offsetY]; }
function stopDrawing() { isDrawing = false; }
function clearCanvas() { ctx.clearRect(0, 0, canvas.width, canvas.height); resultSpan.textContent = '?';}

function draw(e) {
    if (!isDrawing) return;
    ctx.beginPath();
    ctx.moveTo(lastX, lastY);
    ctx.lineTo(e.offsetX, e.offsetY);
    ctx.stroke();
    [lastX, lastY] = [e.offsetX, e.offsetY];
}

canvas.addEventListener('mousedown', startDrawing);
canvas.addEventListener('mousemove', draw);
canvas.addEventListener('mouseup', stopDrawing);
canvas.addEventListener('mouseout', stopDrawing);
clearButton.addEventListener('click', clearCanvas);

createModule().then(Module => {
    console.log("Wasm module loaded.");
    try {
        const layer_sizes_js = [784, 128, 10];
        const layer_sizes_cpp_vec = new Module.VectorInt();
        layer_sizes_js.forEach(size => layer_sizes_cpp_vec.push_back(size));

        Module.init_network(layer_sizes_cpp_vec);
        layer_sizes_cpp_vec.delete();

        Module.load_network_weights("weights.txt");

        predictButton.disabled = false;
        predictButton.textContent = "Predict";
        console.log("Neural Network ready.");

        predictButton.onclick = () => {
            // Process the image data into a flat array of numbers
            const tempCanvas = document.createElement('canvas');
            tempCanvas.width = 28; tempCanvas.height = 28;
            const tempCtx = tempCanvas.getContext('2d');
            tempCtx.drawImage(canvas, 0, 0, 28, 28);
            const smallImageData = tempCtx.getImageData(0, 0, 28, 28);

            const inputArray = []; // This will be a standard JavaScript array
            for (let i = 0; i < smallImageData.data.length; i += 4) {
                inputArray.push(smallImageData.data[i + 3] / 255.0);
            }

            // --- New Memory Management Logic ---
            const input_ptr = Module._malloc(inputArray.length * Float64Array.BYTES_PER_ELEMENT);

            // Create a typed array view into the Wasm memory
            const heap_array = new Float64Array(Module.HEAPU8.buffer, input_ptr, inputArray.length);

            // Copy the JavaScript data into the Wasm memory
            heap_array.set(inputArray);

            // Call the C++ function with the pointer and size
            const resultVector = Module.predict(input_ptr, inputArray.length);

            // Free the memory we allocated
            Module._free(input_ptr);
            // --- End of New Logic ---

            // Process the result (this part is the same as before)
            let maxVal = -Infinity;
            let prediction = -1;
            for (let i = 0; i < resultVector.size(); i++) {
                const val = resultVector.get(i);
                if (val > maxVal) {
                    maxVal = val;
                    prediction = i;
                }
            }

            resultSpan.textContent = prediction;
            resultVector.delete();
        };
    } catch (e) {
        console.error("Error initializing network:", e);
    }
});

predictButton.disabled = true;
predictButton.textContent = "Loading Model...";
