from flask import Flask, request, jsonify
from flask_cors import CORS
import requests
import json

app = Flask(__name__)
CORS(app)  # Allow all origins by default

CROW_URL = "http://localhost:18080"

def request_post(endpoint, json_data):
    try:
        print(f"Sending POST request to {CROW_URL}{endpoint} with data: {json_data}")
        response = requests.post(
            f"{CROW_URL}{endpoint}", 
            json=json_data, 
            timeout=5  # 5 second timeout
        )
        response.raise_for_status()  # Raise exception for 4XX/5XX responses
        return response.text
    except requests.exceptions.Timeout:
        print(f"Request to {endpoint} timed out")
        return json.dumps({"error": "Request timed out", "type": json_data.get("type", "unknown")})
    except requests.exceptions.ConnectionError:
        print(f"Connection error to {endpoint}")
        return json.dumps({"error": "Connection refused", "type": json_data.get("type", "unknown")})
    except requests.exceptions.RequestException as e:
        print(f"Request error to {endpoint}: {e}")
        return json.dumps({"error": str(e), "type": json_data.get("type", "unknown")})

def request_get(endpoint):
    try:
        response = requests.get(
            f"{CROW_URL}{endpoint}", 
            timeout=5  # 5 second timeout
        )
        response.raise_for_status()  # Raise exception for 4XX/5XX responses
        return response.text
    except requests.exceptions.Timeout:
        print(f"Request to {endpoint} timed out")
        return json.dumps({"error": "Request timed out"})
    except requests.exceptions.ConnectionError:
        print(f"Connection error to {endpoint}")
        return json.dumps({"error": "Connection refused"})
    except requests.exceptions.RequestException as e:
        print(f"Request error to {endpoint}: {e}")
        return json.dumps({"error": str(e)})

@app.route("/add_node", methods=["POST"])
def add_node():
    result = request_post("/add_node", {})
    try:
        return jsonify(json.loads(result))
    except:
        return jsonify({"message": result})

@app.route("/add_task", methods=["POST"])
def add_task():
    data = request.get_json()
    result = request_post("/add_task", data)
    try:
        return jsonify(json.loads(result))
    except:
        return jsonify({"message": result})

@app.route("/nodes", methods=["GET"])
def get_nodes():
    result = request_get("/nodes")
    try:
        return app.response_class(result, mimetype='application/json')
    except:
        return jsonify([])  # Return empty array if error

@app.route("/tasks", methods=["GET"])
def get_tasks():
    result = request_get("/tasks")
    try:
        return app.response_class(result, mimetype='application/json')
    except:
        return jsonify([])  # Return empty array if error

@app.route("/set_scheduler", methods=["POST"])
def set_scheduler():
    data = request.get_json()
    print(f"Flask: setting scheduler to {data}")
    
    result = request_post("/set_scheduler", data)
    print(f"Flask: got response: {result}")
    
    try:
        # Try to parse as JSON
        json_result = json.loads(result)
        return jsonify(json_result)
    except json.JSONDecodeError:
        # If not valid JSON, return as message
        scheduler_type = data.get("type", "fifo")
        scheduler_name = "First-In-First-Out" if scheduler_type == "fifo" else \
                         "Round Robin" if scheduler_type == "roundrobin" else \
                         "Load Balanced" if scheduler_type == "loadbalanced" else "Unknown"
        
        return jsonify({
            "type": scheduler_type,
            "name": scheduler_name,
            "message": result
        })

@app.route("/scheduler_info", methods=["GET"])
def get_scheduler_info():
    result = request_get("/scheduler_info")
    try:
        return app.response_class(result, mimetype='application/json')
    except:
        return jsonify({"type": "fifo", "name": "First-In-First-Out"})

# Add a route for database statistics
@app.route("/db_stats", methods=["GET"])
def get_db_stats():
    result = request_get("/db_stats")
    try:
        return app.response_class(result, mimetype='application/json')
    except:
        return jsonify({
            "total_tasks": 0,
            "pending_tasks": 0,
            "running_tasks": 0,
            "completed_tasks": 0,
            "total_nodes": 0
        })

# Add a health check endpoint
@app.route("/health", methods=["GET"])
def health_check():
    try:
        # Try to connect to the C++ backend
        response = requests.get(f"{CROW_URL}/tasks", timeout=2)
        if response.ok:
            return jsonify({"status": "ok", "backend": "connected"})
        else:
            return jsonify({
                "status": "warning", 
                "backend": "error", 
                "message": f"Backend returned: {response.status_code}"
            })
    except requests.exceptions.RequestException as e:
        return jsonify({
            "status": "error", 
            "backend": "disconnected", 
            "message": str(e)
        })

if __name__ == "__main__":
    print("🚀 Flask proxy running at http://localhost:5000")
    app.run(port=5000, debug=True)