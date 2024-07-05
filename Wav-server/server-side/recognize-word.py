import speech_recognition as sr
import os

current_dir = os.getcwd()

filename = current_dir + "/get_wav/enregistrement.wav"

r = sr.Recognizer()
with sr.AudioFile(filename) as source:
    audio = r.record(source)
    try:
        datafr = r.recognize_google(audio, language="fr-FR")
    except:
        print("Ressayez s'il vous plaît...")

file = open("rapport.txt", "w")
file.write(datafr + 2*"\n")

file.close()