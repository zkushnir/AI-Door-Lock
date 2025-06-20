import face_recognition
import os
import sys
import pickle

# Load known face encodings and names
with open("known_faces.pkl", "rb") as f:
    data = pickle.load(f)
    known_encodings = data["encodings"]
    known_names = data["names"]

# Get uploaded image path from argument
if len(sys.argv) < 2:
    print("error:no_path")
    sys.exit(1)

path = sys.argv[1]
if not os.path.exists(path):
    print("error:invalid_path")
    sys.exit(1)

# Load uploaded image and find face locations and encodings
image = face_recognition.load_image_file(path)
locations = face_recognition.face_locations(image)
encodings = face_recognition.face_encodings(image, locations)

# Default result if no match
result = "Not authorised"

# Check each encoding for matches
for encoding in encodings:
    matches = face_recognition.compare_faces(known_encodings, encoding, tolerance=0.5)
    if any(matches):
        # Find the first matched index and get the corresponding name
        first_match_index = matches.index(True)
        matched_name = known_names[first_match_index]
        result = f"Welcome {matched_name}"
        break

# Print the result
print(result)
