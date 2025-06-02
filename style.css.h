const char STYLE_CSS[] PROGMEM = R"rawliteral(
@import url("https://fonts.googleapis.com/css2?family=Signika:wght@700&display=swap");
*,
::before,
::after {
  box-sizing: border-box;
}

body {
  background: #121212;
  color: #e0e0e0;
  text-align: center;
  margin: 0;
  padding: 20px;
  font-family: "Signika", sans-serif;
  font-weight: 700;
}

h1, h2 {
  margin-bottom: 30px;
  color: #fff;
}

.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 20px;
  max-width: 1200px;
  margin: auto;
}

.btn-container {
  background: #1f1f1f;
  padding: 15px;
  border-radius: 10px;
  box-shadow: 0 0 10px rgba(0,0,0,0.3);
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
}

.display-name { 
  padding: 5px 5px;
  color: #ccc;
  margin-top: 10px;
}

.btn {
  padding: 12px 0;
  font-size: 14px;
  border: none;
  border-radius: 5px;
  margin: 5px;
  cursor: pointer;
  width: 100%;
  box-sizing: border-box;
  transition: background-color 0.3s ease;
  color: #fff;
  font-weight: bold;
}

.on-button {
  background-color: #2196f3;
  color: white;
  border: none;
  padding: 10px 20px;
  border-radius: 6px;
  font-weight: bold;
  cursor: pointer;
}

.off-button {
  background-color: #555;
  color: white;
  border: none;
  padding: 10px 20px;
  border-radius: 6px;
  font-weight: bold;
  cursor: pointer;
}

.on-button:hover {
  background-color: #1976d2;
}

.off-button:hover {
  background-color: #777;
}

.config_btn, .update_btn {
  background-color: #3a3a3a;
  border: 1px solid #555;
  border-radius: 8px;
  color: #ddd;
  font-size: 18px;
  padding: 12px 20px;
  cursor: pointer;
  margin: 10px 5px;
  width: 150px;
  transition: background-color 0.3s ease;
  font-weight: 600;
}

.config_btn:hover, .update_btn:hover, .config-btn:hover {
  background-color: #555;
}

.form-container {
  width:300px;
  margin: 20px auto;
  padding: 15px;
  background-color: #121212;
  border-radius: 10px;
  box-shadow: 0 0 15px rgba(0,0,0,0.7);
  color: #ccc;
}

.form-container form {
  display: block;
  text-align: left;
}

.form-container label,
.form-container input[type="text"] {
  display: block;
  width: 100%;
}

.form-container label {
  margin-top: 15px;
  margin-bottom: 5px;
  font-weight: 600;
  color: #bbb;
}

.form-container input[type="text"] {
  padding: 10px 12px;
  border-radius: 6px;
  border: 1px solid #444;
  background-color: #222;
  color: #eee;
  font-size: 16px;
  box-sizing: border-box;
  transition: border-color 0.3s ease;
}

.form-container input[type="text"]:focus {
  outline: none;
  border-color: #4caf50;
}

.form-container input[type="submit"],
.form-container a.button {
  margin-top: 20px;
  padding: 12px 24px;
  background-color: #4caf50;
  border: none;
  border-radius: 6px;
  color: #fff;
  font-weight: 700;
  cursor: pointer;
  text-decoration: none;
  display: inline-block;
  text-align: center;
}

.form-container input[type="submit"]:hover,
.form-container a.button:hover {
  background-color: #388e3c;
}

.btn, 
input[type="submit"], 
a.button, 
.config_btn, 
.update_btn {
  display: block;
  margin: 10px auto;
  cursor: pointer;
  text-align: center;
}

.btn.active {
  box-shadow: 0 0 8px 2px #ffd700;
  // border: 2px solid #ffd700;
}

.message {
  margin-top: 20px;
  font-weight: bold;
  font-size: 18px;
  transition: opacity 0.3s ease;
  opacity: 1;
}


.config-form {
  max-width: 600px;
  margin: 0 auto;
  padding: 20px;
}

.config-form label {
  display: block;
  margin-top: 15px;
  font-weight: bold;
  color: #e9f0ee;
}

.config-form input[type="text"] {
  width: 100%;
  padding: 10px;
  margin-top: 5px;
  border-radius: 8px;
  border: 1px solid #ccc;
  font-size: 16px;
  box-sizing: border-box;
}

.config-form button[type="submit"] {
  margin-top: 25px;
  padding: 12px 25px;
  font-size: 16px;
  background-color: #007bff;
  color: white;
  border: none;
  border-radius: 8px;
  cursor: pointer;
  transition: background-color 0.3s ease;
}

.config-form button[type="submit"]:hover {
  background-color: #0056b3;
}


input[type="text"],
input[type="number"],
select {
  background-color: #1e1e1e;
  color: #ffffff;
  border: 1px solid #444;
  padding: 8px;
  border-radius: 5px;
  width: 50em;
  box-sizing: border-box;
}

input[type="time"],
select {
  background-color: #1e1e1e;
  color: #ffffff;
  border: 1px solid #444;
  padding: 8px;
  border-radius: 5px;
  width: 20em;;
  box-sizing: border-box;
}

