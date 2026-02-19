import cv2
import mediapipe as mp
import pyautogui
import time

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
screen_w, screen_h = pyautogui.size()

mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp_face_mesh.FaceMesh(refine_landmarks=False, max_num_faces=1)

TOL = 0.03
SPEED_SCALE = 180
MAX_PIXELS = 80

center_nose_x, center_nose_y = None, None

print("실행 후, 머리를 중앙에 두고 스페이스바(공백키)를 누르세요. (ESC로 종료)")

last_time = time.time()
frame_count = 0

while True:
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.flip(frame, 1)
    h, w, _ = frame.shape
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_mesh.process(rgb)

    key = cv2.waitKey(1) & 0xFF

    frame_count += 1
    if frame_count % 30 == 0:
        now = time.time()
        fps = 30 / (now - last_time)
        print(f"FPS: {fps:.1f}")
        last_time = now

    vx = vy = 0

    if results.multi_face_landmarks:
        lm = results.multi_face_landmarks[0]
        cx, cy = int(lm.landmark[1].x * w), int(lm.landmark[1].y * h)

        if key == ord(" "):
            center_nose_x, center_nose_y = cx, cy
            print(f"중앙 기준점이 설정되었습니다: ({center_nose_x}, {center_nose_y})")

        if center_nose_x is not None and center_nose_y is not None:
            dx = (cx - center_nose_x) / w
            dy = (cy - center_nose_y) / h

            if abs(dx) > TOL:
                vx = int((abs(dx) - TOL) * SPEED_SCALE) * (1 if dx > 0 else -1)
            if abs(dy) > TOL:
                vy = int((abs(dy) - TOL) * SPEED_SCALE) * (1 if dy > 0 else -1)

            vx = max(-MAX_PIXELS, min(MAX_PIXELS, vx))
            vy = max(-MAX_PIXELS, min(MAX_PIXELS, vy))

            # 스무딩: pyautogui 이동을 3번으로 나눠서 호출
            SMOOTH_STEPS = 3
            if vx != 0 or vy != 0:
                for i in range(SMOOTH_STEPS):
                    pyautogui.moveRel(vx // SMOOTH_STEPS, vy // SMOOTH_STEPS)
                    # time.sleep(0.001)  # 너무 짧으면 생략 가능

            color = (0, 255, 0) if vx == 0 and vy == 0 else (0, 0, 255)
            cv2.circle(frame, (cx, cy), 8, color, -1)
            cv2.line(frame, (center_nose_x, center_nose_y), (cx, cy), color, 2)
            cv2.putText(frame, f"{vx=},{vy=}", (30, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
            cv2.circle(frame, (center_nose_x, center_nose_y), 8, (255, 0, 0), 2)
        else:
            cv2.putText(frame, "스페이스바로 중앙 Calibration!", (30, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
            cv2.circle(frame, (cx, cy), 8, (0, 255, 255), -1)
    else:
        cv2.putText(frame, "얼굴을 인식 중...", (30, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

    cv2.imshow("Head Mouse Proportional", frame)
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()
