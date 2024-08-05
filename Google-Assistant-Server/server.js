const express = require('express');
const { exec } = require('child_process');
const app = express();

// Clé API pour sécuriser les requêtes
const API_KEY = 'VOTRE_CLE_API_GENERATED';  // Remplacez par la clé API générée

app.use(express.json());

// Middleware pour vérifier la clé API
app.use((req, res, next) => {
  const apiKey = req.header('x-api-key');
  if (apiKey !== API_KEY) {
    return res.status(403).send('Accès refusé');
  }
  next();
});

app.post('/execute', (req, res) => {
  const deviceId = req.body.deviceId;
  const deviceModelId = req.body.deviceModelId;
  const phrase = req.body.phrase; // Nouvelle phrase à envoyer

  // Vérification des paramètres
  if (!deviceId || !deviceModelId || !phrase) {
    return res.status(400).send('Paramètres manquants : deviceId, deviceModelId ou phrase');
  }

  // Construire la commande avec les paramètres
  const command = `./run_assistant.sh ${deviceId} ${deviceModelId} "${phrase}"`;

  exec(command, { shell: '/bin/bash' }, (error, stdout, stderr) => {
    if (error) {
      return res.status(500).send(`Erreur d'exécution : ${error.message}`);
    }
    if (stderr) {
      return res.status(500).send(`Erreur de commande : ${stderr}`);
    }
    res.send(stdout);
  });
});

const PORT = 3000;  // Choisissez le port que vous souhaitez utiliser
app.listen(PORT, () => {
  console.log(`Serveur en écoute sur le port ${PORT}`);
});
