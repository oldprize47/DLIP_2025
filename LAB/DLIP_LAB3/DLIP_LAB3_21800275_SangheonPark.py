import numpy as np
import cv2
from matplotlib import pyplot as plt
from scipy.optimize import curve_fit

def onChange(pos):
    pass

def func(x,a,b,c,d,e):		#curve_fitting에 사용할 함수 사전 정의
    return a*x**4 + b*x**3 + c*x**2 + d*x+e

src = []
for i in range(3):
    img = cv2.imread(f"LV{i+1}.png")
    src.append(img)

    src_arr = np.stack(src, axis=0)

    img = src_arr[i].copy()
    img1 = img.copy()

    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    plt.imshow(img_rgb)
    plt.title(f'Source - LV{i+1}')
    plt.axis('off')
    plt.show()
    # cv2.namedWindow(f'Source - {i}', 0)
    # cv2.imshow(f'Source - {i}', img)

    T2 = 200
    T1 = 50

    b, g, r = cv2.split(img)
    temp = r.copy()

    # /// Reduce noise with a kernel 3x3
    temp = cv2.GaussianBlur(temp, (3, 3), 5)

    canny_img = cv2.Canny(temp, T1, T2)
    cv2.rectangle(canny_img, (int(temp.shape[1] / 2), 0), (int(temp.shape[1]), int(temp.shape[0])), 0, cv2.FILLED)
    cv2.rectangle(canny_img, (0, 0), (int(temp.shape[1] / 4), int(temp.shape[0] * 5 / 12)), 0, cv2.FILLED)
    cv2.rectangle(canny_img, (0, 0), (int(temp.shape[1]), int(temp.shape[0] * 3 / 12)), 0, cv2.FILLED)
    pts1 = np.array([[int(temp.shape[1] / 2), 0], [int(temp.shape[1] * 5 / 12), int(temp.shape[0])], [int(temp.shape[1]), int(temp.shape[0])]])
    cv2.fillPoly(canny_img, [pts1], 0)
    pts2 = np.array([[int(temp.shape[1] * 7 / 12), 0], [int(temp.shape[1] * 4 / 12), int(temp.shape[0])], [int(temp.shape[1]), int(temp.shape[0])]])
    cv2.fillPoly(canny_img, [pts2], 0)

    element = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3)) # cv.MORPH_ELLIPSE, cv.MORPH_CROSS, cv.MORPH_RECT
    canny_img = cv2.morphologyEx(canny_img, cv2.MORPH_DILATE, element, canny_img, (-1,-1),1)     # cv.MORPH_ERODE, cv.MORPH_DILATE, cv.MORPH_OPEN, cv.MORPH_CLOSE, cv.MORPH_GRADIENT

    contour, hierarchy = cv2.findContours(canny_img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    arr_point = np.empty([0,2])

    for k in contour:
        retval = cv2.arcLength(k,0)
        if retval > 950:
            cv2.drawContours(img1, k, -1, (0,255,0), 4)
            for j in k:
                cv2.circle(img1, tuple(j[0]), 1, (255,0,0), -1)
                arr_point = np.vstack([arr_point,j[0]])

    x = arr_point[:,0]
    y = arr_point[:,1]

    popt, pcov = curve_fit(func,x,y)
    fit_x = np.linspace(min(x),max(x),5000)
    cur_y = func(fit_x,popt[0],popt[1],popt[2], popt[3], popt[4])
    score = img.shape[0] - max(cur_y)

    fit_x = fit_x.reshape(-1,1)
    cur_y = cur_y.reshape(-1,1)

    arr_cur = np.concatenate((fit_x, cur_y), axis=1)

    cv2.polylines(img_rgb, [arr_cur.astype(np.int32)], False, (0, 255, 0), 4)

    if score < 120:
        level = 3
    elif score < 250:
        level = 2
    else:
        level = 1

    cv2.rectangle(img_rgb, (int(img_rgb.shape[1]/2 - 10 ), int(img_rgb.shape[0] / 4) - 30), (int(img_rgb.shape[1]/2) + 250, int(img_rgb.shape[0] / 4) + 50), (0,255,0), 2, cv2.LINE_8)
    cv2.putText(img_rgb, f"Score : {round(score,2)}", (int(img_rgb.shape[1]/2), int(img_rgb.shape[0] / 4)), cv2.FONT_HERSHEY_DUPLEX, 1, (255,255,255), 2)
    cv2.putText(img_rgb, f"Level : {level}", (int(img_rgb.shape[1]/2 + 11), int(img_rgb.shape[0] / 4 + 40)), cv2.FONT_HERSHEY_DUPLEX, 1, (255,255,255), 2)

    # cv2.namedWindow(f"CHAIN_APPROX_SIMPLE - {i}", 0)
    # cv2.imshow(f"CHAIN_APPROX_SIMPLE - {i}", img1)

    plt.imshow(img_rgb)
    plt.title(f'Result - LV{i+1}')
    plt.axis('off')
    plt.show()
    # cv2.namedWindow(f'Final - {i}', 0)
    # cv2.imshow(f'Final - {i}', img)