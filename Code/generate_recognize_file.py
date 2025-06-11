import face_recognition
import os
import pickle

known_faces_dir = "known_faces/zach"  # <-- Updated to your specific folder
known_encodings = []
known_names = []

for filename in os.listdir(known_faces_dir):
    if filename.lower().endswith(('.png', '.jpg', '.jpeg')):
        filepath = os.path.join(known_faces_dir, filename)
        print(f"Processing {filepath}...")

        image = face_recognition.load_image_file(filepath)
        encodings = face_recognition.face_encodings(image)

        if len(encodings) > 0:
            known_encodings.append(encodings[0])
            # Use filename without extension as the person's name
            name = os.path.splitext(filename)[0]
            known_names.append(name)
            print(f"Added encoding for {name}")
        else:
            print(f"No faces found in {filename}, skipping.")

# Save both encodings and names into a dictionary
data = {
    "encodings": known_encodings,
    "names": known_names
}

with open("known_faces.pkl", "wb") as f:
    pickle.dump(data, f)

print("Pickle file 'known_faces.pkl' created successfully.")
