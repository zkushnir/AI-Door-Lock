import cv2
import os

# ======== SETTINGS ========
person_name = "Anton"  # <-- change to your name
save_dir = os.path.join("known_faces", person_name)
os.makedirs(save_dir, exist_ok=True)

# ======== CAMERA SETUP ========
cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Error: Cannot access webcam.")
    exit()

img_count = 0
print("[INFO] Press SPACE to capture an image. ESC to exit.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame.")
        break

    cv2.imshow("Face Capture - Press SPACE", frame)
    key = cv2.waitKey(1)

    if key == 27:  # ESC key
        break
    elif key == 32:  # SPACE key
        img_path = os.path.join(save_dir, f"{person_name}_{img_count}.jpg")
        cv2.imwrite(img_path, frame)
        print(f"[INFO] Saved: {img_path}")
        img_count += 1

cap.release()
cv2.destroyAllWindows()
