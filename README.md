<h1>🌊 Smart Buoy Network</h1>
<h3>Community-Driven Ocean & Disaster Monitoring System</h3>

<hr/>

<h2>Overview</h2>

<p>
<strong>Smart Buoy Network</strong> is a low-cost, solar-powered IoT system for nearshore and village-level ocean monitoring,
designed to complement INCOIS’s high-precision but sparse sensor infrastructure.
</p>

<p>
While INCOIS buoys and tide gauges provide scientific-grade data at select offshore locations,
micro-level hazards near fishing villages often go undetected.
This project fills that gap by deploying dense, affordable, community-operated smart buoys
that provide real-time local validation of abnormal ocean conditions.
</p>

<p><strong>Key idea:</strong><br/>
Not a replacement for INCOIS — a dense, village-level validation layer.
</p>

<hr/>

<h2>🎯 Problem Statement</h2>
<ul>
  <li>INCOIS sensors are accurate but sparse</li>
  <li>Nearshore hazards (flooding, surges, rough seas) need localized detection</li>
  <li>Citizen reports lack sensor-backed verification</li>
  <li>Existing systems are too expensive to deploy densely</li>
</ul>

<hr/>

<h2>💡 Solution</h2>
<p>
A network of low-cost smart buoys that:
</p>
<ul>
  <li>Measure wave motion, water level, pressure, tilt, and battery health</li>
  <li>Transmit data in real-time to a cloud backend</li>
  <li>Classify ocean conditions into <strong>Normal / Alert / Warning</strong></li>
  <li>Support community trust through local validation</li>
</ul>

<p>
Each buoy acts as a <strong>micro-observer</strong>, turning fishing villages into
<strong>active partners in disaster monitoring</strong>.
</p>

<hr/>

<h2>🧠 Ocean State Classification (Prototype Logic)</h2>

<table border="1" cellpadding="6" cellspacing="0">
  <thead>
    <tr>
      <th>State</th>
      <th>Conditions</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>🟢 Normal</td>
      <td>Calm waves, predicted tides</td>
      <td>Safe operations</td>
    </tr>
    <tr>
      <td>🟡 Alert</td>
      <td>Rough seas, abnormal motion</td>
      <td>Precaution required</td>
    </tr>
    <tr>
      <td>🔴 Warning</td>
      <td>Extreme motion, surge indicators</td>
      <td>Emergency response</td>
    </tr>
  </tbody>
</table>

<p><strong>Classification is based on:</strong></p>
<ul>
  <li>Sensor thresholds</li>
  <li>Temporal smoothing</li>
  <li>(Future) multi-buoy & citizen report cross-verification</li>
</ul>

<hr/>

<h2>🚀 Features</h2>

<h3>🔧 Hardware & Sensors</h3>
<ul>
  <li><strong>ESP8266 (NodeMCU)</strong> – Core controller</li>
  <li><strong>MPU6050</strong> – Wave motion, tilt, vibration</li>
  <li><strong>BMP280</strong> – Atmospheric pressure & temperature</li>
  <li><strong>Ultrasonic Sensor</strong> – Water level estimation</li>
  <li><strong>Battery Monitoring</strong> – Voltage tracking</li>
  <li><strong>Li-ion Battery + TP4056</strong> – Portable power system</li>
</ul>

<hr/>

<h3>☁️ Backend (Supabase)</h3>
<ul>
  <li>Secure REST-based ingestion</li>
  <li>PostgreSQL storage</li>
  <li>Two-tier data design:
    <ul>
      <li><code>buoy_second_data</code> → high-frequency motion data</li>
      <li><code>buoy_minute_data</code> → aggregated environmental data</li>
    </ul>
  </li>
  <li>Persistent UUID-based buoy identity</li>
  <li>Local offline buffering using <strong>LittleFS</strong></li>
</ul>

<hr/>

<h3>📊 Frontend (React)</h3>
<ul>
  <li>Real-time alert visualization</li>
  <li>Severity-based alert table</li>
  <li>Location, urgency, and response metadata</li>
  <li>Designed for easy extension to maps (Leaflet / Mapbox)</li>
</ul>

<hr/>

<h2>🧱 System Architecture</h2>
<pre>
[Sensors]
   ↓
[ESP8266 Firmware]
   ↓ HTTPS REST
[Supabase Backend]
   ↓
[React Dashboard]
</pre>

<hr/>

<h2>📂 Project Structure</h2>
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
└── README.md
</pre>

<hr/>

<h2>⚙️ Firmware Data Strategy</h2>

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
      <td>Acceleration, vibration, water level</td>
    </tr>
    <tr>
      <td>1 min</td>
      <td><code>buoy_minute_data</code></td>
      <td>Gyro, pressure, temperature, battery</td>
    </tr>
  </tbody>
</table>

<h3>Reliability</h3>
<ul>
  <li>Flash-based local backup</li>
  <li>Automatic retry on network failure</li>
  <li>HTTPS communication (BearSSL)</li>
</ul>

<hr/>

<h2>🧪 Tech Stack</h2>

<h3>Embedded</h3>
<ul>
  <li>Arduino (ESP8266)</li>
  <li>MPU6050_light</li>
  <li>Adafruit BMP280</li>
  <li>ArduinoJson</li>
  <li>LittleFS</li>
</ul>

<h3>Backend</h3>
<ul>
  <li>Supabase (PostgreSQL + REST)</li>
  <li>API key authentication</li>
  <li>HTTPS transport</li>
</ul>

<h3>Frontend</h3>
<ul>
  <li>React.js</li>
  <li>JavaScript</li>
  <li>CSS</li>
</ul>

<hr/>

<h2>🛠️ Setup Guide</h2>

<h3>1️⃣ Hardware</h3>
<ul>
  <li>Assemble circuit per diagram</li>
  <li>Ensure <strong>3.3V compatibility</strong> for ESP8266</li>
</ul>

<h3>2️⃣ Firmware</h3>
<ul>
  <li>Configure WiFi credentials</li>
  <li>Add Supabase URL & API key</li>
  <li>Flash firmware</li>
</ul>

<h3>3️⃣ Supabase</h3>
<ul>
  <li>Create tables:
    <ul>
      <li><code>buoy_second_data</code></li>
      <li><code>buoy_minute_data</code></li>
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
  <li>Coastal flood detection</li>
  <li>Rough sea alerts for fishermen</li>
  <li>Smart harbors & ports</li>
  <li>Disaster response validation</li>
  <li>Environmental research</li>
</ul>

<hr/>

<h2>🔮 Future Enhancements</h2>
<ul>
  <li>GPS drift detection</li>
  <li>Deep sleep power optimization</li>
  <li>OTA firmware updates</li>
  <li>Multi-buoy fleet management</li>
  <li>AI-based anomaly detection</li>
  <li>LoRa / mesh networking support</li>
</ul>

<hr/>

<h2>👨‍💻 Author</h2>
<p>
<strong>Sarvesh S</strong><br/>
Smart Buoy Network<br/>
A full-stack IoT + disaster-tech engineering project combining
<strong>embedded systems, cloud infrastructure, and real-time visualization</strong>
</p>
