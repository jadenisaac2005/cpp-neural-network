// Get references to our HTML elements
const canvas = document.getElementById('drawingCanvas');
const ctx = canvas.getContext('2d');
const predictButton = document.getElementById('predictButton');
const clearButton = document.getElementById('clearButton');
const resultSpan = document.getElementById('result');

// Setup canvas properties
ctx.lineWidth = 20; // Make the drawing line thick
ctx.lineCap = 'round';
ctx.strokeStyle = 'black';

let isDrawing = false;
let lastX = 0;
let lastY = 0;

// Drawing functions
function startDrawing(e) {
    isDrawing = true;
    [lastX, lastY] = [e.offsetX, e.offsetY];
}

function draw(e) {
    if (!isDrawing) return;
    ctx.beginPath();
    ctx.moveTo(lastX, lastY);
    ctx.lineTo(e.offsetX, e.offsetY);
    ctx.stroke();
    [lastX, lastY] = [e.offsetX, e.offsetY];
}

function stopDrawing() {
    isDrawing = false;
}

function clearCanvas() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    resultSpan.textContent = '?';
}

// Add event listeners for drawing
canvas.addEventListener('mousedown', startDrawing);
canvas.addEventListener('mousemove', draw);
canvas.addEventListener('mouseup', stopDrawing);
canvas.addEventListener('mouseout', stopDrawing);

// Add event listener for the clear button
clearButton.addEventListener('click', clearCanvas);

// --- WebAssembly and Neural Network Logic ---
let nn; // This will hold our C++ Network object

// Load the Wasm module
createModule().then(Module => {
    console.log("Wasm module loaded.");
    try {
        // Create an instance of our C++ Network class
        // The architecture [784, 128, 10] must match the trained model
        nn = new Module.Network(new Module.VectorInt([784, 128, 10]));

        // Load the pre-trained weights
        nn.load_weights("weights.txt");

        predictButton.disabled = false;
        predictButton.textContent = "Predict";
        console.log("Neural Network ready.");
    } catch (e) {
        console.error("Error initializing network:", e);
    }
});

// Disable the predict button until the model is ready
predictButton.disabled = true;
predictButton.textContent = "Loading Model...";

// Predict button click handler
predictButton.onclick = () => {
    // 1. Get the image data from the large canvas
    const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);

    // 2. Create a temporary small canvas to downsample the image
    const tempCanvas = document.createElement('canvas');
    tempCanvas.width = 28;
    tempCanvas.height = 28;
    const tempCtx = tempCanvas.getContext('2d');

    // 3. Draw the large image onto the small canvas (this does the downsampling)
    tempCtx.drawImage(canvas, 0, 0, 28, 28);

    // 4. Get the pixel data from the small canvas
    const smallImageData = tempCtx.getImageData(0, 0, 28, 28);
    const inputArray = [];

    // 5. Convert the image data to a normalized grayscale array
    for (let i = 0; i < smallImageData.data.length; i += 4) {
        // The alpha channel (data[i+3]) determines the pixel's value
        inputArray.push(smallImageData.data[i + 3] / 255.0);
    }

    // 6. Call the C++ predict function
    try {
        const resultVector = nn.predict(inputArray);

        // 7. Find the index of the highest value in the result
        let maxVal = -Infinity;
        let prediction = -1;
        for (let i = 0; i < resultVector.size(); i++) {
            if (resultVector.get(i) > maxVal) {
                maxVal = resultVector.get(i);
                prediction = i;
            }
        }

        // 8. Display the prediction
        resultSpan.textContent = prediction;
        resultVector.delete(); // Important: free the memory used by the C++ vector

    } catch (e) {
        console.error("Error during prediction:", e);
    }
};
