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
