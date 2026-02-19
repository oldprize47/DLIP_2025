import argparse
from pathlib import Path

import cv2
from ultralytics import YOLO


BASE_COLOR = (0, 200, 255)    # orange-ish in BGR
CUSTOM_COLOR = (0, 255, 120)  # green in BGR


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run two YOLO models on one webcam stream and draw both outputs."
    )
    parser.add_argument("--source", default="0", help="Webcam index or video path (default: 0)")
    parser.add_argument("--base_model", default="yolov8n.pt", help="Base model path")
    parser.add_argument(
        "--custom_model",
        default="runs/detect/speaker_exp1/weights/best.pt",
        help="Custom model path",
    )
    parser.add_argument("--device", default="0", help="CUDA device id or 'cpu'")
    parser.add_argument("--imgsz", type=int, default=640, help="Inference image size")
    parser.add_argument("--conf_base", type=float, default=0.25, help="Base model confidence threshold")
    parser.add_argument(
        "--conf_custom", type=float, default=0.25, help="Custom model confidence threshold"
    )
    parser.add_argument("--iou", type=float, default=0.45, help="IoU threshold")
    parser.add_argument("--width", type=int, default=1280, help="Capture width")
    parser.add_argument("--height", type=int, default=720, help="Capture height")
    parser.add_argument("--mirror", action="store_true", help="Mirror webcam frame")
    parser.add_argument("--dshow", action="store_true", help="Use DirectShow backend on Windows")
    return parser.parse_args()


def resolve_source(source: str):
    return int(source) if source.isdigit() else source


def ensure_model_path(path_str: str) -> Path:
    path = Path(path_str)
    if not path.exists():
        raise FileNotFoundError(f"Model not found: {path}")
    return path


def get_class_name(names, cls_id: int) -> str:
    if isinstance(names, dict):
        return names.get(cls_id, str(cls_id))
    if isinstance(names, list) and 0 <= cls_id < len(names):
        return names[cls_id]
    return str(cls_id)


def draw_detections(frame, result, prefix: str, color, conf_thr: float) -> int:
    if result.boxes is None:
        return 0

    drawn = 0
    for box in result.boxes:
        conf = float(box.conf[0])
        if conf < conf_thr:
            continue

        cls_id = int(box.cls[0])
        name = get_class_name(result.names, cls_id)
        x1, y1, x2, y2 = [int(round(v)) for v in box.xyxy[0].tolist()]
        label = f"{prefix}:{name} {conf:.2f}"

        cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
        (tw, th), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
        text_top = max(0, y1 - th - 6)
        cv2.rectangle(frame, (x1, text_top), (x1 + tw + 6, text_top + th + 6), color, -1)
        cv2.putText(
            frame,
            label,
            (x1 + 3, text_top + th + 1),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            (0, 0, 0),
            1,
            cv2.LINE_AA,
        )
        drawn += 1

    return drawn


def predict_frame(model: YOLO, frame, conf: float, iou: float, imgsz: int, device: str):
    results = model.predict(
        source=frame,
        conf=conf,
        iou=iou,
        imgsz=imgsz,
        device=device,
        verbose=False,
    )
    return results[0]


def main():
    args = parse_args()

    base_model_path = ensure_model_path(args.base_model)
    custom_model_path = ensure_model_path(args.custom_model)
    source = resolve_source(args.source)

    backend = cv2.CAP_DSHOW if args.dshow else cv2.CAP_ANY
    cap = cv2.VideoCapture(source, backend)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)

    if not cap.isOpened():
        raise RuntimeError("Failed to open camera/source. Try --dshow or another --source index.")

    base_model = YOLO(str(base_model_path))
    custom_model = YOLO(str(custom_model_path))

    print("Running dual-model webcam inference.")
    print("  Base model  :", base_model_path)
    print("  Custom model:", custom_model_path)
    print("  Key: q (or ESC) to quit")

    while True:
        ok, frame = cap.read()
        if not ok:
            continue

        if args.mirror:
            frame = cv2.flip(frame, 1)

        base_result = predict_frame(
            model=base_model,
            frame=frame,
            conf=args.conf_base,
            iou=args.iou,
            imgsz=args.imgsz,
            device=args.device,
        )
        custom_result = predict_frame(
            model=custom_model,
            frame=frame,
            conf=args.conf_custom,
            iou=args.iou,
            imgsz=args.imgsz,
            device=args.device,
        )

        view = frame.copy()
        base_count = draw_detections(view, base_result, "COCO", BASE_COLOR, args.conf_base)
        custom_count = draw_detections(view, custom_result, "CUSTOM", CUSTOM_COLOR, args.conf_custom)

        info = f"COCO:{base_count}  CUSTOM:{custom_count}  device:{args.device}  q:quit"
        cv2.putText(view, info, (10, 26), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2, cv2.LINE_AA)
        cv2.putText(view, "COCO=orange  CUSTOM=green", (10, 52), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2, cv2.LINE_AA)

        cv2.imshow("dual yolo webcam", view)
        key = cv2.waitKey(1) & 0xFF
        if key in (ord("q"), 27):
            break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
