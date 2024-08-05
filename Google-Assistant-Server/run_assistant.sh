#!/bin/bash

# Récupérer les paramètres
DEVICE_ID=$1
DEVICE_MODEL_ID=$2
PHRASE=$3

# Activer l'environnement virtuel
source ~/prog/cristal-env/bin/activate

# Exécuter la commande Python avec un délai et envoyer une phrase
(
  # Lancer la commande Python
  python -m googlesamples.assistant.grpc.textinput --device-id "$DEVICE_ID" --device-model-id "$DEVICE_MODEL_ID" &
  PID=$! # Obtenir l'ID de processus de la commande

  # Attendre 0,2 seconde
  sleep 0.2

  # Envoyer la phrase au processus
  echo "$PHRASE" > /proc/$PID/fd/0
) # Fermer le groupe de commandes