// src/App.js
import React, { useState, useEffect } from 'react';
import './styles/styles.css';

function App() {
  // State variables for tasks, nodes, and notifications
  const [tasks, setTasks] = useState([]);
  const [nodes, setNodes] = useState([]);
  const [notifications, setNotifications] = useState([]);
  
  // Fetch both tasks and nodes on initial load
  useEffect(() => {
    fetchData();
  }, []);

  // Main data fetch function
  const fetchData = async () => {
    await fetchTasks();
    await fetchNodes();
    await updateOverview();
  };

  // Helper function to show notifications
  const showNotification = (message, type = 'success') => {
    setNotifications((prevNotifications) => [
      ...prevNotifications,
      { message, type },
    ]);
    
    setTimeout(() => {
      setNotifications((prevNotifications) => prevNotifications.filter((notif) => notif.message !== message));
    }, 5000);
  };

  // Function to update dashboard overview statistics
  const updateOverview = async () => {
    try {
      const [tasksRes, nodesRes] = await Promise.all([
        fetch("http://localhost:5000/tasks"),
        fetch("http://localhost:5000/nodes")
      ]);
      
      const tasksData = await tasksRes.json();
      const nodesData = await nodesRes.json();
      
      setTasks(tasksData);
      setNodes(nodesData);
      
      // More logic for overview update if necessary
    } catch (err) {
      showNotification(`Failed to update overview: ${err}`, 'error');
    }
  };

  const addNode = async () => {
    try {
      const res = await fetch("http://localhost:5000/add_node", {
        method: "POST",
      });
      const data = await res.json();
      showNotification("Node added successfully!");
      fetchNodes();
      updateOverview();
    } catch (err) {
      showNotification(`Failed to add node: ${err}`, 'error');
    }
  };

  const addTask = async () => {
    try {
      const res = await fetch("http://localhost:5000/add_task", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: "Task-" + Math.floor(Math.random() * 1000), duration: Math.floor(Math.random() * 20) + 5 })
      });
      const data = await res.json();
      showNotification("Task added successfully!");
      fetchTasks();
      updateOverview();
    } catch (err) {
      showNotification(`Failed to add task: ${err}`, 'error');
    }
  };

  const fetchTasks = async () => {
    try {
      const res = await fetch("http://localhost:5000/tasks");
      const data = await res.json();
      setTasks(data);
    } catch (err) {
      showNotification(`Failed to fetch tasks: ${err}`, 'error');
    }
  };

  const fetchNodes = async () => {
    try {
      const res = await fetch("http://localhost:5000/nodes");
      const data = await res.json();
      setNodes(data);
    } catch (err) {
      showNotification(`Failed to fetch nodes: ${err}`, 'error');
    }
  };

  // Status badge rendering
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
          <i className="fas fa-sync-alt"></i> Refresh All
        </button>
      </div>

      {/* Notifications */}
      <div className="notifications">
        {notifications.map((notif, index) => (
          <div key={index} className={`notification ${notif.type}`}>
            <i className={notif.type === 'success' ? 'fas fa-check-circle' : 'fas fa-exclamation-circle'}></i>
            <div>{notif.message}</div>
          </div>
        ))}
      </div>

      {/* Dashboard overview */}
      <div className="dashboard-overview" id="overview">
        {/* Overview content will be dynamically added */}
      </div>

      {/* Task Section */}
      <div className="card" id="taskSection">
        <div className="card-header">
          <h2><i className="fas fa-clipboard-list"></i> Task List</h2>
        </div>
        <div className="card-body">
          {tasks.length === 0 ? (
            <div style={{ textAlign: 'center', padding: '20px', color: '#6c757d' }}>
              <i className="fas fa-info-circle" style={{ fontSize: '24px', marginBottom: '10px' }}></i>
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
            <div style={{ textAlign: 'center', padding: '20px', color: '#6c757d' }}>
              <i className="fas fa-info-circle" style={{ fontSize: '24px', marginBottom: '10px' }}></i>
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
