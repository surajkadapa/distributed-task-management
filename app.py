from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import json

app = Flask(__name__)
CORS(app)  # Allow all origins by default

CROW_URL = "http://localhost:18080"

def curl_post(endpoint, json_data):
    try:
        response = subprocess.check_output([
            "curl", "-s", "-X", "POST", f"{CROW_URL}{endpoint}",
            "-H", "Content-Type: application/json",
            "-d", json.dumps(json_data)
        ])
        return response.decode('utf-8')
    except subprocess.CalledProcessError as e:
        return str(e)

def curl_get(endpoint):
    try:
        response = subprocess.check_output([
            "curl", "-s", f"{CROW_URL}{endpoint}"
        ])
        return response.decode('utf-8')
    except subprocess.CalledProcessError as e:
        return str(e)

@app.route("/add_node", methods=["POST"])
def add_node():
    result = curl_post("/add_node", {})
    return jsonify({"message": result})

@app.route("/add_task", methods=["POST"])
def add_task():
    data = request.get_json()
    result = curl_post("/add_task", data)
    return jsonify({"message": result})

@app.route("/nodes", methods=["GET"])
def get_nodes():
    result = curl_get("/nodes")
    return app.response_class(result, mimetype='application/json')

@app.route("/tasks", methods=["GET"])
def get_tasks():
    result = curl_get("/tasks")
    return app.response_class(result, mimetype='application/json')

if __name__ == "__main__":
    print("🚀 Flask proxy running at http://localhost:5000")
    app.run(port=5000)
