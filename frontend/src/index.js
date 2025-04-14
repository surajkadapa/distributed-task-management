import React from 'react';
import ReactDOM from 'react-dom/client'; // Updated import from 'react-dom' to 'react-dom/client'
import './index.css'; // Your global styles can go here
import App from './App';
import './styles/styles.css';

const root = ReactDOM.createRoot(document.getElementById('root')); // Create root using createRoot
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);
