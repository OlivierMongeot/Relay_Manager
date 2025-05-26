#ifndef HTML_PAGES_H
#define HTML_PAGES_H

const char SAVE_SUCCESS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0" charset="UTF-8">
  <title>Sauvegarde</title>
</head>
<body>
  <h2>Noms sauvegardés.</h2>
  <a href="/">Retour</a>
</body>
</html>
)rawliteral";

const char CONFIG_FORM_HEADER[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
 <meta name="viewport" content="width=device-width, initial-scale=1.0" charset="UTF-8">
  <title>Configuration Relais</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <h1>Configuration des relais</h1>
  
  <form id='configForm' class='config-form' action='/save' method='POST'>
)rawliteral";

const char CONFIG_FORM_FOOTER[] PROGMEM = R"rawliteral(
  <br><br>
  <input type="submit" value="Sauvegarder">
  </form>
  <a href="/">Retour</a>

 

  <div id="message" style="margin-top:15px; color:lightgreen;"></div>

  <script>
  document.addEventListener('DOMContentLoaded', function() {


    function loadLogs() {
      fetch('/get-logs')
        .then(response => response.text())
        .then(data => {
          document.getElementById('logs').textContent = data;
        })
        .catch(err => {
          document.getElementById('logs').textContent = 'Erreur lors du chargement des logs.';
        });
    }

    const form = document.getElementById('configForm');
    if (!form) {
      console.error("Formulaire introuvable !");
      return;
    }
    const messageDiv = document.getElementById('message');

    form.addEventListener('submit', function(event) {
      event.preventDefault();

      const formData = new FormData(form);

      fetch(form.action, {
        method: form.method,
        body: formData
      })
      .then(response => {
        if (!response.ok) throw new Error('Erreur réseau');
        return response.text();
      })
      .then(text => {
        messageDiv.style.color = 'lightgreen';
        messageDiv.textContent = 'Noms sauvegardés avec succès !';
        setTimeout(() => {
        messageDiv.textContent = '';
        }, 3000);
      })
      .catch(err => {
        messageDiv.style.color = 'red';
        messageDiv.textContent = 'Erreur : ' + err.message;
      });
    });
  });
   
  </script>
    <button onclick="loadLogs()">Afficher les logs</button>
    <pre id="logs" style="max-height: 300px; overflow-y: auto; border: 1px solid #ccc; padding: 10px;"></pre>
</body>
</html>
)rawliteral";

const char HOME_PAGE_HEADER[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0" charset="UTF-8">
  <title>RELAYS CONTROLER</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <h1>Contrôle des relais</h1>
  <div class='grid'>
)rawliteral";

const char HOME_PAGE_FOOTER[] PROGMEM = R"rawliteral(
 
  <script>
  function showToast(message, isError = false) {
    const toast = document.getElementById("toast");
    toast.textContent = message;
    toast.style.backgroundColor = isError ? "#d32f2f" : "#323232";
    toast.classList.add("show");
    setTimeout(() => {
      toast.classList.remove("show");
    }, 3000);
  }

  document.querySelectorAll('.btn').forEach(button => {
    button.addEventListener('click', (event) => {
      event.preventDefault();

      const relay = button.getAttribute('data-relay');
      const action = button.getAttribute('data-action');
      const relayName = button.getAttribute('data-relay-name');
      const url = `/${action}${relay}`;

      fetch(url)
        .then(response => {
          if (response.ok) {
            // Met à jour le style des boutons du même relais
            const container = button.closest('.btn-container');
            const btnOn = container.querySelector('.btn.on');
            const btnOff = container.querySelector('.btn.off');

            if (action === 'ON') {
              btnOn.classList.add('active');
              btnOff.classList.remove('active');
            } else {
              btnOff.classList.add('active');
              btnOn.classList.remove('active');
            }

          showToast(`Relais ${relayName} : ${action} envoyé avec succès !`);

          } else {
              showToast(`Erreur lors de la commande Relais ${relay}`, true);
          }
        })
        .catch(error => {
          showToast(`Erreur réseau : ${error}`, true);
        });
    });
  });

</script>
<div id="toast" class="toast">Message de notification</div>
</body>
</html>
)rawliteral";


#endif
