import React, { useState, useEffect } from 'react';
import axios from 'axios';
import './App.css';

function App() {
  const [output, setOutput] = useState([]);
  const [menuOptions, setMenuOptions] = useState({});
  const [isStarted, setIsStarted] = useState(false);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const [apiDebug, setApiDebug] = useState(null);

  // Set up axios interceptors for debugging
  useEffect(() => {
    // Add request interceptor
    axios.interceptors.request.use(
      config => {
        console.log('API Request:', config);
        return config;
      },
      error => {
        console.error('API Request Error:', error);
        return Promise.reject(error);
      }
    );

    // Add response interceptor
    axios.interceptors.response.use(
      response => {
        console.log('API Response:', response);
        return response;
      },
      error => {
        console.error('API Response Error:', error);
        return Promise.reject(error);
      }
    );
  }, []);

  const startTaskManager = async () => {
    setIsLoading(true);
    setError(null);
    setApiDebug(null);
    
    try {
      const response = await axios.get('http://127.0.0.1:5000/start');
      
      console.log('Start response data:', response.data);
      setApiDebug(JSON.stringify(response.data, null, 2));
      
      if (response.data.output) {
        setOutput(response.data.output);
        setMenuOptions(response.data.menu_options || {});
        setIsStarted(true);
      } else {
        throw new Error('Invalid response from server: Missing output');
      }
    } catch (err) {
      console.error('Error starting task manager:', err);
      setError(err.response?.data?.error || err.message || 'Failed to start task manager');
      if (err.response) {
        setApiDebug(JSON.stringify(err.response.data, null, 2));
      }
    } finally {
      setIsLoading(false);
    }
  };

  const handleOptionClick = async (option) => {
    setIsLoading(true);
    setError(null);
    setApiDebug(null);
    
    try {
      const response = await axios.post('http://127.0.0.1:5000/send_choice', { choice: option });
      
      console.log('Option response data:', response.data);
      setApiDebug(JSON.stringify(response.data, null, 2));
      
      if (response.data.output) {
        setOutput(response.data.output);
        setMenuOptions(response.data.menu_options || {});
      } else {
        throw new Error('Invalid response from server: Missing output');
      }
    } catch (err) {
      console.error('Error sending option:', err);
      setError(err.response?.data?.error || err.message || 'Failed to process option');
      if (err.response) {
        setApiDebug(JSON.stringify(err.response.data, null, 2));
      }
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="terminal-app">
      <header className="app-header">
        <h1>Distributed Task Management System</h1>
        <div className="status-indicator">
          Status: {isStarted ? 'Connected' : 'Not Connected'} 
          {isLoading && ' (Loading...)'}
        </div>
      </header>
      
      <div className="app-container">
        {!isStarted ? (
          <div className="start-container">
            <button 
              className="start-button" 
              onClick={startTaskManager}
              disabled={isLoading}
            >
              {isLoading ? 'Starting...' : 'Start Task Manager'}
            </button>
            {isLoading && <div className="loading-indicator">Connecting to task manager...</div>}
          </div>
        ) : (
          <>
            <div className="terminal-output">
              {output.length > 0 ? (
                output.map((line, index) => (
                  <div key={index} className="terminal-line">
                    {line}
                  </div>
                ))
              ) : (
                <div className="terminal-line">No output received</div>
              )}
              {isLoading && <div className="terminal-line loading">Processing...</div>}
            </div>
            
            <div className="terminal-input">
              {Object.keys(menuOptions).length > 0 ? (
                Object.entries(menuOptions).map(([key, value]) => (
                  <button
                    key={key}
                    className="option-button"
                    onClick={() => handleOptionClick(key)}
                    disabled={isLoading}
                  >
                    {key}. {value}
                  </button>
                ))
              ) : (
                <div className="no-options">No menu options available</div>
              )}
            </div>
          </>
        )}
        
        {error && (
          <div className="error-message">
            <strong>Error:</strong> {error}
          </div>
        )}
        
        {apiDebug && (
          <div className="debug-panel">
            <h3>API Response Debug:</h3>
            <pre>{apiDebug}</pre>
          </div>
        )}
      </div>
    </div>
  );
}

export default App;