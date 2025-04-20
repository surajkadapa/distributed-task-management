const express = require('express');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 18080;

// In-memory DB for nodes and tasks
let nodes = [];
let tasks = [];
let nextNodeId = 1;

// Middleware
app.use(cors()); // CORS enabled for all origins
app.use(express.json()); // Parse JSON bodies

const publicPath = path.join(__dirname, 'public');
console.log(`📁 Serving static files from: ${publicPath}`);

// Enhanced static middleware to log files being served
app.use(express.static(publicPath, {
    extensions: ['html'],
    setHeaders: (res, filePath) => {
        console.log(`✅ Serving: ${filePath}`);
    }
}));

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Routes
app.get('/nodes', (req, res) => {
    res.json(nodes);
});

app.post('/add_node', (req, res) => {
    const newNode = { id: nextNodeId++, task_count: 0 };
    nodes.push(newNode);
    res.status(201).json({ message: 'Node added', node: newNode });
});

app.get('/tasks', (req, res) => {
    res.json(tasks);
});

app.get('/debug', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'), (err) => {
      if (err) {
          console.error('DEBUG ERROR:', err);
          res.status(500).send('Error loading file.');
      }
  });
});


app.post('/add_task', (req, res) => {
    const { name, duration } = req.body;
    if (!name || !duration) {
        return res.status(400).json({ error: 'Missing task name or duration' });
    }

    const newTask = {
        name,
        duration,
        status: 0, // Pending
    };
    tasks.push(newTask);

    // Optionally assign it to a node
    if (nodes.length > 0) {
        const randomNode = nodes[Math.floor(Math.random() * nodes.length)];
        randomNode.task_count += 1;
    }

    res.status(201).json({ message: 'Task added', task: newTask });
});

// Start server
app.listen(PORT, () => {
    console.log(`🚀 Server running at http://localhost:${PORT}`);
});
console.log("✅ File executed till end");
