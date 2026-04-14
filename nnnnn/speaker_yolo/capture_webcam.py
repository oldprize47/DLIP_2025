import argparse
import time
from datetime import datetime
from pathlib import Path

import cv2


def ts():
    # 파일명용 타임스탬프 (충돌 거의 없음)
    return datetime.now().strftime("%Y%m%d_%H%M%S_%f")


def make_writer(out_path: Path, fps: float, frame_size: tuple[int, int]):
    """
    mp4v(.mp4)로 먼저 시도 -> 안 되면 XVID(.avi)로 폴백
    """
    out_path = out_path.with_suffix(".mp4")
    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    writer = cv2.VideoWriter(str(out_path), fourcc, fps, frame_size)

    if writer.isOpened():
        return writer, out_path

    # fallback
    out_path2 = out_path.with_suffix(".avi")
    fourcc2 = cv2.VideoWriter_fourcc(*"XVID")
    writer2 = cv2.VideoWriter(str(out_path2), fourcc2, fps, frame_size)
    if writer2.isOpened():
        return writer2, out_path2

    raise RuntimeError("VideoWriter를 열 수 없습니다. 코덱/권한 문제일 수 있어요.")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--device", type=int, default=0, help="웹캠 인덱스(내장 카메라는 보통 0)")
    parser.add_argument("--dshow", action="store_true", help="Windows에서 DirectShow로 카메라 열기(안 열릴 때 도움)")
    parser.add_argument("--width", type=int, default=1280)
    parser.add_argument("--height", type=int, default=720)
    parser.add_argument("--fps", type=float, default=30.0, help="녹화 fps(대충 30)")
    parser.add_argument("--interval", type=float, default=1.0, help="자동 저장 간격(초)")
    parser.add_argument("--mirror", action="store_true", help="좌우 반전(셀카처럼 보이게)")

    # 너 폴더 구조 기준
    parser.add_argument("--train_dir", type=str, default="images/train")
    parser.add_argument("--val_dir", type=str, default="images/val")
    parser.add_argument("--video_dir", type=str, default="videos")

    args = parser.parse_args()

    train_dir = Path(args.train_dir)
    val_dir = Path(args.val_dir)
    video_dir = Path(args.video_dir)

    for d in (train_dir, val_dir, video_dir):
        d.mkdir(parents=True, exist_ok=True)

    backend = cv2.CAP_DSHOW if args.dshow else cv2.CAP_ANY
    cap = cv2.VideoCapture(args.device, backend)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)

    if not cap.isOpened():
        raise RuntimeError("카메라를 열 수 없습니다.\n" "- 다른 앱(줌/카메라 앱)이 점유 중인지 확인\n" "- --dshow 옵션 추가\n" "- --device 1 로도 시도")

    save_dir = train_dir
    auto = False
    recording = False
    writer = None
    last_auto_time = 0.0
    saved_count = 0

    print("======================================")
    print("키 안내:")
    print("  s : 사진 저장")
    print("  a : 자동 저장 토글(기본 interval=1.0초)")
    print("  r : 녹화 토글( videos/ 아래 저장 )")
    print("  t : train 폴더로 저장 전환")
    print("  v : val   폴더로 저장 전환")
    print("  q : 종료")
    print("======================================")

    while True:
        ok, frame = cap.read()
        if not ok:
            time.sleep(0.05)
            continue

        if args.mirror:
            frame = cv2.flip(frame, 1)

        frame_raw = frame  # 저장/녹화용 (텍스트 안 얹음)
        frame_show = frame.copy()  # 화면 표시용 (텍스트 얹음)

        h, w = frame_raw.shape[:2]
        now = time.time()

        # 자동 저장
        if auto and (now - last_auto_time) >= args.interval:
            img_path = save_dir / f"img_{ts()}.jpg"
            cv2.imwrite(str(img_path), frame_raw)
            saved_count += 1
            last_auto_time = now
            print("auto saved:", img_path)

        # 녹화
        if recording and writer is not None:
            writer.write(frame_raw)

        # 화면 오버레이(저장 이미지에는 안 들어감)
        status = f"save_dir: {save_dir.as_posix()} | auto: {auto}({args.interval}s) | rec: {recording} | saved: {saved_count}"
        cv2.putText(frame_show, status, (10, 25), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2, cv2.LINE_AA)
        cv2.putText(frame_show, "[s]save [a]auto [r]rec [t]train [v]val [q]quit", (10, 55), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2, cv2.LINE_AA)

        cv2.imshow("speaker_yolo capture", frame_show)
        key = cv2.waitKey(1) & 0xFF

        if key == ord("q"):
            break

        elif key == ord("s"):
            img_path = save_dir / f"img_{ts()}.jpg"
            cv2.imwrite(str(img_path), frame_raw)
            saved_count += 1
            print("saved:", img_path)

        elif key == ord("a"):
            auto = not auto
            last_auto_time = 0.0
            print("auto =", auto)

        elif key == ord("t"):
            save_dir = train_dir
            print("save_dir ->", save_dir)

        elif key == ord("v"):
            save_dir = val_dir
            print("save_dir ->", save_dir)

        elif key == ord("r"):
            recording = not recording
            if recording:
                out_path = video_dir / f"rec_{ts()}"
                writer, real_path = make_writer(out_path, args.fps, (w, h))
                print("REC START ->", real_path)
            else:
                if writer is not None:
                    writer.release()
                    writer = None
                print("REC STOP")

    cap.release()
    if writer is not None:
        writer.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
