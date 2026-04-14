import cv2
import pyautogui

# 1. 얼굴 검출기 로딩
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

# 2. 카메라/화면 정보
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
screen_w, screen_h = pyautogui.size()

# 3. 트래커/중앙점 등 변수
tracker = None
center_x, center_y = None, None
TOL = 18  # dead zone (픽셀, 얼굴 움직임 허용 범위)
SPEED_SCALE = 0.8  # 마우스 이동 속도 배율

print("실행 후, 머리를 중앙에 두고 스페이스바(공백키)를 누르세요. (ESC로 종료)")

while True:
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.flip(frame, 1)  # 거울모드
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    key = cv2.waitKey(1) & 0xFF

    # 1) 트래커가 없으면 얼굴 검출로 시작
    if tracker is None:
        faces = face_cascade.detectMultiScale(gray, 1.1, 5)
        if len(faces) > 0:
            x, y, w, h = max(faces, key=lambda rect: rect[2] * rect[3])
            tracker = cv2.TrackerCSRT_create()
            tracker.init(frame, (x, y, w, h))
            fx, fy = x + w // 2, y + h // 2
            cv2.circle(frame, (fx, fy), 8, (0, 255, 255), -1)
            if key == ord(" "):
                center_x, center_y = fx, fy
                print(f"중앙 Calibration: ({center_x}, {center_y})")
        cv2.putText(frame, "얼굴 인식 중...(스페이스로 Calibration)", (20, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
    else:
        # 2) 트래커 동작
        success, box = tracker.update(frame)
        if success:
            x, y, w, h = [int(v) for v in box]
            fx, fy = x + w // 2, y + h // 2
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.circle(frame, (fx, fy), 8, (0, 255, 0), -1)

            # 스페이스바로 중앙 Calibration
            if key == ord(" "):
                center_x, center_y = fx, fy
                print(f"중앙 Calibration: ({center_x}, {center_y})")

            # Calibration 후에만 마우스 이동
            if center_x is not None and center_y is not None:
                dx = fx - center_x
                dy = fy - center_y
                mv_x = 0
                mv_y = 0
                if abs(dx) > TOL:
                    mv_x = int((abs(dx) - TOL) * SPEED_SCALE) * (1 if dx > 0 else -1)
                if abs(dy) > TOL:
                    mv_y = int((abs(dy) - TOL) * SPEED_SCALE) * (1 if dy > 0 else -1)
                if mv_x != 0 or mv_y != 0:
                    pyautogui.moveRel(mv_x, mv_y)
                color = (0, 255, 0) if mv_x == 0 and mv_y == 0 else (0, 0, 255)
                cv2.line(frame, (center_x, center_y), (fx, fy), color, 2)
                cv2.circle(frame, (center_x, center_y), 8, (255, 0, 0), 2)
                cv2.putText(frame, f"{mv_x=},{mv_y=}", (30, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
        else:
            tracker = None  # 트래킹 실패시 재검출

    cv2.imshow("Classic Face Mouse", frame)
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()
