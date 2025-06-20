from flask import Flask, request, jsonify
import os
import subprocess
import shutil
from datetime import datetime

app = Flask(__name__)
UPLOAD_FOLDER = "uploads"
KNOWN_FACES_DIR = "known_faces"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

def get_next_filename(folder="uploads"):
    existing = [f for f in os.listdir(folder) if f.startswith("img_") and f.endswith(".jpg")]
    numbers = [int(f[4:8]) for f in existing if f[4:8].isdigit()]
    next_num = max(numbers, default=0) + 1
    return f"img_{next_num:04d}.jpg"

@app.route("/upload", methods=["POST"])
def upload_image():
    if "file" not in request.files and not request.data:
        return jsonify({"error": "No image file provided"}), 400

    # Save uploaded image
    filename = get_next_filename()
    filepath = os.path.join(UPLOAD_FOLDER, filename)
    with open(filepath, "wb") as f:
        f.write(request.data)

    # Run face recognition script
    try:
        result = subprocess.check_output(["python", "recognize_face.py", filepath], text=True).strip()
    except subprocess.CalledProcessError as e:
        return jsonify({"result": "error", "details": str(e)}), 500

    # Save recognized images to the matching known_faces folder
    if result.startswith("Welcome"):
        name = result.split("Welcome ")[1].strip()
        dest_folder = os.path.join(KNOWN_FACES_DIR, name)
        os.makedirs(dest_folder, exist_ok=True)

        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        dest_path = os.path.join(dest_folder, f"auto_{timestamp}.jpg")
        shutil.copy(filepath, dest_path)
        print(f"Saved image to {dest_path}")

    return jsonify({"result": result})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
