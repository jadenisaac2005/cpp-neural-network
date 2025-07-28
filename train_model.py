import torch
import torch.nn as nn
import torch.optim as optim
import torchvision
import torchvision.transforms as transforms
import numpy as np

# --- 1. Define the Neural Network ---
# This structure must match the one you'll build in C++
class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        # Input: 28*28 = 784, Hidden: 128, Output: 10 (digits 0-9)
        self.fc1 = nn.Linear(784, 128)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(128, 10)

    def forward(self, x):
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        return x

# --- 2. Load Data and Define Hyperparameters ---
transform = transforms.ToTensor()
trainset = torchvision.datasets.MNIST(root='./data', train=True, download=True, transform=transform)
trainloader = torch.utils.data.DataLoader(trainset, batch_size=64, shuffle=True)

model = Net()
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)
EPOCHS = 5

# --- 3. Train the Model ---
print("Starting model training...")
for epoch in range(EPOCHS):
    running_loss = 0.0
    for images, labels in trainloader:
        # Flatten the images from 28x28 to a 784-element vector
        images = images.view(images.shape[0], -1)

        optimizer.zero_grad()
        outputs = model(images)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()

    print(f"Epoch {epoch+1}/{EPOCHS}, Loss: {running_loss/len(trainloader):.4f}")

print("Finished training.")

# --- 4. Save Weights and Biases to a File ---
print("Saving weights and biases to weights.txt...")
with open('weights.txt', 'w') as f:
    params = model.state_dict()

    # Write weights and biases for each layer, flattened into a single line
    f.write(' '.join(map(str, params['fc1.weight'].numpy().flatten())) + '\n')
    f.write(' '.join(map(str, params['fc1.bias'].numpy().flatten())) + '\n')
    f.write(' '.join(map(str, params['fc2.weight'].numpy().flatten())) + '\n')
    f.write(' '.join(map(str, params['fc2.bias'].numpy().flatten())) + '\n')

print("Done. 'weights.txt' has been created.")
