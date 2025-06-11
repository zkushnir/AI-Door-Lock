import face_recognition
import os
import sys
import pickle

# Load known face encodings
with open("known_faces.pkl", "rb") as f:
    data = pickle.load(f)
    known_encodings = data["encodings"]  # <-- extract just the list of face encodings
    known_names = data["names"]          # <-- if you want to use the names too


# Get uploaded image path from argument
if len(sys.argv) < 2:
    print("error:no_path")
    sys.exit(1)

path = sys.argv[1]
if not os.path.exists(path):
    print("error:invalid_path")
    sys.exit(1)

# Load uploaded image and find encodings
image = face_recognition.load_image_file(path)
locations = face_recognition.face_locations(image)
encodings = face_recognition.face_encodings(image, locations)

# Assume default result is no match
result = "no"

# Check for match
for encoding in encodings:
    matches = face_recognition.compare_faces(known_encodings, encoding, tolerance=0.5)
    if any(matches):
        result = "yes"
        break

# Output result to stdout
print(result)
