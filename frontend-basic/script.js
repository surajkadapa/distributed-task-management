// Initialize the dashboard on load
document.addEventListener('DOMContentLoaded', () => {
    fetchData();
    fetchDbStats();
    fetchCurrentScheduler();
});

// Main data fetch function to get both tasks and nodes
async function fetchData() {
    fetchTasks();
    fetchNodes();
    updateOverview();
}

// Fetch database statistics
async function fetchDbStats() {
    try {
        const response = await fetch("http://localhost:5000/db_stats");
        const data = await response.json();

        if (data.error) {
            showNotification(`Failed to fetch database stats: ${data.error}`, 'error');
            return;
        }

        // Update the UI with the stats
        document.getElementById('db-total-tasks').textContent = data.total_tasks;
        document.getElementById('db-pending-tasks').textContent = data.pending_tasks;
        document.getElementById('db-running-tasks').textContent = data.running_tasks;
        document.getElementById('db-completed-tasks').textContent = data.completed_tasks;
        document.getElementById('db-total-nodes').textContent = data.total_nodes;

    } catch (err) {
        showNotification(`Failed to fetch database stats: ${err}`, 'error');
    }
}

function refreshDbStats() {
    fetchDbStats();
    showNotification("Database statistics refreshed");
}

// Fetch current scheduler
async function fetchCurrentScheduler() {
    try {
        const response = await fetch("http://localhost:5000/scheduler_info");
        const data = await response.json();

        if (data.error) {
            showNotification(`Failed to fetch scheduler info: ${data.error}`, 'error');
            return;
        }

        // Update the scheduler selector
        document.getElementById('scheduler-type').value = data.type;

    } catch (err) {
        showNotification(`Failed to fetch scheduler info: ${err}`, 'error');
    }
}

// Change the scheduler
async function changeScheduler(schedulerType) {
    try {
        const response = await fetch("http://localhost:5000/set_scheduler", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ type: schedulerType })
        });

        const data = await response.json();

        if (data.error) {
            showNotification(`Failed to change scheduler: ${data.error}`, 'error');
            return;
        }

        showNotification(`Scheduler changed to ${data.name}`);

    } catch (err) {
        showNotification(`Failed to change scheduler: ${err}`, 'error');
    }
}

// Helper function to show notifications
function showNotification(message, type = 'success') {
    const notifications = document.getElementById('notifications');
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;

    const icon = document.createElement('i');
    icon.className = type === 'success' ? 'fas fa-check-circle' : 'fas fa-exclamation-circle';

    const text = document.createElement('div');
    text.textContent = message;

    notification.appendChild(icon);
    notification.appendChild(text);
    notifications.appendChild(notification);

    // Remove after 5 seconds
    setTimeout(() => {
        notification.style.opacity = '0';
        notification.style.transform = 'translateX(100%)';
        notification.style.transition = 'all 0.3s ease';

        setTimeout(() => {
            notifications.removeChild(notification);
        }, 300);
    }, 5000);
}

// Get status badge HTML
function getStatusBadge(status) {
    switch (status) {
        case 0:
            return '<span class="badge badge-pending"><i class="fas fa-clock"></i> Pending</span>';
        case 1:
            return '<span class="badge badge-running"><i class="fas fa-spinner fa-spin"></i> Running</span>';
        case 2:
            return '<span class="badge badge-completed"><i class="fas fa-check"></i> Completed</span>';
        default:
            return '<span class="badge badge-pending"><i class="fas fa-question"></i> Unknown</span>';
    }
}

// Update dashboard overview statistics
async function updateOverview() {
    try {
        const [tasksRes, nodesRes] = await Promise.all([
            fetch("http://localhost:5000/tasks"),
            fetch("http://localhost:5000/nodes")
        ]);

        const tasks = await tasksRes.json();
        const nodes = await nodesRes.json();

        const pendingTasks = tasks.filter(t => t.status === 0).length;
        const runningTasks = tasks.filter(t => t.status === 1).length;
        const completedTasks = tasks.filter(t => t.status === 2).length;

        const overview = document.getElementById('overview');
        overview.innerHTML = `
            <div class="stat-card">
              <i class="fas fa-server"></i>
              <h3>${nodes.length}</h3>
              <p>Active Nodes</p>
            </div>
            <div class="stat-card">
              <i class="fas fa-tasks"></i>
              <h3>${tasks.length}</h3>
              <p>Total Tasks</p>
            </div>
            <div class="stat-card">
              <i class="fas fa-spinner"></i>
              <h3>${runningTasks}</h3>
              <p>Running Tasks</p>
            </div>
            <div class="stat-card">
              <i class="fas fa-check-circle"></i>
              <h3>${completedTasks}</h3>
              <p>Completed Tasks</p>
            </div>
          `;

        // Also update the database stats
        fetchDbStats();

    } catch (err) {
        showNotification(`Failed to update overview: ${err}`, 'error');
    }
}

async function addNode() {
    try {
        document.getElementById('nodeSection').querySelector('.card-body').innerHTML = `
            <div class="loading">
              <i class="fas fa-spinner"></i>
            </div>
          `;

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
}

// Remove a node
async function removeNode(nodeId) {
    if (!confirm(`Are you sure you want to remove Node ${nodeId}?`)) {
        return;
    }

    try {
        document.getElementById('nodeSection').querySelector('.card-body').innerHTML = `
            <div class="loading">
              <i class="fas fa-spinner"></i>
            </div>
          `;

        const res = await fetch("http://localhost:5000/remove_node", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ node_id: nodeId })
        });
        const data = await res.json();
        showNotification(`Node ${nodeId} removed successfully!`);
        fetchNodes();
        updateOverview();
        fetchDbStats();
    } catch (err) {
        showNotification(`Failed to remove node: ${err}`, 'error');
    }
}

