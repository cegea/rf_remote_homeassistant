// html_content.h
#ifndef html_content_h
#define html_content_h

/**
 * HTML strings to be posted by the server when in AP WiFi mode.
 * 
 */
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang='en'>
<head>
  <meta charset='utf-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <title>Wifi Setup</title>
  <style>
    *,::after,::before { box-sizing: border-box; }
    body { margin: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; 
           font-size: 1rem; font-weight: 400; line-height: 1.5; color: #212529; background-color: #f5f5f5; }
    .form-control { display: block; width: 100%; height: calc(1.5em + .75rem + 2px); border: 1px solid #ced4da; }
    button { cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; 
             padding: .5rem 1rem; font-size: 1.25rem; line-height: 1.5; border-radius: .3rem; width: 100%; }
    .form-signin { width: 100%; max-width: 400px; padding: 15px; margin: auto; }
    h1 { text-align: center; }
  </style>
</head>
<body>
  <main class='form-signin'>
    <form action='/' method='post'>
      <h1 class=''>Wifi Setup</h1><br/>
      <div class='form-floating'>
        <label>SSID</label>
        <input type='text' class='form-control' name='ssid'>
        <label>Password</label>
        <input type='password' class='form-control' name='password'>
      </div>
      <div class='form-floating'>
        <br/>
        <label>SSID 2 (Optional)</label>
        <input type='text' class='form-control' name='ssid2'>
        <label>Password</label>
        <input type='password' class='form-control' name='password2'>
      </div>
      
      <div class='form-floating'>
        <br/>
        <label>MQTT Host</label>
        <input type='text' class='form-control' name='mqtt_host'>
      </div>
      <div class='form-floating'>
        <br/>
        <label>MQTT Port</label>
        <input type='number' class='form-control' name='mqtt_port'>
      </div>
      <div class='form-floating'>
        <br/>
        <label>MQTT User</label>
        <input type='text' class='form-control' name='mqtt_user'>
      </div>
      <div class='form-floating'>
        <br/>
        <label>MQTT Password</label>
        <input type='password' class='form-control' name='mqtt_pswd'>
      </div>
      <br/><br/>
      <button type='submit'>Save</button>
    </form>
  </main>
</body>
</html>
)rawliteral";

#endif
