<h1>🌊 Smart Buoy – Real-Time Disaster & Ocean Monitoring System</h1>

<p>
<strong>Smart Buoy</strong> is an <strong>IoT-based real-time monitoring system</strong> designed to collect ocean and environmental data and push it to
<strong>Supabase</strong> for analytics, visualization, and disaster alerting.
It combines <strong>embedded systems + cloud backend + frontend visualization</strong>,
making it a complete end-to-end engineering project.
</p>

<hr/>

<h2>🚀 Features</h2>

<h3>🔧 Hardware & Sensors</h3>
<ul>
  <li><strong>ESP8266 (NodeMCU)</strong> – Core controller with WiFi</li>
  <li><strong>MPU6050</strong> – Accelerometer & Gyroscope (wave motion, tilt, vibration)</li>
  <li><strong>BMP280</strong> – Temperature & Pressure sensing</li>
  <li><strong>Ultrasonic Sensor</strong> – Water level / wave height estimation</li>
  <li><strong>Battery Monitoring</strong> – Real-time voltage tracking</li>
  <li><strong>Li-ion Battery + TP4056</strong> – Portable & rechargeable power system</li>
</ul>

<h3>☁️ Cloud & Backend (Supabase)</h3>
<ul>
  <li>Secure REST API integration</li>
  <li>Two structured data tables:
    <ul>
      <li><code>buoy_minute_data</code> → Low-frequency aggregated data (1 min)</li>
      <li><code>buoy_second_data</code> → High-frequency real-time motion data (1 sec)</li>
    </ul>
  </li>
  <li>Persistent <strong>UUID-based buoy identity</strong></li>
  <li>Automatic retry & local backup using <strong>LittleFS</strong></li>
  <li>Schema-matching JSON inserts</li>
</ul>

<h3>📊 Frontend (React)</h3>
<ul>
  <li>Disaster Alerts visualization</li>
  <li>Tabular alert tracking with:
    <ul>
      <li>Location (latitude / longitude)</li>
      <li>Urgency levels</li>
      <li>Broadcast channels</li>
      <li>Emergency response teams</li>
    </ul>
  </li>
  <li>Easily extendable to maps (Leaflet / Mapbox)</li>
</ul>

<hr/>

<h2>🧠 System Architecture</h2>

<pre>
[Sensors]
   ↓
[ESP8266 Firmware]
   ↓ WiFi (HTTPS REST)
[Supabase Backend]
   ↓
[React Dashboard]
</pre>

<hr/>

<h2>🗂️ Project Structure</h2>

<pre>
SmartBuoy/
│
├── firmware/
│   └── smart_buoy.ino
│
├── frontend/
│   └── AlertsVisualization.jsx
│
├── hardware/
│   └── circuit_diagram.png
│
├── README.md
</pre>

<hr/>

<h2>⚙️ Firmware Details</h2>

<h3>📡 Data Transmission Strategy</h3>

<table border="1" cellpadding="6" cellspacing="0">
  <thead>
    <tr>
      <th>Interval</th>
      <th>Table</th>
      <th>Data</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>1 sec</td>
      <td><code>buoy_second_data</code></td>
      <td>Acceleration, magnitude, ultrasonic water level</td>
    </tr>
    <tr>
      <td>1 min</td>
      <td><code>buoy_minute_data</code></td>
      <td>Gyro, temperature, pressure, battery, GPS</td>
    </tr>
  </tbody>
</table>

<h3>🛡️ Reliability</h3>
<ul>
  <li>Local flash backup on network failure</li>
  <li>Persistent buoy UUID stored in LittleFS</li>
  <li>Secure HTTPS communication (BearSSL)</li>
</ul>

<hr/>

<h2>🧪 Technologies Used</h2>

<h3>Embedded</h3>
<ul>
  <li>Arduino (ESP8266)</li>
  <li>MPU6050_light</li>
  <li>Adafruit_BMP280</li>
  <li>LittleFS</li>
  <li>ArduinoJson</li>
</ul>

<h3>Backend</h3>
<ul>
  <li>Supabase (PostgreSQL + REST)</li>
  <li>HTTPS + API Key Authentication</li>
</ul>

<h3>Frontend</h3>
<ul>
  <li>React.js</li>
  <li>JavaScript</li>
  <li>CSS</li>
</ul>

<hr/>

<h2>🛠️ Setup Instructions</h2>

<h3>1️⃣ Hardware</h3>
<ul>
  <li>Assemble the circuit as per the provided diagram</li>
  <li>Ensure correct voltage levels (ESP8266 = 3.3V)</li>
</ul>

<h3>2️⃣ Firmware</h3>
<ul>
  <li>Update WiFi credentials</li>
  <li>Add Supabase URL & API key</li>
  <li>Flash firmware to ESP8266</li>
</ul>

<h3>3️⃣ Supabase</h3>
<ul>
  <li>Create tables:
    <ul>
      <li><code>buoy_minute_data</code></li>
      <li><code>buoy_second_data</code></li>
    </ul>
  </li>
  <li>Enable REST access</li>
</ul>

<h3>4️⃣ Frontend</h3>
<pre>
npm install
npm start
</pre>

<hr/>

<h2>📌 Use Cases</h2>
<ul>
  <li>Flood & tsunami early warning</li>
  <li>Coastal monitoring</li>
  <li>Smart ports & harbors</li>
  <li>Research & environmental analytics</li>
  <li>Disaster management systems</li>
</ul>

<hr/>

<h2>🔮 Future Improvements</h2>
<ul>
  <li>GPS module integration</li>
  <li>Power optimization (deep sleep)</li>
  <li>OTA firmware updates</li>
  <li>Map-based alert visualization</li>
  <li>AI-based wave anomaly detection</li>
  <li>Multi-buoy fleet management</li>
</ul>

<hr/>

<h2>👨‍💻 Author</h2>
<p>
<strong>Sarvesh S</strong><br/>
<strong>Smart Buoy</strong><br/>
Built as a full-stack IoT & disaster-tech project combining
<strong>hardware, backend, and frontend engineering</strong>.
</p>
