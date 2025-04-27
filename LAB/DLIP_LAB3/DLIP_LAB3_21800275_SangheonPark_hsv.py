import numpy as np
import cv2
from matplotlib import pyplot as plt
from scipy.optimize import curve_fit

def onChange(pos):
    pass

src = []
for i in range(3):
    img = cv2.imread(f"LV{i+1}.png")
    src.append(img)

src_arr = np.stack(src, axis=0)

img = src_arr[1].copy()
img1 = img.copy()
img2 = img.copy()

T2 = 200
T1 = 50

cv2.namedWindow('source', cv2.WINDOW_NORMAL)
cv2.imshow('source',img)

cv2.namedWindow("Trackbar_Windows",0)
cv2.createTrackbar("threshold2", "Trackbar_Windows", 0, 255, onChange)
cv2.createTrackbar("threshold1", "Trackbar_Windows", 0, 255, onChange)

cv2.setTrackbarPos("threshold2", "Trackbar_Windows", T2)
cv2.setTrackbarPos("threshold1", "Trackbar_Windows", T1)

b, g, r = cv2.split(img)
temp = r.copy()

# /// Reduce noise with a kernel 3x3
temp = cv2.GaussianBlur(temp, (5, 5), 10)
cv2.imshow("GaussianBlur", temp)
element = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3)) # cv.MORPH_ELLIPSE, cv.MORPH_CROSS, cv.MORPH_RECT

while cv2.waitKey(1) != ord('q'):

    T2 = cv2.getTrackbarPos("threshold2", "Trackbar_Windows")
    T1 = cv2.getTrackbarPos("threshold1", "Trackbar_Windows")
    canny_img = cv2.Canny(temp, T1, T2)
    cv2.rectangle(canny_img, (int(temp.shape[1] / 2), 0), (int(temp.shape[1]), int(temp.shape[0])), 0, cv2.FILLED)
    cv2.rectangle(canny_img, (0, 0), (int(temp.shape[1] / 4), int(temp.shape[0] * 5 / 12)), 0, cv2.FILLED)
    cv2.rectangle(canny_img, (0, 0), (int(temp.shape[1]), int(temp.shape[0] * 3 / 12)), 0, cv2.FILLED)
    # pts1 = np.array([[int(temp.shape[1] / 2), 0], [int(temp.shape[1] * 1 / 3), int(temp.shape[0])], [int(temp.shape[1]), int(temp.shape[0])]])
    # cv2.fillPoly(canny_img, [pts1], 0)
    # pts2 = np.array([[int(temp.shape[1] * 9 / 12), 0], [int(temp.shape[1] * 3 / 12), int(temp.shape[0])], [int(temp.shape[1]), int(temp.shape[0])]])
    # cv2.fillPoly(canny_img, [pts2], 0)
    pts1 = np.array([[int(temp.shape[1] / 2), 0], [int(temp.shape[1] * 5 / 12), int(temp.shape[0])], [int(temp.shape[1]), int(temp.shape[0])]])
    cv2.fillPoly(canny_img, [pts1], 0)
    pts2 = np.array([[int(temp.shape[1] * 7 / 12), 0], [int(temp.shape[1] * 4 / 12), int(temp.shape[0])], [int(temp.shape[1]), int(temp.shape[0])]])
    cv2.fillPoly(canny_img, [pts2], 0)
    canny_img = cv2.morphologyEx(canny_img, cv2.MORPH_DILATE, element, canny_img, (-1,-1),1)     # cv.MORPH_ERODE, cv.MORPH_DILATE, cv.MORPH_OPEN, cv.MORPH_CLOSE, cv.MORPH_GRADIENT
    cv2.imshow("Trackbar_Windows", canny_img)

contour, hierarchy = cv2.findContours(canny_img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

contour2, hierarchy = cv2.findContours(canny_img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# print(f'도형의 갯수: {len(contour), len(contour2)}')

count = 0

cv2.namedWindow('CHAIN_APPROX_NONE', 0)

arr_point = np.empty([0,2])

for i in contour:
    retval = cv2.arcLength(i,0)

    if retval > 900:
        cv2.drawContours(img1, i, -1, (0,255,0), 4)
        count = count + 1
        # cv2.imshow('CHAIN_APPROX_NONE', img1)

cv2.namedWindow('CHAIN_APPROX_NONE', 0)
cv2.namedWindow('CHAIN_APPROX_SIMPLE', 0)
count = 0

for i in contour2:
    retval = cv2.arcLength(i,0)
    if retval > 800:
        print(f"contour Length: {retval}")
        if retval > 970:
            print(f"contour Length: {retval}")
            cv2.drawContours(img2, i, -1, (0,255,0), 4)
            count = count + 1
            for j in i:
                cv2.circle(img2, tuple(j[0]), 1, (255,0,0), -1)
                arr_point = np.vstack([arr_point,j[0]])
             
        # cv2.imshow('CHAIN_APPROX_SIMPLE', img2)
print("arr_point")
print(arr_point)
# for i in contour:

def func(x,a,b,c,d,e):		#curve_fitting에 사용할 함수 사전 정의
    return a*x**4 + b*x**3 + c*x**2 + d*x+e

x = arr_point[:,0]
y = arr_point[:,1]
popt, pcov = curve_fit(func,x,y)
print("x")
print(x)
print("y")
print(y)
fit_x = np.linspace(min(x),max(x),5000)
cur_y = func(fit_x,popt[0],popt[1],popt[2], popt[3], popt[4])
maxPoint = max(cur_y)
# print("minx")
# print(min(x))
# print("Maxx")
# print(max(x))

fit_x = fit_x.reshape(-1,1)
# print("fit_x")
# print(fit_x.shape)
cur_y = cur_y.reshape(-1,1)
# print("cur_y")
# print(cur_y.shape)

arr_cur = np.concatenate((fit_x, cur_y), axis=1)
print("arr_cur shape")
print(arr_cur.shape)

cv2.polylines(img, [arr_cur.astype(np.int32)], False, (0, 255, 0), 4)

print(f"maxPoint: {maxPoint}")
# plt.plot(x,cur_y)
# cv2.circle(img, (int(img.shape[1] /  5), maxPoint), 1, (255,0,0))

# cv2.drawContours(img2, contour2, -1, (0,255,0), 4)

# 컨투어 꼭지점 좌표를 작은 파랑색 점(원)으로 표시 ---⑨
# for i in contour2:
#     for j in i:
#         cv2.circle(img2, tuple(j[0]), 1, (255,0,0), -1) 
# cv2.namedWindow('CHAIN_APPROX_SIMPLE', 0)

cv2.imshow('CHAIN_APPROX_NONE', img1)
cv2.imshow("CHAIN_APPROX_SIMPLE", img2)

cv2.namedWindow('Final', 0)
cv2.imshow('Final', img)

while True:
    key = cv2.waitKey(1) & 0xFF
    if key == 27:  # ESC key
        break

cv2.destroyAllWindows()