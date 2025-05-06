"""
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : Sangheon Park
Created         : 2025-04-22
Modified        : 2025-05-04
Language/ver	: Python
Course		: Deep Learning & Image Processing

Description     : DLIP_LAB3_21800275_SangheonPark.py

"""

import numpy as np
import cv2 as cv
from matplotlib import pyplot as plt
from scipy.optimize import curve_fit


# ----------------------------- Functions -------------------------------- #
def mask_ROI(canny_img):
    # Mask out non‑interest zones with 3 rectangles
    cv.rectangle(canny_img, (int(canny_img.shape[1] * 0.5), 0), (int(canny_img.shape[1]), int(canny_img.shape[0])), 0, cv.FILLED)
    cv.rectangle(canny_img, (0, 0), (int(canny_img.shape[1] * 0.25), int(canny_img.shape[0] * 0.41)), 0, cv.FILLED)
    cv.rectangle(canny_img, (0, 0), (int(canny_img.shape[1]), int(canny_img.shape[0] * 0.25)), 0, cv.FILLED)

    # Remove diagonal noise with two triangular masks
    pts1 = np.array([[int(canny_img.shape[1] * 0.5), 0], [int(canny_img.shape[1] * 0.41), int(canny_img.shape[0])], [int(canny_img.shape[1]), int(canny_img.shape[0])]])
    cv.fillPoly(canny_img, [pts1], 0)  # 1
    pts2 = np.array([[int(canny_img.shape[1] * 0.58), 0], [int(canny_img.shape[1] * 0.33), int(canny_img.shape[0])], [int(canny_img.shape[1]), int(canny_img.shape[0])]])
    cv.fillPoly(canny_img, [pts2], 0)  # 2


def Morphology(img):
    # Vertical kernel to erase horizontal streaks
    element1 = cv.getStructuringElement(cv.MORPH_RECT, (1, 3))  # cv.MORPH_ELLIPSE, cv.MORPH_CROSS, cv.MORPH_RECT
    # Large ellipse to reconnect broken edges
    element2 = cv.getStructuringElement(cv.MORPH_ELLIPSE, (7, 7))
    morp_img = cv.morphologyEx(img, cv.MORPH_OPEN, element1, iterations=1)  # cv.MORPH_ERODE, cv.MORPH_DILATE, cv.MORPH_OPEN, cv.MORPH_CLOSE, cv.MORPH_GRADIENT
    morp_img = cv.morphologyEx(morp_img, cv.MORPH_DILATE, element2, iterations=4)
    return morp_img


def valid_cont_point(contours, cont_img):
    arr_point = np.empty([0, 2])
    for cont in contours:
        cont_len = cv.arcLength(cont, 0)
        if cont_len > 500:  # Ignore tiny contours (likely noise)
            cv.drawContours(cont_img, [cont], -1, (0, 255, 0), 2)
            for j in cont:
                if j[0][0] > int(cont_img.shape[1] * 0.25) and j[0][1] < int(cont_img.shape[0] * 0.41):  # Skip points inside masked ROI
                    continue
                cv.circle(cont_img, tuple(j[0]), 1, (255, 0, 0), -1)
                arr_point = np.vstack([arr_point, j[0]])
    return arr_point


def func(x, a, b, c):  # curve_fitting func
    return a * x**2 + b * x + c


def get_curve_fit_point(arr_point):
    global maxpoint
    x = arr_point[:, 0]
    y = arr_point[:, 1]

    popt, pcov = curve_fit(func, x, y)  # Fit a quadratic curve to edge points
    fit_x = np.linspace(min(x), max(x), 5000)
    cur_y = func(fit_x, popt[0], popt[1], popt[2])

    maxpoint = max(cur_y)  # Highest y‑value gives sag depth

    fit_x = fit_x.reshape(-1, 1)
    cur_y = cur_y.reshape(-1, 1)

    return fit_x, cur_y


# Convert score to tension level (3=low tension)
def det_level(score):
    if score < 120:
        level = 3
    elif score < 250:
        level = 2
    else:
        level = 1
    return level


# Overlay score and level on the frame
def write_text(img, score, level):
    cv.rectangle(img, (int(img.shape[1] * 0.5 - 10), int(img.shape[0] * 0.25) - 30), (int(img.shape[1] * 0.5) + 250, int(img.shape[0] * 0.25) + 50), (0, 255, 0), 2, cv.LINE_8)
    cv.putText(img, f"Score : {score}", (int(img.shape[1] * 0.5), int(img.shape[0] * 0.25)), cv.FONT_HERSHEY_DUPLEX, 1, (255, 255, 255), 2)
    cv.putText(img, f"Level : {level}", (int(img.shape[1] * 0.5 + 11), int(img.shape[0] * 0.25 + 40)), cv.FONT_HERSHEY_DUPLEX, 1, (255, 255, 255), 2)
    # Draw reference lines for tension levels
    cv.line(img, (0, int(img.shape[0]) - 250), (int(img.shape[1]), int(img.shape[0]) - 250), (255, 255, 0), 2, cv.LINE_8)
    cv.line(img, (0, int(img.shape[0]) - 120), (int(img.shape[1]), int(img.shape[0]) - 120), (0, 255, 255), 2, cv.LINE_8)


# ----------------------------- Main -------------------------------- #
video = cv.VideoCapture("LAB3_Video.mp4")

# for recording ------
w = round(video.get(cv.CAP_PROP_FRAME_WIDTH))
h = round(video.get(cv.CAP_PROP_FRAME_HEIGHT))
fps = video.get(cv.CAP_PROP_FPS)
fourcc = cv.VideoWriter_fourcc(*"mp4v")
out = cv.VideoWriter("final_video.mp4", fourcc, fps, (w, h))
if not out.isOpened():
    print("File open failed!")
    video.release()
    exit()
# for recording ------

while video.isOpened():
    check, img = video.read()
    if not check:  # Break when no more frames are available
        print("Frame is done")
        break

    cont_img = img.copy()

    cv.namedWindow(f"Source", 0)
    cv.imshow(f"Source", img)

    b, g, r = cv.split(img)
    temp = r.copy()

    # Reduce sensor noise before Canny edge detection
    blured = cv.GaussianBlur(temp, (3, 3), 1)
    # Detect edges from the red channel
    canny_img = cv.Canny(blured, 50, 90)

    mask_ROI(canny_img)

    morp_img = Morphology(canny_img)

    contours, hierarchy = cv.findContours(morp_img, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    arr_point = valid_cont_point(contours, cont_img)

    fit_x, cur_y = get_curve_fit_point(arr_point)

    # Draw fitted curve on the image
    arr_cur = np.concatenate((fit_x, cur_y), axis=1)
    cv.polylines(img, [arr_cur.astype(np.int32)], False, (0, 255, 0), 4)

    score = round(float(img.shape[0] - maxpoint), 2)
    level = det_level(score)

    write_text(img, score, level)

    cv.namedWindow(f"Final", 0)
    cv.imshow(f"Final", img)

    # recording video
    out.write(img)

    # Process frames until video ends or 'esc' pressed
    if cv.waitKey(25) & 0xFF == 27:
        print("quit video")
        break

video.release()
out.release()
cv.destroyAllWindows()
