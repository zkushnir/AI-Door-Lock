import face_recognition
import os
import pickle

known_faces_root = "known_faces"  # Root folder containing subfolders per person
known_encodings = []
known_names = []

for person_name in os.listdir(known_faces_root):
    person_dir = os.path.join(known_faces_root, person_name)
    if not os.path.isdir(person_dir):
        continue  # Skip files, only folders

    print(f"Processing folder: {person_name}")

    for filename in os.listdir(person_dir):
        if filename.lower().endswith(('.png', '.jpg', '.jpeg')):
            filepath = os.path.join(person_dir, filename)
            print(f"  Processing file: {filepath}")

            image = face_recognition.load_image_file(filepath)
            encodings = face_recognition.face_encodings(image)

            if len(encodings) > 0:
                known_encodings.append(encodings[0])
                known_names.append(person_name)
                print(f"    Added encoding for {person_name}")
            else:
                print(f"    No faces found in {filename}, skipping.")

# Save all known encodings and names
data = {
    "encodings": known_encodings,
    "names": known_names
}

with open("known_faces.pkl", "wb") as f:
    pickle.dump(data, f)

print("Pickle file 'known_faces.pkl' created successfully.")
