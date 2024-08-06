#!/bin/bash

# Récupérer les paramètres
DEVICE_ID=$1
DEVICE_MODEL_ID=$2
PHRASE=$3

# Activer l'environnement virtuel
source ~/prog/cristal-env/bin/activate
echo "Environnement activé."

# Obtenir la date et l'heure actuelle
CURRENT_DATETIME=$(date '+%d-%m-%Y %H:%M:%S')

# Construire la commande complète
COMMAND="python -m googlesamples.assistant.grpc.textinput --device-id $DEVICE_ID --device-model-id $DEVICE_MODEL_ID"

# Écrire la commande et la phrase dans le fichier de log avec l'horodatage
echo "[$CURRENT_DATETIME] Command: $COMMAND, Phrase: \"$PHRASE\"" >> command_log.txt

# Exécuter le script expect
expect ./send_command.exp "$DEVICE_ID" "$DEVICE_MODEL_ID" "$PHRASE"