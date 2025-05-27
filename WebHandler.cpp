#include "WebHandler.h"
#include "html_pages.h"
#include "style.css.h"
#include <SPIFFS.h>
#include <set>  
#include "LogManager.h"

extern const char* HTTP_USERNAME;
extern const char* HTTP_PASSWORD;

WebHandler::WebHandler(
  AsyncWebServer& server,
  RelayScheduler& scheduler,
  RelayNameManager& relayManager,
  const int* relayPins,
  int relayCount
) : _server(server), _scheduler(scheduler), _relayManager(relayManager), _relayPins(relayPins), _relayCount(relayCount) {}

bool WebHandler::isAuthenticated(AsyncWebServerRequest *request) {
  if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
    request->requestAuthentication();
    return false;
  }
  return true;
}

void WebHandler::handleRoot(AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) return;

  String html = HOME_PAGE_HEADER;

  for (int i = 1; i <= _relayCount; i++) {
    String name = _relayManager.getName(i - 1);

    bool isOn = digitalRead(_relayPins[i - 1]) == LOW; // LOW = ON si logique inverse
    String checked = isOn ? "checked" : "";

    html += "<div class='btn-container'>";
    html += "<div class='toggle-container d'>";
    html += "<input class='toggle-input relay-toggle' type='checkbox' data-relay='" + String(i) + "' data-relay-name='" + name + "' " + checked + ">";
    html += "<div class='toggle-handle'>";
    html += "<p class='toggle-text off'>OFF</p>";
    html += "<p class='toggle-text on'>ON</p>";
    html += "</div></div>";
    html += "<div class='display-name'><h3>" + name + "</h3></div>";
    html += "</div>";
  }
  html += "</div>";
  html += R"rawliteral(
      <script>
      document.querySelectorAll('.relay-toggle').forEach(toggle => {
        toggle.addEventListener('change', () => {
          const relay = toggle.getAttribute('data-relay');
          const name = toggle.getAttribute('data-relay-name');
          const action = toggle.checked ? 'ON' : 'OFF';

          fetch(`/${action}${relay}`)
            .then(response => {
              if (response.ok) {
                showToast(`Relais ${name} ${action} envoyé avec succès !`);
              } else {
                showToast(`Erreur lors de la commande du relais ${name}`, true);
              }
            })
            .catch(error => {
              showToast(`Erreur réseau : ${error}`, true);
            });
        });
      });

      </script>
      )rawliteral";

      html += R"rawliteral(
          <div>
          <div class="bottom-buttons">
            <a class="settings-icon" href="/schedules" title="Voir les horaires">🗓️</a>
            <a class="settings-icon" href="/config" title="Configuration">⚙️</a>
            <a class="settings-icon" href="/update" title="Mise à jour">🔄</a>
          </div>
        )rawliteral";
      html += HOME_PAGE_FOOTER;
  request->send(200, "text/html; charset=utf-8", html);
}

void WebHandler::handleConfig(AsyncWebServerRequest *request) {


  if (!isAuthenticated(request)) return;

  AsyncResponseStream *response = request->beginResponseStream("text/html");

  response->print(CONFIG_FORM_HEADER);

  for (int i = 0; i < _relayCount; i++) {
    response->print("<label for='relay");
    response->print(i);
    response->print("'>Relais ");
    response->print(i + 1);
    response->print(" :</label>");
    response->print("<input type='text' id='relay");
    response->print(i);
    response->print("' name='relay");
    response->print(i);
    response->print("' value='");
    response->print(_relayManager.getName(i));
    response->print("'>");
  }
  response->print(CONFIG_FORM_FOOTER);

  request->send(response);

}


