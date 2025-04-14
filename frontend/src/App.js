// src/App.js
import React, { useState, useEffect } from 'react';
import './styles/styles.css';

function App() {
  const [tasks, setTasks] = useState([]);
  const [nodes, setNodes] = useState([]);
  const [notifications, setNotifications] = useState([]);
  const [isAutoRefresh, setIsAutoRefresh] = useState(true); // Auto-refresh enabled by default
  
  useEffect(() => {
    // Initial data fetch
    fetchData();
    
    // Set up auto-refresh
    let intervalId = null;
    
    if (isAutoRefresh) {
      intervalId = setInterval(() => {
        fetchData();
      }, 2000); // Refresh every 5 seconds
    }
    
    // Clean up interval on component unmount or when auto-refresh is disabled
    return () => {
      if (intervalId) clearInterval(intervalId);
    };
  }, [isAutoRefresh]); // Re-establish interval when isAutoRefresh changes

  const fetchData = async () => {
    try {
      await Promise.all([fetchTasks(), fetchNodes()]);
    } catch (err) {
      showNotification(`Error refreshing data: ${err}`, 'error');
    }
  };

  const showNotification = (message, type = 'success') => {
    setNotifications((prev) => [...prev, { message, type, id: Date.now() }]);
    setTimeout(() => {
      setNotifications((prev) => prev.filter((n) => n.message !== message));
    }, 5000);
  };

  const addNode = async () => {
    try {
      const res = await fetch("http://localhost:18080/add_node", { method: "POST" });
      const data = await res.json();
      showNotification(data.message || "Node added successfully!");
      fetchNodes();
    } catch (err) {
      showNotification(`Failed to add node: ${err}`, 'error');
    }
  };

  const addTask = async () => {
    try {
      const res = await fetch("http://localhost:18080/add_task", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          name: "Task-" + Math.floor(Math.random() * 1000),
          duration: Math.floor(Math.random() * 20) + 5,
        }),
      });
      const data = await res.json();
      showNotification(data.message || "Task added successfully!");
      fetchTasks();
    } catch (err) {
      showNotification(`Failed to add task: ${err}`, 'error');
    }
  };

  const fetchTasks = async () => {
    try {
      const res = await fetch("http://localhost:18080/tasks");
      const data = await res.json();
      setTasks(Array.isArray(data) ? data : []);
    } catch (err) {
      console.error(`Failed to fetch tasks: ${err}`);
      // Don't show notification on auto-refresh errors to avoid spam
    }
  };
  
  const fetchNodes = async () => {
    try {
      const res = await fetch("http://localhost:18080/nodes");
      const data = await res.json();
      setNodes(Array.isArray(data) ? data : []);
    } catch (err) {
      console.error(`Failed to fetch nodes: ${err}`);
      // Don't show notification on auto-refresh errors to avoid spam
    }
  };
  
  const toggleAutoRefresh = () => {
    setIsAutoRefresh(!isAutoRefresh);
    showNotification(`Auto-refresh ${!isAutoRefresh ? 'enabled' : 'disabled'}`);
  };

  const getStatusBadge = (status) => {
    switch (status) {
      case 0:
        return <span className="badge badge-pending"><i className="fas fa-clock"></i> Pending</span>;
      case 1:
        return <span className="badge badge-running"><i className="fas fa-spinner fa-spin"></i> Running</span>;
      case 2:
        return <span className="badge badge-completed"><i className="fas fa-check"></i> Completed</span>;
      default:
        return <span className="badge badge-pending"><i className="fas fa-question"></i> Unknown</span>;
    }
  };

  return (
    <div className="container">
      {/* Header */}
      <div className="header">
        <h1><i className="fas fa-brain"></i> Task Manager Dashboard</h1>
        <div className="last-updated">
          Last updated: {new Date().toLocaleTimeString()}
          {isAutoRefresh && <span className="auto-refresh-indicator"> (Auto-refresh on)</span>}
        </div>
      </div>

      {/* Controls */}
      <div className="controls">
        <button className="btn btn-primary" onClick={addNode}>
          <i className="fas fa-server"></i> Add Node
        </button>
        <button className="btn btn-primary" onClick={addTask}>
          <i className="fas fa-tasks"></i> Add Dummy Task
        </button>
        <button className="btn btn-success" onClick={fetchData}>
          <i className="fas fa-sync-alt"></i> Refresh Now
        </button>
        <button 
          className={`btn ${isAutoRefresh ? 'btn-warning' : 'btn-secondary'}`} 
          onClick={toggleAutoRefresh}>
          <i className={`fas ${isAutoRefresh ? 'fa-pause-circle' : 'fa-play-circle'}`}></i>
          {isAutoRefresh ? ' Pause Auto-refresh' : ' Enable Auto-refresh'}
        </button>
      </div>

      {/* Notifications */}
      <div className="notifications">
        {notifications.map((notif) => (
          <div key={notif.id || Math.random()} className={`notification ${notif.type}`}>
            <i className={notif.type === 'success' ? 'fas fa-check-circle' : 'fas fa-exclamation-circle'}></i>
            <div>{notif.message}</div>
          </div>
        ))}
      </div>

      {/* Task Section */}
      <div className="card" id="taskSection">
        <div className="card-header">
          <h2><i className="fas fa-clipboard-list"></i> Task List</h2>
        </div>
        <div className="card-body">
          {tasks.length === 0 ? (
            <div className="empty-section">
              <i className="fas fa-info-circle"></i>
              <p>No tasks available. Click "Add Dummy Task" to create one.</p>
            </div>
          ) : (
            <table>
              <thead>
                <tr>
                  <th>#</th>
                  <th>Task Name</th>
                  <th>Duration (s)</th>
                  <th>Status</th>
                </tr>
              </thead>
              <tbody>
                {tasks.map((task, i) => (
                  <tr key={i}>
                    <td>{i + 1}</td>
                    <td>{task.name}</td>
                    <td>{task.duration}</td>
                    <td>{getStatusBadge(task.status)}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>

      {/* Node Section */}
      <div className="card" id="nodeSection">
        <div className="card-header">
          <h2><i className="fas fa-network-wired"></i> Available Nodes</h2>
        </div>
        <div className="card-body">
          {nodes.length === 0 ? (
            <div className="empty-section">
              <i className="fas fa-info-circle"></i>
              <p>No nodes available. Click "Add Node" to create one.</p>
            </div>
          ) : (
            <table>
              <thead>
                <tr>
                  <th>#</th>
                  <th>Node ID</th>
                  <th>Tasks Assigned</th>
                  <th>Status</th>
                </tr>
              </thead>
              <tbody>
                {nodes.map((node, i) => (
                  <tr key={i}>
                    <td>{i + 1}</td>
                    <td>{node.id}</td>
                    <td>{node.task_count}</td>
                    <td><span className="badge badge-running"><i className="fas fa-circle"></i> Active</span></td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>
    </div>
  );
}

export default App;