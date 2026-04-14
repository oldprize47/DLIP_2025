% Using Saved Camera Parameters
clear all; close all; clc;

load cameraParams.mat

% Load image
imageFileName = 'mdr0_20160719_085950_I.mp4_20160719_091251.078.jpg';
originalImage = imread(imageFileName);

% For example, you can use the calibration data to remove effects of lens distortion.
undistortedImage = undistortImage(originalImage, cameraParams);

% Display images (raw / undistorted)
figure, imshow(originalImage)
figure, imshow(undistortedImage)