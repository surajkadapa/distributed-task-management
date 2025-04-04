from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS
import os
import random

app = Flask(__name__, static_folder='public', static_url_path='/')
CORS(app)  # Enable CORS

# In-memory DB for nodes and tasks
nodes = []
tasks = []
next_node_id = 1

# Serve index.html
@app.route('/')
def serve_index():
    return send_from_directory(app.static_folder, 'index.html')

# Serve any static files (like JS, CSS, etc.)
@app.route('/<path:path>')
def serve_static(path):
    return send_from_directory(app.static_folder, path)

# API: Get nodes
@app.route('/nodes', methods=['GET'])
def get_nodes():
    return jsonify(nodes)

# API: Add node
@app.route('/add_node', methods=['POST'])
def add_node():
    global next_node_id
    new_node = { "id": next_node_id, "task_count": 0 }
    next_node_id += 1
    nodes.append(new_node)
    return jsonify({ "message": "Node added", "node": new_node }), 201

# API: Get tasks
@app.route('/tasks', methods=['GET'])
def get_tasks():
    return jsonify(tasks)

# API: Add task
@app.route('/add_task', methods=['POST'])
def add_task():
    data = request.get_json()
    name = data.get('name')
    duration = data.get('duration')

    if not name or not duration:
        return jsonify({ "error": "Missing task name or duration" }), 400

    new_task = {
        "name": name,
        "duration": duration,
        "status": 0
    }
    tasks.append(new_task)

    if nodes:
        random_node = random.choice(nodes)
        random_node["task_count"] += 1

    return jsonify({ "message": "Task added", "task": new_task }), 201

if __name__ == '__main__':
    print(f"🌐 Serving on http://localhost:18080")
    app.run(host='0.0.0.0', port=18081, debug=True)