// Show add task modal
function showAddTaskModal() {
    document.getElementById('addTaskModal').style.display = 'block';
    document.getElementById('task-name').focus();
}

// Close add task modal
function closeAddTaskModal() {
    document.getElementById('addTaskModal').style.display = 'none';
}

// Submit add task form
async function submitAddTask() {
    const name = document.getElementById('task-name').value;
    const duration = parseInt(document.getElementById('task-duration').value);

    if (!name) {
        showNotification("Task name is required", 'error');
        return;
    }

    if (isNaN(duration) || duration < 1) {
        showNotification("Duration must be a positive number", 'error');
        return;
    }

    try {
        document.getElementById('taskSection').querySelector('.card-body').innerHTML = `
            <div class="loading">
              <i class="fas fa-spinner"></i>
            </div>
          `;

        closeAddTaskModal();

        const res = await fetch("http://localhost:5000/add_task", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ name, duration })
        });
        const data = await res.json();
        showNotification(`Task "${name}" added successfully!`);
        fetchTasks();
        updateOverview();
    } catch (err) {
        showNotification(`Failed to add task: ${err}`, 'error');
    }
}

async function fetchTasks() {
    try {
        const taskBody = document.getElementById('taskSection').querySelector('.card-body');
        taskBody.innerHTML = `
            <div class="loading">
              <i class="fas fa-spinner"></i>
            </div>
          `;

        const res = await fetch("http://localhost:5000/tasks");
        const data = await res.json();

        if (data.length === 0) {
            taskBody.innerHTML = `
              <div style="text-align: center; padding: 20px; color: #6c757d;">
                <i class="fas fa-info-circle" style="font-size: 24px; margin-bottom: 10px;"></i>
                <p>No tasks available. Click "Add Task" to create one.</p>
              </div>
            `;
            return;
        }

        let html = `<table>
            <thead>
              <tr>
                <th>#</th>
                <th>ID</th>
                <th>Task Name</th>
                <th>Duration (s)</th>
                <th>Status</th>
              </tr>
            </thead>
            <tbody>`;

        data.forEach((task, i) => {
            html += `<tr>
                <td>${i + 1}</td>
                <td>${task.id}</td>
                <td>${task.name}</td>
                <td>${task.duration}</td>
                <td>${getStatusBadge(task.status)}</td>
              </tr>`;
        });

        html += `</tbody></table>`;
        taskBody.innerHTML = html;
    } catch (err) {
        showNotification(`Failed to fetch tasks: ${err}`, 'error');
        document.getElementById('taskSection').querySelector('.card-body').innerHTML = `
            <div style="text-align: center; padding: 20px; color: #dc3545;">
              <i class="fas fa-exclamation-triangle" style="font-size: 24px; margin-bottom: 10px;"></i>
              <p>Error loading tasks. Please try again.</p>
            </div>
          `;
    }
}

async function fetchNodes() {
    try {
        const nodeBody = document.getElementById('nodeSection').querySelector('.card-body');
        nodeBody.innerHTML = `
            <div class="loading">
              <i class="fas fa-spinner"></i>
            </div>
          `;

        const res = await fetch("http://localhost:5000/nodes");
        const data = await res.json();

        if (data.length === 0) {
            nodeBody.innerHTML = `
              <div style="text-align: center; padding: 20px; color: #6c757d;">
                <i class="fas fa-info-circle" style="font-size: 24px; margin-bottom: 10px;"></i>
                <p>No nodes available. Click "Add Node" to create one.</p>
              </div>
            `;
            return;
        }

        let html = `<table>
            <thead>
              <tr>
                <th>#</th>
                <th>Node ID</th>
                <th>Tasks Assigned</th>
                <th>Status</th>
                <th>Actions</th>
              </tr>
            </thead>
            <tbody>`;

        data.forEach((node, i) => {
            html += `<tr>
                <td>${i + 1}</td>
                <td>${node.id}</td>
                <td>${node.task_count}</td>
                <td><span class="badge badge-running"><i class="fas fa-circle"></i> Active</span></td>
                <td>
                  <button class="btn btn-sm btn-danger" onclick="removeNode(${node.id})">
                    <i class="fas fa-trash"></i> Remove
                  </button>
                </td>
              </tr>`;
        });

        html += `</tbody></table>`;
        nodeBody.innerHTML = html;
    } catch (err) {
        showNotification(`Failed to fetch nodes: ${err}`, 'error');
        document.getElementById('nodeSection').querySelector('.card-body').innerHTML = `
            <div style="text-align: center; padding: 20px; color: #dc3545;">
              <i class="fas fa-exclamation-triangle" style="font-size: 24px; margin-bottom: 10px;"></i>
              <p>Error loading nodes. Please try again.</p>
            </div>
          `;
    }
}

// Close modal when clicking outside
window.onclick = function (event) {
    const modal = document.getElementById('addTaskModal');
    if (event.target == modal) {
        modal.style.display = "none";
    }
}

// Handle Enter key in the task modal
document.getElementById('task-duration').addEventListener('keyup', function (event) {
    if (event.key === 'Enter') {
        submitAddTask();
    }
});