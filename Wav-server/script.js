function uploadFile() {
  // Récupérer le fichier WAV sélectionné par l'utilisateur
  let fileInput = document.getElementById('fileInput');
  let file = fileInput.files[0];

  if (!file) {
      alert("Veuillez sélectionner un fichier WAV.");
      return;
  }

  // Créer un objet FormData pour envoyer le fichier via AJAX
  let formData = new FormData();
  formData.append('file', file);

  // Envoyer le fichier au serveur via AJAX
  $.ajax({
      url: 'http://178.128.174.115/',
      type: 'POST',
      data: formData,
      processData: false,
      contentType: false,
      success: function(response) {
          alert("Le fichier a été téléchargé avec succès !");
          console.log(response);
      },
      error: function(xhr, status, error) {
          alert("Une erreur s'est produite lors du téléchargement du fichier : " + error);
          console.error(error);
      }
  });
}
