# AI Door Lock

**AI Door Lock** is a smart home security project that uses facial recognition to unlock an apartment door. The system consists of an ESP32-CAM module outside the door that captures an image when a button is pressed. This image is sent to a server running on a Raspberry Pi, where a facial recognition model determines whether the face belongs to an authorized user. If verified, a second ESP32 module inside the apartment activates a servo motor to unlock the deadbolt.

## 🚪 How It Works

- 📷 **ESP32-CAM (outside)**: Captures a photo when a button is pressed
- 🌐 **Raspberry Pi server**: Receives the image and runs facial recognition
- 🔓 **ESP32 (inside)**: Receives unlock command and controls the deadbolt via a servo

---

## 🛠️ Setup Instructions

### 1. Clone This Repository

```bash
git clone https://github.com/zkushnir/AI-Door-Lock.git
cd ai-door-lock

**### 2. Create and Activate a Virtual Environment**

python -m venv venv
venv\Scripts\activate       # Windows
# OR
source venv/bin/activate    # macOS/Linux

### 3. Install Python Dependencies
pip install -r requirements.txt

### 4. Start the Server

### 5. Flash ESP32 Modules
ESP32-CAM (outside):

Connects to Wi-Fi

Captures and sends face image to the Pi server

ESP32 (inside):

Listens for unlock command via Wi-Fi

Controls a servo to unlock the deadbolt

📸 Dependencies (Server Side)
face_recognition

flask or similar web framework

requests

opencv-python

(See requirements.txt for the full list)

🔐 Security Notes
Use a secure Wi-Fi connection

Consider encrypting image transmissions between ESP32 and Raspberry Pi

Limit the face database to trusted users only

🧠 Author
Made with ❤️ by Zach Kush
📍 CMU | Smart systems for a smarter home

📄 License
MIT License © 2025 Zach Kush