void WebHandler::handleSave(AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) return;

  char paramName[12]; // Assez grand pour "relay" + 2 chiffres + null-terminator

  for (int i = 0; i < _relayCount; i++) {
    snprintf(paramName, sizeof(paramName), "relay%d", i);

    if (request->hasParam(paramName, true)) {
      String newName = request->getParam(paramName, true)->value();
      _relayManager.setName(i, newName);
    }
  }

  _relayManager.saveAll();
  request->send(200, "text/html; charset=utf-8", SAVE_SUCCESS_PAGE);
}


int WebHandler::calculateDurationMinutes(uint8_t hourOn, uint8_t minOn, uint8_t hourOff, uint8_t minOff) {
  int start = hourOn * 60 + minOn;
  int end = hourOff * 60 + minOff;
  int duration = end - start;
  if (duration < 0) duration += 24 * 60;  // gère le cas où l'heure de fin est après minuit
  return duration;
}

String WebHandler::formatTime(int hour, int minute) {
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
  return String(buf);
}

void WebHandler::handleSchedule(AsyncWebServerRequest *request) {

  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Programmations</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0" charset="UTF-8">
      <link rel="stylesheet" href="/style.css">
    </head>
    <body>
      <h2>Programmations des relais</h2>
  )rawliteral";

  // Récupère les programmations
  const auto& schedules = _scheduler.getSchedules();

  // Code pour récupérer les relais uniques
  std::set<uint8_t> uniqueRelays;
  for (const auto& s : schedules) {
    uniqueRelays.insert(s.relayIndex);
  }

  html += "<div style='margin-bottom: 10px;'>";
  html += "<label for='relayFilter'>Filtrer par relais :</label> ";
  html += "<select id='relayFilter' onchange='filterTable()'>";
  html += "<option value='all'>Tous</option>";
  for (auto relay : uniqueRelays) {
    String name = _relayManager.getName(relay);
    if (name.isEmpty()) name = "Relais " + String(relay);
    html += "<option value='" + String(relay) + "'>" + name + "</option>";
  }
  html += "</select></div>";

  html += R"rawliteral(
    <div class='schedule-container'>
    <table><thead>
      <tr>
        <th>Relais</th>
        <th>Heure de début</th>
        <th>Heure de fin</th>
        <th>Action</th>
      </tr>
    </thead><tbody>
  )rawliteral";

  for (size_t i = 0; i < schedules.size(); ++i) {
    const auto& s = schedules[i];
    String relayName = _relayManager.getName(s.relayIndex);
    if (relayName.isEmpty()) relayName = "Relais " + String(s.relayIndex);
    html += "<tr data-relay='" + String(s.relayIndex) + "'><td data-label='Relais'>" + relayName + 
        "</td><td data-label=\"Heure d'allumage\">" + formatTime(s.hourOn, s.minOn) + 
        "</td><td data-label=\"Heure d'arrêt\">" + formatTime(s.hourOff, s.minOff) + 
        "</td><td data-label='Action'><a href='/deleteSchedule?index=" + String(i) + "'>Supprimer</a></td></tr>";
  }

  html += R"rawliteral(
      </tbody></table></div>
      <h3>Ajouter une programmation</h3>
      <form method='GET' action='/addSchedule'>
        <label>Relais :</label>
        <select name='relayIndex'>
  )rawliteral";

  for (int i = 0; i < _relayCount; ++i) {
    String name = _relayManager.getName(i);
    if (name.isEmpty()) name = "Relais " + String(i + 1);
    html += "<option value='" + String(i) + "'>" + name + "</option>";
  }

  html += R"rawliteral(
        </select><br>
        <label>Heure de début :</label>
        <input type='time' name='hourOn' required><br>
        <label>Heure de fin :</label>
        <input type='time' name='hourOff' required><br>
        <input type='submit' value='Ajouter'>
      </form>
      <br><a href='/'>Retour à l'accueil</a>
        )rawliteral";

  html += R"rawliteral(
    <script>
    function filterTable() {
      const selectedRelay = document.getElementById("relayFilter").value;
      const rows = document.querySelectorAll("table tbody tr");

      rows.forEach(row => {
        const relay = row.getAttribute("data-relay");
        row.style.display = (selectedRelay === "all" || selectedRelay === relay) ? "" : "none";
      });
    }
    </script>
  )rawliteral";

  html += R"rawliteral(
    </body>
    </html>
  )rawliteral";

  request->send(200, "text/html", html);
}

