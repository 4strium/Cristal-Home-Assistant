import speech_recognition as sr
import os

current_dir = os.getcwd()
filename = current_dir + "/enregistrement.wav"

r = sr.Recognizer()
with sr.AudioFile(filename) as source:
    audio = r.record(source)
    try:
        datafr = r.recognize_google(audio, language="fr-FR")
        print("Reconnaissance réussie : ", datafr)
    except:
        print("Ressayez s'il vous plaît...")
        datafr = "Erreur de reconnaissance"

with open("rapport.txt", "w") as file:
    file.write(datafr)
    file.write("\n")

# Vérifiez le contenu du fichier après l'écriture
with open("rapport.txt", "r") as file:
    content = file.read()
    print("Contenu du fichier :", repr(content))