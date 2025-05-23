##########################################################
# PyTorch Tutorial:  Classification CNN Model for Evaluation
#
# Author: Y.K.Kim
# mod: 2024-5-21 
#
# Descr: This example is tesing pretrined CNN model for classification
#
# Model: LeNet
# Dataset: MNIST
#
##########################################################


import torch 
import torch.nn as nn
import torch.nn.functional as F
import torchvision
import torchvision.transforms as transforms
from torch.utils.data import DataLoader
from torchvision import datasets
from torchvision.transforms import ToTensor
import numpy as np 
import matplotlib.pyplot as plt

# User defined Model
import myModel as myModel

# Model weight directory
ModelPATH='./weights/'



##########################################################
## Part 0:  GPU setting
##########################################################

# Select GPU or CPU for training.
device = "cuda" if torch.cuda.is_available() else "cpu"
print(f"Using {device} device")



##########################################################
## Part 1:  Prepare Dataset
##########################################################

# Download Dataset from TorchVision MNIST
# Once, downloaded locally, it does not download again.
#
#  MNIST shape:  1x28x28 

# transformation to tensor:  converts 0~255 value to 0~1 value.
data_transform = transforms.Compose([            
            transforms.ToTensor(),
])


# EVAL DATA
test_data = datasets.MNIST(
    root="data",
    train=False,
    download=True,
    transform=data_transform,
)

# Create DataLoader with Batch size N
# Input Dim:  [N, C, H, W]
batch_size = 64
test_dataloader = DataLoader(test_data, batch_size=batch_size, shuffle=True)

for X, y in test_dataloader:
    print(f"Shape of X [N, C, H, W]: {X.shape}")
    print(f"Shape of y: {y.shape} {y.dtype}")
    break



##########################################################
## Part 4:  Test Model - Evaluation
##########################################################

model=torch.load(ModelPATH+'MLP_MNIST_model.pth', weights_only=False)
myModel.test(test_dataloader, model, device)

##########################################################
## Part 5:  Visualize Evaluation Results
##########################################################

# Select one batch of images
dataiter = iter(test_dataloader)
images, labels = next(dataiter)
print(f"images.size: {images.size()}")

# # Plot 9 output images
# figure = plt.figure()
# num_of_images = 9

# for index in range(num_of_images):
#     plt.subplot(3, 3, index+1)
#     plt.axis('off')    
#     plt.title("Predicted: {}".format(labels[index].item()))
#     plt.imshow(images[index].cpu().numpy().squeeze(), cmap='gray_r')
# plt.show()

num_of_images = 9
# Evaluate on one batch test images
with torch.no_grad():
  for X, y in dataiter:
    X, y = X.to(device), y.to(device)
    
    # Prediction Label 
    pred = model(X)
    _, predicted = torch.max(pred.data, dim = 1)

    # Plot 9 output images
    figure = plt.figure()
    for index in range(num_of_images):
        plt.subplot(3, 3, index+1)
        plt.axis('off')
        plt.title("Predicted: {}".format(predicted[index].item()))
        plt.imshow(X[index].cpu().numpy().squeeze(), cmap='gray_r')
    plt.show()

# # Evaluate on one batch test images
# images, labels = images.to(device), labels.to(device)
# with torch.no_grad():
#     # Prediction Label WWW
#     pred = model(images)
#     _, predicted = torch.max(pred.data, dim = 1)

# Plot 9 output images
figure = plt.figure()
num_of_images = 9
for index in range(num_of_images):
    plt.subplot(3, 3, index+1)
    plt.axis('off')    
    plt.title("Predicted: {}".format(predicted[index].item()))
    plt.imshow(images[index].cpu().numpy().squeeze(), cmap='gray_r')
plt.show()