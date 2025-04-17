  import React, { useState, useEffect, useRef } from 'react';
  import { BarChart, Bar, LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend } from 'recharts';
  import { 
    Server, 
    Clock, 
    CheckCircle, 
    AlertCircle, 
    Plus, 
    RefreshCw, 
    List, 
    Activity,
    Cpu,
    Layers,
    Settings
  } from 'lucide-react';

  const API_URL = "http://localhost:5000";

  const TaskManager = () => {
    const [tasks, setTasks] = useState([]);
    const [nodes, setNodes] = useState([]);
    const [loading, setLoading] = useState(true);
    const [stats, setStats] = useState({
      totalTasks: 0,
      pendingTasks: 0,
      runningTasks: 0,
      completedTasks: 0,
      nodes: 0
    });
    const [notifications, setNotifications] = useState([]);
    const [selectedView, setSelectedView] = useState('dashboard');
    const [scheduler, setScheduler] = useState({
      type: 'fifo',
      name: 'First-In-First-Out'
    });
    const [dropdownOpen, setDropdownOpen] = useState(false);
    const dropdownRef = useRef(null);

    useEffect(() => {
      fetchData();
      // Set up interval for periodic updates
      const interval = setInterval(fetchData, 5000);
      
      // Close dropdown when clicking outside
      const handleClickOutside = (event) => {
        if (dropdownRef.current && !dropdownRef.current.contains(event.target)) {
          setDropdownOpen(false);
        }
      };
      
      document.addEventListener("mousedown", handleClickOutside);
      
      return () => {
        clearInterval(interval);
        document.removeEventListener("mousedown", handleClickOutside);
      };
    }, []);

    const fetchSchedulerInfo = async () => {
      try {
        const res = await fetch(`${API_URL}/scheduler_info`);
        if (!res.ok) {
          throw new Error('Failed to fetch scheduler info');
        }
        const data = await res.json();
        setScheduler(data);
      } catch (error) {
        addNotification(`Error fetching scheduler info: ${error.message}`, 'error');
      }
    };

    const fetchData = async () => {
      setLoading(true);
      try {
        const [tasksRes, nodesRes] = await Promise.all([
          fetch(`${API_URL}/tasks`),
          fetch(`${API_URL}/nodes`)
        ]);
        
        if (!tasksRes.ok || !nodesRes.ok) {
          throw new Error('Failed to fetch data');
        }
        
        const tasksData = await tasksRes.json();
        const nodesData = await nodesRes.json();
        
        setTasks(tasksData);
        setNodes(nodesData);
        
        // Update statistics
        const pendingCount = tasksData.filter(t => t.status === 0).length;
        const runningCount = tasksData.filter(t => t.status === 1).length;
        const completedCount = tasksData.filter(t => t.status === 2).length;
        
        setStats({
          totalTasks: tasksData.length,
          pendingTasks: pendingCount,
          runningTasks: runningCount,
          completedTasks: completedCount,
          nodes: nodesData.length
        });
        
        // Fetch scheduler info
        fetchSchedulerInfo();
        
      } catch (error) {
        addNotification(`Error fetching data: ${error.message}`, 'error');
      } finally {
        setLoading(false);
      }
    };

    const addNode = async () => {
      try {
        const res = await fetch(`${API_URL}/add_node`, {
          method: 'POST'
        });
        
        if (!res.ok) {
          throw new Error('Failed to add node');
        }
        
        addNotification('Node added successfully');
        fetchData();
      } catch (error) {
        addNotification(`Error adding node: ${error.message}`, 'error');
      }
    };

    const addTask = async () => {
      try {
        const taskName = `Task-${Math.floor(Math.random() * 1000)}`;
        const duration = Math.floor(Math.random() * 20) + 5;
        
        const res = await fetch(`${API_URL}/add_task`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ name: taskName, duration })
        });
        
        if (!res.ok) {
          throw new Error('Failed to add task');
        }
        
        addNotification(`Task "${taskName}" added successfully`);
        fetchData();
      } catch (error) {
        addNotification(`Error adding task: ${error.message}`, 'error');
      }
    };
    
    const changeScheduler = async (type) => {
      try {
        const res = await fetch(`${API_URL}/set_scheduler`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ type })
        });
        
        if (!res.ok) {
          throw new Error('Failed to change scheduler');
        }
        
        const data = await res.json();
        setScheduler(data);
        addNotification(`Scheduler changed to ${data.name}`);
        setDropdownOpen(false);
      } catch (error) {
        addNotification(`Error changing scheduler: ${error.message}`, 'error');
      }
    };

    const addNotification = (message, type = 'success') => {
      const id = Date.now();
      setNotifications(prev => [...prev, { id, message, type }]);
      
      // Auto-remove notification after 5 seconds
      setTimeout(() => {
        setNotifications(prev => prev.filter(notif => notif.id !== id));
      }, 5000);
    };

    const getStatusBadge = (status) => {
      switch (status) {
        case 0:
          return (
            <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-gray-100 text-gray-800">
              <Clock className="w-3 h-3 mr-1" />
              Pending
            </span>
          );
        case 1:
          return (
            <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-blue-100 text-blue-800">
              <RefreshCw className="w-3 h-3 mr-1 animate-spin" />
              Running
            </span>
          );
        case 2:
          return (
            <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-green-100 text-green-800">
              <CheckCircle className="w-3 h-3 mr-1" />
              Completed
            </span>
          );
        default:
          return (
            <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-gray-100 text-gray-800">
              Unknown
            </span>
          );
      }
    };

    // Generate chart data
    const generateChartData = () => {
      return [
        { name: 'Pending', value: stats.pendingTasks || 0, color: '#9CA3AF' },
        { name: 'Running', value: stats.runningTasks || 0, color: '#3B82F6' },
        { name: 'Completed', value: stats.completedTasks || 0, color: '#10B981' }
      ];
    };

    // Generate mock historical data for the line chart
    const generateHistoricalData = () => {
      // In a real application, this would be fetched from an API
      return [
        { name: '1h ago', tasks: 4, nodes: 2 },
        { name: '50m ago', tasks: 6, nodes: 2 },
        { name: '40m ago', tasks: 8, nodes: 3 },
        { name: '30m ago', tasks: 10, nodes: 3 },
        { name: '20m ago', tasks: 12, nodes: 4 },
        { name: '10m ago', tasks: stats.totalTasks - 2, nodes: stats.nodes - 1 },
        { name: 'Now', tasks: stats.totalTasks, nodes: stats.nodes }
      ];
    };

    // Scheduler selector component
    const SchedulerSelector = () => (
      <div className="relative inline-block text-left" ref={dropdownRef}>
        <div>
          <button
            type="button"
            className="inline-flex justify-center rounded-md border border-gray-300 shadow-sm px-4 py-2 bg-white text-sm font-medium text-gray-700 hover:bg-gray-50 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500"
            id="scheduler-options"
            aria-haspopup="true"
            aria-expanded={dropdownOpen}
            onClick={() => setDropdownOpen(!dropdownOpen)}
          >
            <Settings className="mr-2 h-5 w-5 text-gray-500" />
            Scheduler: {scheduler.name}
            <svg className="-mr-1 ml-2 h-5 w-5" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
              <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
            </svg>
          </button>
        </div>
        {dropdownOpen && (
          <div
            className="origin-top-right absolute right-0 mt-2 w-56 rounded-md shadow-lg bg-white ring-1 ring-black ring-opacity-5 divide-y divide-gray-100 z-10"
            role="menu"
            aria-orientation="vertical"
            aria-labelledby="scheduler-options"
          >
            <div className="py-1" role="none">
              <button
                onClick={() => changeScheduler('fifo')}
                className={`${scheduler.type === 'fifo' ? 'bg-gray-100 text-gray-900' : 'text-gray-700'} block w-full text-left px-4 py-2 text-sm hover:bg-gray-100`}
                role="menuitem"
              >
                First-In-First-Out (FIFO)
              </button>
              <button
                onClick={() => changeScheduler('roundrobin')}
                className={`${scheduler.type === 'roundrobin' ? 'bg-gray-100 text-gray-900' : 'text-gray-700'} block w-full text-left px-4 py-2 text-sm hover:bg-gray-100`}
                role="menuitem"
              >
                Round Robin
              </button>
              <button
                onClick={() => changeScheduler('loadbalanced')}
                className={`${scheduler.type === 'loadbalanced' ? 'bg-gray-100 text-gray-900' : 'text-gray-700'} block w-full text-left px-4 py-2 text-sm hover:bg-gray-100`}
                role="menuitem"
              >
                Load Balanced
              </button>
            </div>
          </div>
        )}
      </div>
    );

    // Render different views
    const renderView = () => {
      switch (selectedView) {
        case 'dashboard':
          return renderDashboard();
        case 'tasks':
          return renderTasks();
        case 'nodes':
          return renderNodes();
        default:
          return renderDashboard();
      }
    };

    const renderDashboard = () => (
      <div className="space-y-6">
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-5 gap-6">
          <DashboardCard 
            title="Total Nodes" 
            value={stats.nodes} 
            icon={<Server className="h-6 w-6 text-blue-500" />} 
            color="blue"
          />
          <DashboardCard 
            title="Total Tasks" 
            value={stats.totalTasks} 
            icon={<Layers className="h-6 w-6 text-indigo-500" />} 
            color="indigo"
          />
          <DashboardCard 
            title="Pending" 
            value={stats.pendingTasks} 
            icon={<Clock className="h-6 w-6 text-gray-500" />} 
            color="gray"
          />
          <DashboardCard 
            title="Running" 
            value={stats.runningTasks} 
            icon={<RefreshCw className="h-6 w-6 text-blue-500" />} 
            color="blue"
          />
          <DashboardCard 
            title="Completed" 
            value={stats.completedTasks} 
            icon={<CheckCircle className="h-6 w-6 text-green-500" />} 
            color="green"
          />
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <div className="bg-white rounded-lg shadow p-6">
            <h3 className="text-lg font-medium text-gray-900 mb-4">Task Status Distribution</h3>
            <div className="h-64 w-full">
              {stats.totalTasks > 0 ? (
                <BarChart width={500} height={250} data={generateChartData()} margin={{ top: 20, right: 30, left: 20, bottom: 5 }}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="name" />
                  <YAxis />
                  <Tooltip />
                  <Legend />
                  <Bar dataKey="value" name="Tasks" fill="#3B82F6" />
                </BarChart>
              ) : (
                <div className="flex h-full items-center justify-center text-gray-500">
                  No task data available
                </div>
              )}
            </div>
          </div>
          
          <div className="bg-white rounded-lg shadow p-6">
            <h3 className="text-lg font-medium text-gray-900 mb-4">System Activity</h3>
            <div className="h-64 w-full">
              <LineChart width={500} height={250} data={generateHistoricalData()} margin={{ top: 20, right: 30, left: 20, bottom: 5 }}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="name" />
                <YAxis />
                <Tooltip />
                <Legend />
                <Line type="monotone" dataKey="tasks" stroke="#3B82F6" activeDot={{ r: 8 }} name="Tasks" />
                <Line type="monotone" dataKey="nodes" stroke="#10B981" name="Nodes" />
              </LineChart>
            </div>
          </div>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <div className="bg-white rounded-lg shadow overflow-hidden">
            <div className="px-6 py-4 border-b border-gray-200 flex justify-between items-center">
              <h3 className="text-lg font-medium text-gray-900">Recent Tasks</h3>
              <button 
                onClick={() => setSelectedView('tasks')}
                className="text-blue-600 hover:text-blue-800 text-sm font-medium"
              >
                View All
              </button>
            </div>
            <div className="divide-y divide-gray-200">
              {tasks && tasks.length > 0 ? (
                tasks.slice(0, 5).map((task, index) => (
                  <div key={index} className="px-6 py-4 flex justify-between items-center">
                    <div>
                      <p className="text-sm font-medium text-gray-900">{task.name}</p>
                      <p className="text-sm text-gray-500">Duration: {task.duration}s</p>
                    </div>
                    {getStatusBadge(task.status)}
                  </div>
                ))
              ) : (
                <div className="px-6 py-4 text-center text-gray-500">
                  No tasks available
                </div>
              )}
            </div>
          </div>
          
          <div className="bg-white rounded-lg shadow overflow-hidden">
            <div className="px-6 py-4 border-b border-gray-200 flex justify-between items-center">
              <h3 className="text-lg font-medium text-gray-900">Active Nodes</h3>
              <button 
                onClick={() => setSelectedView('nodes')}
                className="text-blue-600 hover:text-blue-800 text-sm font-medium"
              >
                View All
              </button>
            </div>
            <div className="divide-y divide-gray-200">
              {nodes && nodes.length > 0 ? (
                nodes.slice(0, 5).map((node, index) => (
                  <div key={index} className="px-6 py-4 flex justify-between items-center">
                    <div>
                      <p className="text-sm font-medium text-gray-900">Node-{node.id}</p>
                      <p className="text-sm text-gray-500">Tasks assigned: {node.task_count}</p>
                    </div>
                    <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-green-100 text-green-800">
                      Active
                    </span>
                  </div>
                ))
              ) : (
                <div className="px-6 py-4 text-center text-gray-500">
                  No nodes available
                </div>
              )}
            </div>
          </div>
        </div>
      </div>
    );

    const renderTasks = () => (
      <div className="bg-white shadow rounded-lg overflow-hidden">
        <div className="px-6 py-4 border-b border-gray-200">
          <h2 className="text-lg font-medium text-gray-900">All Tasks</h2>
        </div>
        <div className="overflow-x-auto">
          {tasks && tasks.length > 0 ? (
            <table className="min-w-full divide-y divide-gray-200">
              <thead className="bg-gray-50">
                <tr>
                  <th scope="col" className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Task Name
                  </th>
                  <th scope="col" className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Duration (s)
                  </th>
                  <th scope="col" className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Status
                  </th>
                </tr>
              </thead>
              <tbody className="bg-white divide-y divide-gray-200">
                {tasks.map((task, index) => (
                  <tr key={index} className={index % 2 === 0 ? 'bg-white' : 'bg-gray-50'}>
                    <td className="px-6 py-4 whitespace-nowrap text-sm font-medium text-gray-900">
                      {task.name}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                      {task.duration}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap">
                      {getStatusBadge(task.status)}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          ) : (
            <div className="text-center py-6 text-gray-500">
              No tasks available
            </div>
          )}
        </div>
      </div>
    );

    const renderNodes = () => (
      <div className="bg-white shadow rounded-lg overflow-hidden">
        <div className="px-6 py-4 border-b border-gray-200">
          <h2 className="text-lg font-medium text-gray-900">All Nodes</h2>
        </div>
        <div className="overflow-x-auto">
          {nodes && nodes.length > 0 ? (
            <table className="min-w-full divide-y divide-gray-200">
              <thead className="bg-gray-50">
                <tr>
                  <th scope="col" className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Node ID
                  </th>
                  <th scope="col" className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Tasks Assigned
                  </th>
                  <th scope="col" className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                    Status
                  </th>
                </tr>
              </thead>
              <tbody className="bg-white divide-y divide-gray-200">
                {nodes.map((node, index) => (
                  <tr key={index} className={index % 2 === 0 ? 'bg-white' : 'bg-gray-50'}>
                    <td className="px-6 py-4 whitespace-nowrap text-sm font-medium text-gray-900">
                      Node-{node.id}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                      {node.task_count}
                    </td>
                    <td className="px-6 py-4 whitespace-nowrap">
                      <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-green-100 text-green-800">
                        <span className="w-2 h-2 mr-1 bg-green-400 rounded-full"></span>
                        Active
                      </span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          ) : (
            <div className="text-center py-6 text-gray-500">
              No nodes available
            </div>
          )}
        </div>
      </div>
    );

    // Dashboard Card Component
    const DashboardCard = ({ title, value, icon, color }) => {
      const colorClasses = {
        blue: 'bg-blue-50 border-blue-200',
        indigo: 'bg-indigo-50 border-indigo-200',
        green: 'bg-green-50 border-green-200',
        red: 'bg-red-50 border-red-200',
        gray: 'bg-gray-50 border-gray-200'
      };
      
      return (
        <div className={`rounded-lg shadow border p-6 ${colorClasses[color] || colorClasses.gray}`}>
          <div className="flex items-center">
            <div className="flex-shrink-0">
              {icon}
            </div>
            <div className="ml-5 w-0 flex-1">
              <dt className="text-sm font-medium text-gray-500 truncate">
                {title}
              </dt>
              <dd className="flex items-baseline">
                <div className="text-2xl font-semibold text-gray-900">
                  {value}
                </div>
              </dd>
            </div>
          </div>
        </div>
      );
    };

    return (
      <div className="min-h-screen bg-gray-100">
        {/* Navbar */}
        <nav className="bg-white shadow-sm">
          <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
            <div className="flex justify-between h-16">
              <div className="flex">
                <div className="flex-shrink-0 flex items-center">
                  <Cpu className="h-8 w-8 text-blue-600" />
                  <span className="ml-2 text-xl font-bold text-gray-900">Task Manager</span>
                </div>
                <div className="hidden sm:ml-6 sm:flex sm:space-x-8">
                  <button
                    onClick={() => setSelectedView('dashboard')}
                    className={`${
                      selectedView === 'dashboard'
                        ? 'border-blue-500 text-gray-900'
                        : 'border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700'
                    } inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium`}
                  >
                    <Activity className="mr-2 h-5 w-5" />
                    Dashboard
                  </button>
                  <button
                    onClick={() => setSelectedView('tasks')}
                    className={`${
                      selectedView === 'tasks'
                        ? 'border-blue-500 text-gray-900'
                        : 'border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700'
                    } inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium`}
                  >
                    <List className="mr-2 h-5 w-5" />
                    Tasks
                  </button>
                  <button
                    onClick={() => setSelectedView('nodes')}
                    className={`${
                      selectedView === 'nodes'
                        ? 'border-blue-500 text-gray-900'
                        : 'border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700'
                    } inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium`}
                  >
                    <Server className="mr-2 h-5 w-5" />
                    Nodes
                  </button>
                </div>
              </div>
              <div className="flex items-center">
                <SchedulerSelector />
                <button
                  onClick={addNode}
                  className="ml-3 mr-2 inline-flex items-center px-3 py-2 border border-transparent text-sm leading-4 font-medium rounded-md text-white bg-blue-600 hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500"
                >
                  <Server className="mr-1 h-4 w-4" />
                  Add Node
                </button>
                <button
                  onClick={addTask}
                  className="mr-2 inline-flex items-center px-3 py-2 border border-transparent text-sm leading-4 font-medium rounded-md text-white bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500"
                >
                  <Plus className="mr-1 h-4 w-4" />
                  Add Task
                </button>
                <button
                  onClick={fetchData}
                  className="inline-flex items-center px-3 py-2 border border-gray-300 text-sm leading-4 font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500"
                >
                  <RefreshCw className="mr-1 h-4 w-4" />
                  Refresh
                </button>
              </div>
            </div>
          </div>
        </nav>

        {/* Mobile nav */}
        <div className="sm:hidden bg-white border-t border-gray-200 fixed bottom-0 w-full z-10">
          <div className="grid grid-cols-3 gap-1">
            <button
              onClick={() => setSelectedView('dashboard')}
              className={`${
                selectedView === 'dashboard' ? 'text-blue-600' : 'text-gray-500'
              } flex flex-col items-center justify-center py-3`}
            >
              <Activity className="h-6 w-6" />
              <span className="text-xs mt-1">Dashboard</span>
            </button>
            <button
              onClick={() => setSelectedView('tasks')}
              className={`${
                selectedView === 'tasks' ? 'text-blue-600' : 'text-gray-500'
              } flex flex-col items-center justify-center py-3`}
            >
              <List className="h-6 w-6" />
              <span className="text-xs mt-1">Tasks</span>
            </button>
            <button
              onClick={() => setSelectedView('nodes')}
              className={`${
                selectedView === 'nodes' ? 'text-blue-600' : 'text-gray-500'
              } flex flex-col items-center justify-center py-3`}
            >
              <Server className="h-6 w-6" />
              <span className="text-xs mt-1">Nodes</span>
            </button>
          </div>
        </div>

        {/* Main content */}
        <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 pb-24 sm:pb-6">
          {loading ? (
            <div className="flex justify-center items-center h-64">
              <div className="animate-spin rounded-full h-12 w-12 border-t-2 border-b-2 border-blue-500"></div>
            </div>
          ) : (
            renderView()
          )}
        </main>

        {/* Notifications */}
        <div className="fixed bottom-4 right-4 z-50 space-y-2">
          {notifications.map(notification => (
            <div
              key={notification.id}
              className={`${
                notification.type === 'error' ? 'bg-red-50 text-red-800 border-red-200' : 'bg-green-50 text-green-800 border-green-200'
              } p-4 rounded-lg shadow-lg border max-w-md flex items-start`}
            >
              {notification.type === 'error' ? (
                <AlertCircle className="h-5 w-5 mr-3 mt-0.5" />
              ) : (
                <CheckCircle className="h-5 w-5 mr-3 mt-0.5" />
              )}
              <span>{notification.message}</span>
            </div>
          ))}
        </div>
      </div>
    );
  };

  export default TaskManager;