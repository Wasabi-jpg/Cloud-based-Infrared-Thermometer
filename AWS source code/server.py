from flask import Flask, request, jsonify, render_template
import time

app = Flask(__name__)

# Store received data in memory for demonstration purposes
stored_data = {"ambientTemp": None, "objectTemp": None, "timestamp": time.time()}

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/data", methods=["GET", "POST"])
def receive_data():
    global stored_data

    if request.method == "POST":
        try:
            # Parse the incoming JSON data
            data = request.get_json()
            if not data:
                return jsonify({"error": "No JSON payload received"}), 400

            # Update stored data with received temperature values
            stored_data["ambientTemp"] = data.get("ambientTemp")
            stored_data["objectTemp"] = data.get("objectTemp")
            stored_data["timestamp"] = time.time()  # Update timestamp

            print(f"Received Ambient Temperature: {stored_data['ambientTemp']} °C")
            print(f"Received Object Temperature: {stored_data['objectTemp']} °C")

            return jsonify({"message": "Data received successfully"}), 200
        except Exception as e:
            print(f"Error: {e}")
            return jsonify({"error": "Something went wrong"}), 500
    else:
        # Handle GET requests by returning the data
        return jsonify([{
            "timestamp": stored_data["timestamp"],
            "ambientTemp": stored_data["ambientTemp"],
            "objectTemp": stored_data["objectTemp"]
        }])

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
