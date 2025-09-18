import React from 'react';

const AlertsVisualization = () => {
  // Mock data based on the provided table schema
  const mockAlerts = [
    {
      alert_id: '123e4567-e89b-12d3-a456-426614174000',
      alert_type: 'Flood',
      urgency: 5,
      location: { lat: 28.6139, lng: 77.2090 },
      city: 'Delhi',
      state: 'Delhi',
      created_at: '2023-10-01T10:00:00Z',
      possible_end: '2023-10-02T10:00:00Z',
      message: 'Heavy flooding in low-lying areas. Evacuate immediately.',
      broadcast_channels: ['SMS', 'Email', 'TV'],
      teams_dispatched: ['team-uuid-1', 'team-uuid-2']
    },
    {
      alert_id: '223e4567-e89b-12d3-a456-426614174001',
      alert_type: 'Earthquake',
      urgency: 4,
      location: { lat: 19.0760, lng: 72.8777 },
      city: 'Mumbai',
      state: 'Maharashtra',
      created_at: '2023-10-02T08:30:00Z',
      possible_end: null,
      message: 'Magnitude 6.5 earthquake detected. Stay indoors.',
      broadcast_channels: ['Radio', 'App Notification'],
      teams_dispatched: ['team-uuid-3']
    },
    {
      alert_id: '323e4567-e89b-12d3-a456-426614174002',
      alert_type: 'Cyclone',
      urgency: 3,
      location: { lat: 13.0827, lng: 80.2707 },
      city: 'Chennai',
      state: 'Tamil Nadu',
      created_at: '2023-10-03T14:00:00Z',
      possible_end: '2023-10-05T14:00:00Z',
      message: 'Cyclone approaching. Secure your property.',
      broadcast_channels: ['SMS', 'TV'],
      teams_dispatched: []
    }
  ];

  return (
    <div style={{ padding: '20px', fontFamily: 'Arial, sans-serif' }}>
      <h2>Disaster Alerts Visualization</h2>
      <table style={{ width: '100%', borderCollapse: 'collapse', marginTop: '20px' }}>
        <thead>
          <tr style={{ backgroundColor: '#f2f2f2' }}>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Alert ID</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Type</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Urgency</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Location</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>City</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>State</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Created At</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Possible End</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Message</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Broadcast Channels</th>
            <th style={{ border: '1px solid #ddd', padding: '8px', textAlign: 'left' }}>Teams Dispatched</th>
          </tr>
        </thead>
        <tbody>
          {mockAlerts.map((alert) => (
            <tr key={alert.alert_id}>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.alert_id}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.alert_type}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.urgency}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>
                Lat: {alert.location.lat}, Lng: {alert.location.lng}
              </td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.city}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.state}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{new Date(alert.created_at).toLocaleString()}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>
                {alert.possible_end ? new Date(alert.possible_end).toLocaleString() : 'N/A'}
              </td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.message}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.broadcast_channels.join(', ')}</td>
              <td style={{ border: '1px solid #ddd', padding: '8px' }}>{alert.teams_dispatched.join(', ')}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};

export default AlertsVisualization;
