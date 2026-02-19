from ultralytics import YOLO

model = YOLO("yolo26n.pt")
results = model.predict(source=0, show=True)  # 0 = webcam
