'''
@ Deep Learning & Image Processing  by Young-Keun Kim - Handong Global University

Author          : Sangheon Park
Created         : 2025-04-22
Modified        : 2025-04-30
Language/ver	: Python
Course		: Deep Learning & Image Processing

Description     : DLIP_LAB3_21800275_SangheonPark.cpp

'''
import numpy as np
import cv2 as cv
from matplotlib import pyplot as plt
from scipy.optimize import curve_fit

def mask_ROI(canny_img):
    # 3 Boxes
    cv.rectangle(canny_img, (int(canny_img.shape[1] / 2), 0), (int(canny_img.shape[1]), int(canny_img.shape[0])), 0, cv.FILLED)
    cv.rectangle(canny_img, (0, 0), (int(canny_img.shape[1] / 4), int(canny_img.shape[0] * 5 / 12)), 0, cv.FILLED)
    cv.rectangle(canny_img, (0, 0), (int(canny_img.shape[1]), int(canny_img.shape[0] * 3 / 12)), 0, cv.FILLED)

    # 2 triangles

    pts1 = np.array([[int(canny_img.shape[1] / 2), 0], [int(canny_img.shape[1] * 5 / 12), int(canny_img.shape[0])], [int(canny_img.shape[1]), int(canny_img.shape[0])]])
    cv.fillPoly(canny_img, [pts1], 0) # 1
    pts2 = np.array([[int(canny_img.shape[1] * 7 / 12), 0], [int(canny_img.shape[1] * 4 / 12), int(canny_img.shape[0])], [int(canny_img.shape[1]), int(canny_img.shape[0])]])
    cv.fillPoly(canny_img, [pts2], 0) # 2

def valid_cont_point(contour):
    arr_point = np.empty([0,2])

    for k in contour:
        retval = cv.arcLength(k,0)
        if retval > 950:
            cv.drawContours(cont_img, k, -1, (0,255,0), 4)
            for j in k:
                cv.circle(cont_img, tuple(j[0]), 1, (255,0,0), -1)
                arr_point = np.vstack([arr_point,j[0]])
    return arr_point

def get_curve_fit_point(arr_point):
    global maxpoint
    x = arr_point[:,0]
    y = arr_point[:,1]

    popt, pcov = curve_fit(func,x,y)
    fit_x = np.linspace(min(x),max(x),5000)
    cur_y = func(fit_x,popt[0],popt[1],popt[2], popt[3], popt[4])

    maxpoint = max(cur_y)

    fit_x = fit_x.reshape(-1,1)
    cur_y = cur_y.reshape(-1,1)
    
    return fit_x, cur_y

def det_level(score):
    if score < 120:
        level = 3  
    elif score < 250:
        level = 2
    else:
        level = 1
    return level

def write_text(img, score, level):
    cv.rectangle(img, (int(img.shape[1]/2 - 10 ), int(img.shape[0] / 4) - 30), (int(img.shape[1]/2) + 250, int(img.shape[0] / 4) + 50), (0,255,0), 2, cv.LINE_8)
    cv.putText(img, f"Score : {score}", (int(img.shape[1]/2), int(img.shape[0] / 4)), cv.FONT_HERSHEY_DUPLEX, 1, (255,255,255), 2)
    cv.putText(img, f"Level : {level}", (int(img.shape[1]/2 + 11), int(img.shape[0] / 4 + 40)), cv.FONT_HERSHEY_DUPLEX, 1, (255,255,255), 2)

def func(x,a,b,c,d,e):		#curve_fitting func
    return a*x**4 + b*x**3 + c*x**2 + d*x+e

# ----------------------------- Main --------------------------------
src = []
for i in range(3):
    img = cv.imread(f"LV{i+1}.png")
    src.append(img)

    src_arr = np.stack(src, axis=0)

    img = src_arr[i].copy()
    cont_img = img.copy()

    # img_rgb = cv.cvtColor(img, cv.COLOR_BGR2RGB)
    cv.namedWindow(f'Source - {i}', 0)
    cv.imshow(f'Source - {i}', img)

    b, g, r = cv.split(img)
    temp = r.copy()

    # /// Reduce noise with a kernel 3x3
    blured = cv.GaussianBlur(temp, (3, 3), 5)

    canny_img = cv.Canny(blured, 50, 200)

    mask_ROI(canny_img)

    # cv.namedWindow(f'mask_ROI - {i}', 0)
    # cv.imshow(f'mask_ROI - {i}', canny_img)

    element = cv.getStructuringElement(cv.MORPH_ELLIPSE, (3,3)) # cv.MORPH_ELLIPSE, cv.MORPH_CROSS, cv.MORPH_RECT
    morp_img = cv.morphologyEx(canny_img, cv.MORPH_DILATE, element)     # cv.MORPH_ERODE, cv.MORPH_DILATE, cv.MORPH_OPEN, cv.MORPH_CLOSE, cv.MORPH_GRADIENT

    contour, hierarchy = cv.findContours(morp_img, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    arr_point = valid_cont_point(contour)

    fit_x, cur_y = get_curve_fit_point(arr_point)

    #Draw Curve_fit Func on Image
    arr_cur = np.concatenate((fit_x, cur_y), axis=1)
    cv.polylines(img, [arr_cur.astype(np.int32)], False, (0, 255, 0), 4)

    score = round(float(img.shape[0] - maxpoint), 2)
    level = det_level(score)

    write_text(img, score, level)

    cv.namedWindow(f'Final - {i}', 0)
    cv.imshow(f'Final - {i}', img)

while True:
    key = cv.waitKey(0) & 0xFF
    if key == 27:  # ESC key
        break

cv.destroyAllWindows()