input[type="submit"] {
  background-color: #333;
  color: #fff;
  border: 1px solid #555;
  padding: 10px 20px;
  border-radius: 5px;
  cursor: pointer;
}

input[type="submit"]:hover {
  background-color: #555;
}

label {
  display: block;
  margin-top: 10px;
  margin-bottom: 5px;
}

.schedule-container {
    max-width: 50%;
    margin: 0 auto;
  }

table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 20px;

}

table, th, td {
  border: 1px solid #444;
}

th, td {
  padding: 10px;
  text-align: left;
}

th {
  background-color: #222;
}

a {
  color: #66ccff;
  text-decoration: none;
}

a:hover {
  text-decoration: underline;
}

.bottom-buttons {
  margin-top: 20px;
  text-align: center;
  display: flex;
  justify-content: center;
  gap: 20px; /* espace entre les icônes */
}

.bottom-buttons a.settings-icon {
  font-size: 2rem;
  color: white;
  text-decoration: none;
  transition: transform 0.2s ease;
}

.toast {
    visibility: hidden;
    min-width: 250px;
    background-color: #323232;
    color: #fff;
    text-align: center;
    border-radius: 8px;
    padding: 16px;
    position: fixed;
    z-index: 1000;
    left: 50%;
    bottom: 30px;
    transform: translateX(-50%);
    font-size: 16px;
    box-shadow: 0 4px 8px rgba(0,0,0,0.2);
    transition: visibility 0s, opacity 0.5s ease-in-out;
    opacity: 0;
}

.toast.show {
    visibility: visible;
    opacity: 1;
}

input{
  -webkit-appearance: none;
  appearance: none;
}

.toggle-container.d {
  position: relative;
  margin-top: 4px;
  border-radius: 4px;
  width: 76px;
  height: 32px;
  background-image: linear-gradient(rgba(255, 255, 255, 0.125), rgba(255, 255, 255, 0.125)), url("https://assets.codepen.io/4175254/wood-pattern.png");
  background-size: 100% 100%, 328px 110px;
  background-position: center;
  box-shadow: inset 1px 0 1px rgba(73, 31, 1, 0.25), inset -1px 0 1px rgba(73, 31, 1, 0.25), 0 0 1px rgba(0, 0, 0, 0.5), 0 1px 2px rgba(0, 0, 0, 0.5);
}

.toggle-container.d .toggle-input {
  position: absolute;
  z-index: 2;
  inset: 0;
  cursor: pointer;
}
.toggle-container.d .toggle-handle {
  display: flex;
  align-items: center;
  position: relative;
  border-radius: inherit;
  padding: 8px;
  width: 70px;
  height: inherit;
  background-image: linear-gradient(90deg, rgba(73, 31, 1, 0.125), rgba(255, 255, 255, 0.25), rgba(73, 31, 1, 0.25)), url("https://assets.codepen.io/4175254/wood-pattern.png");
  background-size: 200% 100%, 328px 110px;
  background-position: 100% 0, center;
  box-shadow: inset 0 0.5px 1px #e9d38d, inset 1px 0 1px #e9d38d;
  transition-property: transform, background-position, box-shadow;
  transition-duration: 0.4s;
}
.toggle-container.d:has(:checked) .toggle-handle {
  transform: translateX(6px);
  background-position: 0 0, center;
  box-shadow: inset 0 0.5px 1px #e9d38d, inset -1px 0 1px #e9d38d;
}

.toggle-container.d .toggle-text {
  position: absolute;
  font-size: 14px;
  color: #775b40;
  text-shadow: 0 1px 1px rgba(255, 255, 255, 0.75);
  transition-property: transform, color;
  transition-duration: 0.4s;
}
.toggle-container.d .toggle-text.off {
  color: #ac4443;
}
.toggle-container.d:has(:checked) .off {
  transform: translateX(-10%) scaleX(0.8);
  color: #775b40;
}

.toggle-container.d .toggle-text.on {
  right: 8px;
  transform: translateX(10%) scaleX(0.8);
}
.toggle-container.d:has(:checked) .on {
  transform: translateX(0) scaleX(1);
  color: #5297ba;
}

@media (max-width: 650px) {
  .btn {
    font-size: 22px !important;
    padding: 18px 0;
     width: 100%;
  }

  .schedule-container {
      max-width: 90%;
  }

  h3 {
    font-size: 20px;
    margin-top: 10px;
    margin-bottom: 10px;
  }

  .btn-container {
    padding: 10px;
  }

  table, thead, tbody, th, td, tr {
    display: block;
    width: auto;
  }

  table thead {
    display: none;
  }

  table tr {
    margin-bottom: 15px;
    background-color: #1a1a1a;
    border-radius: 6px;
    padding: 10px;
  }

  table td {
    text-align: right;
    position: relative;
    padding-left: 50%;
    font-size: 0.95rem;
  }

  table td::before {
    position: absolute;
    left: 10px;
    width: 45%;
    white-space: nowrap;
    text-align: left;
    font-weight: bold;
    content: attr(data-label);
  }
}

@media (min-width: 1200px) {
  .grid {
    grid-template-columns: repeat(6, 1fr);
  }
}

)rawliteral";