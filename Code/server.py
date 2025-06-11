from flask import Flask, request, jsonify
import os
import subprocess

app = Flask(__name__)
UPLOAD_FOLDER = "uploads"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

def get_next_filename():
    existing = [f for f in os.listdir(UPLOAD_FOLDER) if f.startswith("img_") and f.endswith(".jpg")]
    numbers = [int(f[4:8]) for f in existing if f[4:8].isdigit()]
    next_num = max(numbers, default=0) + 1
    return f"img_{next_num:04d}.jpg"

@app.route("/upload", methods=["POST"])
def upload_image():
    if "file" not in request.files and not request.data:
        return jsonify({"error": "No image file provided"}), 400

    # Save incoming image
    filename = get_next_filename()
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    # Save raw bytes (ESP32 sends as image/jpeg)
    with open(filepath, "wb") as f:
        f.write(request.data)

    # Run face recognition script with the new image path
    try:
        result = subprocess.check_output(["python", "recognize_face.py", filepath], text=True).strip()
    except subprocess.CalledProcessError as e:
        return jsonify({"result": "error", "details": str(e)}), 500

    # Only return final result string like "yes" or "no"
    return jsonify({"result": result})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
