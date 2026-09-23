const canvas = document.getElementById('drawingCanvas');
const ctx = canvas.getContext('2d');
const predictButton = document.getElementById('predictButton');
const clearButton = document.getElementById('clearButton');
const resultSpan = document.getElementById('result');
const probabilitiesDiv = document.getElementById('probabilities');

// Thick enough that the stroke survives being shrunk down to MNIST's 20px box.
ctx.lineWidth = 25;
ctx.lineCap = 'round';
ctx.strokeStyle = 'black';
let isDrawing = false;
let lastX = 0, lastY = 0;

function startDrawing(e) { isDrawing = true; [lastX, lastY] = [e.offsetX, e.offsetY]; }
function stopDrawing() { isDrawing = false; }
function clearCanvas() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    resultSpan.textContent = '?';
    probabilitiesDiv.innerHTML = '';
}

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

// Converts the freehand drawing on `sourceCanvas` into a 28x28 image that
// matches how MNIST digits are framed: bounding-box cropped, scaled so the
// longer side is 20px (aspect ratio kept), and centered on the ink's center
// of mass inside a 28x28 frame. Returns an ImageData, or null if the canvas
// is blank.
function preprocessToMnistFrame(sourceCanvas) {
    const W = sourceCanvas.width;
    const H = sourceCanvas.height;
    const srcCtx = sourceCanvas.getContext('2d');
    const srcData = srcCtx.getImageData(0, 0, W, H).data;

    // Ink is opaque black on a transparent background, so the alpha channel
    // alone tells us where the stroke is (0 = background, 255 = full ink) —
    // the same polarity MNIST uses (low = background, high = ink).
    const INK_THRESHOLD = 10;
    let minX = W, minY = H, maxX = -1, maxY = -1;
    for (let y = 0; y < H; y++) {
        for (let x = 0; x < W; x++) {
            const alpha = srcData[(y * W + x) * 4 + 3];
            if (alpha > INK_THRESHOLD) {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }
    }

    if (maxX < 0) return null; // Nothing drawn.

    const boxW = maxX - minX + 1;
    const boxH = maxY - minY + 1;

    // Crop to the bounding box of the ink.
    const cropCanvas = document.createElement('canvas');
    cropCanvas.width = boxW;
    cropCanvas.height = boxH;
    cropCanvas.getContext('2d').drawImage(
        sourceCanvas, minX, minY, boxW, boxH, 0, 0, boxW, boxH
    );

    // Scale so the longer side is 20px, keeping the aspect ratio.
    const scale = 20 / Math.max(boxW, boxH);
    const newW = Math.max(1, Math.round(boxW * scale));
    const newH = Math.max(1, Math.round(boxH * scale));

    const scaledCanvas = document.createElement('canvas');
    scaledCanvas.width = newW;
    scaledCanvas.height = newH;
    scaledCanvas.getContext('2d').drawImage(cropCanvas, 0, 0, newW, newH);

    // Paste into a 28x28 frame, first roughly centered by bounding box...
    const FRAME = 28;
    const finalCanvas = document.createElement('canvas');
    finalCanvas.width = FRAME;
    finalCanvas.height = FRAME;
    const finalCtx = finalCanvas.getContext('2d');

    const offsetX = Math.round((FRAME - newW) / 2);
    const offsetY = Math.round((FRAME - newH) / 2);
    finalCtx.drawImage(scaledCanvas, offsetX, offsetY);

    // ...then re-center on the ink's center of mass, as MNIST digits are.
    const frameData = finalCtx.getImageData(0, 0, FRAME, FRAME).data;
    let totalMass = 0, sumX = 0, sumY = 0;
    for (let y = 0; y < FRAME; y++) {
        for (let x = 0; x < FRAME; x++) {
            const v = frameData[(y * FRAME + x) * 4 + 3];
            totalMass += v;
            sumX += v * x;
            sumY += v * y;
        }
    }

    if (totalMass > 0) {
        const comX = sumX / totalMass;
        const comY = sumY / totalMass;
        const dx = Math.round(FRAME / 2 - comX);
        const dy = Math.round(FRAME / 2 - comY);

        finalCtx.clearRect(0, 0, FRAME, FRAME);
        finalCtx.drawImage(scaledCanvas, offsetX + dx, offsetY + dy);
    }

    return finalCtx.getImageData(0, 0, FRAME, FRAME);
}

function renderProbabilities(probs) {
    let topIdx = 0;
    for (let i = 1; i < probs.length; i++) {
        if (probs[i] > probs[topIdx]) topIdx = i;
    }

    probabilitiesDiv.innerHTML = probs.map((p, i) => {
        const pct = (p * 100).toFixed(1);
        const rowClass = i === topIdx ? 'prob-row top' : 'prob-row';
        return `<div class="${rowClass}">` +
            `<span class="prob-label">${i}</span>` +
            `<span class="prob-bar-track"><span class="prob-bar-fill" style="width:${pct}%"></span></span>` +
            `<span class="prob-value">${pct}%</span>` +
            `</div>`;
    }).join('');

    return topIdx;
}

createModule().then(async Module => {
    console.log("Wasm module loaded.");
    try {
        console.log("Creating layer sizes...");
        const layer_sizes_js = [784, 128, 10];
        const layer_sizes_cpp_vec = new Module.VectorInt();
        layer_sizes_js.forEach(size => layer_sizes_cpp_vec.push_back(size));
        console.log("Calling init_network...");
        Module.init_network(layer_sizes_cpp_vec);
        console.log("init_network done.");
        layer_sizes_cpp_vec.delete();

        // Load the pretrained weights into the Wasm module's virtual filesystem,
        // then have the C++ side read them in. Without this, the network runs
        // on whatever random weights its constructor happened to initialize.
        console.log("Fetching trained weights...");
        const weightsResponse = await fetch('network.data');
        const weightsText = await weightsResponse.text();
        Module.FS.writeFile('/network_weights.txt', weightsText);
        Module.load_network_weights('/network_weights.txt');
        console.log("Trained weights loaded.");

        predictButton.disabled = false;
        predictButton.textContent = "Predict";
        console.log("Neural Network ready.");

        predictButton.onclick = () => {
            const frame = preprocessToMnistFrame(canvas);
            if (!frame) {
                resultSpan.textContent = '?';
                probabilitiesDiv.innerHTML = '<p>Draw a digit first.</p>';
                return;
            }

            const inputArray = [];
            for (let i = 0; i < frame.data.length; i += 4) {
                inputArray.push(frame.data[i + 3] / 255.0);
            }

            const input_ptr = Module._malloc(inputArray.length * Float64Array.BYTES_PER_ELEMENT);
            const heap_array = new Float64Array(Module.HEAPU8.buffer, input_ptr, inputArray.length);
            heap_array.set(inputArray);

            // The C++ side now returns softmax probabilities for all 10 digits.
            const resultVector = Module.predict(input_ptr, inputArray.length);
            Module._free(input_ptr);

            const probs = [];
            for (let i = 0; i < resultVector.size(); i++) {
                probs.push(resultVector.get(i));
            }
            resultVector.delete();

            const prediction = renderProbabilities(probs);
            resultSpan.textContent = prediction;
        };
    } catch (e) {
        console.error("Error initializing network:", e);
    }
});

predictButton.disabled = true;
predictButton.textContent = "Loading Model...";