void WebHandler::handleDeleteSchedule(AsyncWebServerRequest *request) {
  if (!request->hasParam("index")) {
    request->send(400, "text/plain", "Paramètre 'index' manquant");
    return;
  }

  int index = request->getParam("index")->value().toInt();
  _scheduler.removeSchedule(index); 

  request->redirect("/schedules");
}

void WebHandler::handleAddSchedule(AsyncWebServerRequest *request) {
  if (!request->hasParam("relayIndex") || !request->hasParam("hourOn") || !request->hasParam("hourOff")) {
    request->send(400, "text/plain", "Paramètres manquants");
    return;
  }

  int relay = request->getParam("relayIndex")->value().toInt();

  String timeOnStr = request->getParam("hourOn")->value();  // format "HH:MM"
  int hourOn = timeOnStr.substring(0, 2).toInt();
  int minOn = timeOnStr.substring(3).toInt();

  String timeOffStr = request->getParam("hourOff")->value();  // format "HH:MM"
  int hourOff = timeOffStr.substring(0, 2).toInt();
  int minOff = timeOffStr.substring(3).toInt();

  _scheduler.addRelay(relay, hourOn, minOn, hourOff, minOff);

  request->redirect("/schedules");
}

void WebHandler::handleRelayStatus(AsyncWebServerRequest *request) {
  DynamicJsonDocument doc(1024);
  JsonArray states = doc.createNestedArray("relays");

  for (int i = 0; i < _relayCount; i++) {
    JsonObject relay = states.createNestedObject();
    relay["id"] = i + 1;
    relay["name"] = _relayManager.getName(i);
    relay["state"] = digitalRead(_relayPins[i]) == LOW; // true = ON
  }

  String json;
  serializeJson(doc, json);
  request->send(200, "application/json", json);
}


void WebHandler::setupRoutes() {

  _server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", STYLE_CSS);
  });

  _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleRoot(request);
  });

  _server.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleConfig(request);
  });

  _server.on("/save", HTTP_POST, [this](AsyncWebServerRequest *request) {
    handleSave(request);
  });

  _server.on("/schedules", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleSchedule(request);
  });

  _server.on("/deleteSchedule", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleDeleteSchedule(request);
  });

  _server.on("/addSchedule", HTTP_GET, [this](AsyncWebServerRequest *request) {
    handleAddSchedule(request);
  });

  _server.on("/get-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", LogManager::getLogs());
    });

  _server.on("/clear-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
      LogManager::clearLogs();
      request->send(200, "text/plain", "Logs effacés.");
    });
  
  _server.on("/relay-status", HTTP_GET, std::bind(&WebHandler::handleRelayStatus, this, std::placeholders::_1));

  for (int i = 1; i <= _relayCount; i++) {
      _server.on(("/ON" + String(i)).c_str(), HTTP_GET, [this, i](AsyncWebServerRequest *request) {
      digitalWrite(_relayPins[i - 1], LOW);
      // if (websocketEnabled) {
      //       WsManager::notifyAllClientsRelayStates();
      //     }
      request->send(200, "application/json", "{\"status\":\"OK\"}");
        });
      _server.on(("/OFF" + String(i)).c_str(), HTTP_GET, [this, i](AsyncWebServerRequest *request) {
      digitalWrite(_relayPins[i - 1], HIGH);
      // if (websocketEnabled) {
      //       WsManager::notifyAllClientsRelayStates();
      //     }
      request->send(200, "application/json", "{\"status\":\"OK\"}");
         });
  } 

}
