from flask import Flask, request, jsonify, send_from_directory
import subprocess
import re
import time
from flask_cors import CORS
import os

app = Flask(__name__, static_folder="frontend", static_url_path="/")
CORS(app)

# Global variable to store the process
task_manager_process = None

def read_process_output(timeout=5):
    """Read all available output from the process with a timeout"""
    if task_manager_process is None:
        return []
    
    output_lines = []
    end_time = time.time() + timeout
    
    while time.time() < end_time:
        line = task_manager_process.stdout.readline()
        if not line:
            time.sleep(0.1)
            continue
            
        line = line.decode('utf-8').strip() if isinstance(line, bytes) else line.strip()
        if line:
            print(f"DEBUG: Read line: {line}")
            output_lines.append(line)
            
            if "Enter choice:" in line:
                print("DEBUG: Found 'Enter choice:' prompt")
                break
    
    return output_lines

def parse_menu_options(output_lines):
    """Extract menu options from output lines"""
    print(f"DEBUG: Raw output lines received for parsing: {output_lines}")
    
    menu_options = {}
    for line in output_lines:
        print(f"DEBUG: Checking line: '{line}'")  # Log each line before processing

        # Match lines like "1. Task Management"
        match = re.match(r"(\d+)\.\s+(.*)", line)
        if match:
            option_num = match.group(1)
            option_text = match.group(2)
            menu_options[option_num] = option_text
            print(f"DEBUG: Matched menu option -> {option_num}: {option_text}")
    
    print(f"DEBUG: Final parsed menu options: {menu_options}")
    return menu_options


@app.route("/")
def serve_frontend():
    """Serve the main HTML file"""
    return send_from_directory("frontend", "index.html")

@app.route("/start", methods=["GET"])
def start_task_manager():
    """Start the task manager process and return initial menu"""
    global task_manager_process
    
    print("DEBUG: /start endpoint hit")
    
    try:
        print("DEBUG: Starting new task_manager process")
        task_manager_process = subprocess.Popen(
            ["./task_manager"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            bufsize=0,
            universal_newlines=True
        )
        
        if task_manager_process.poll() is not None:
            return jsonify({"error": "Failed to start task_manager process"}), 500
    except Exception as e:
        return jsonify({"error": f"Error starting process: {str(e)}"}), 500
    
    time.sleep(1)
    
    output_lines = read_process_output()
    
    if not output_lines:
        return jsonify({"error": "No output received from task_manager"}), 500
    
    if not any("Enter choice:" in line for line in output_lines):
        output_lines.append("Enter choice:")
        print("DEBUG: Manually added 'Enter choice:' prompt")
    
    menu_options = parse_menu_options(output_lines)
    
    print(f"DEBUG: Returning output: {output_lines}")
    print(f"DEBUG: Returning menu options: {menu_options}")
    
    return jsonify({
        "output": output_lines,
        "menu_options": menu_options
    })

@app.route("/send_choice", methods=["POST"])
def send_choice():
    """Send a choice to the task manager and get the next menu"""
    global task_manager_process
    
    if task_manager_process is None or task_manager_process.poll() is not None:
        return jsonify({"error": "Task manager process is not running"}), 400
    
    data = request.json
    choice = data.get("choice")
    
    if not choice:
        return jsonify({"error": "No choice provided"}), 400
    
    print(f"DEBUG: Sending choice: {choice}")
    
    try:
        choice_with_newline = f"{choice}\n"
        task_manager_process.stdin.write(choice_with_newline)
        task_manager_process.stdin.flush()
    except Exception as e:
        return jsonify({"error": f"Failed to send choice: {str(e)}"}), 500
    
    time.sleep(1)
    
    output_lines = read_process_output()
    
    menu_options = parse_menu_options(output_lines)
    
    if output_lines and not any("Enter choice:" in line for line in output_lines):
        output_lines.append("Enter choice:")
        print("DEBUG: Manually added 'Enter choice:' prompt")
    
    print(f"DEBUG: Returning output: {output_lines}")
    print(f"DEBUG: Returning menu options: {menu_options}")
    
    return jsonify({
        "output": output_lines,
        "menu_options": menu_options
    })

if __name__ == "__main__":
    app.run(debug=True)
