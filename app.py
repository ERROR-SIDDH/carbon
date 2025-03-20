from flask import Flask, render_template, request, jsonify
from pymongo import MongoClient
import requests

app = Flask(__name__)

# MongoDB setup
client = MongoClient('mongodb+srv://sidharthmannikoth:rvr03xtyS5h1HwS5@cluster0.y2pyp.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0')
db = client["carbon_emissions"]
logs_collection = db["logs"]
emission_counts_collection = db["emission_counts"]

# Initialize emission counts if not present
def initialize_counts():
    if emission_counts_collection.count_documents({}) == 0:
        emission_counts_collection.insert_one({"LOW": 0, "MODERATE": 0, "HEAVY": 0})

initialize_counts()

# WAQI API Setup
WAQI_API_TOKEN = 'b988d55c352bb208874ff9ff8b82975d52210c84'  # Get a free token from https://aqicn.org/data-platform/token/
WAQI_API_URL = f'https://api.waqi.info/feed/chennai/?token={WAQI_API_TOKEN}'

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/get_data', methods=['GET'])
def get_data():
    # Fetch air quality data
    response = requests.get(WAQI_API_URL)
    api_data = response.json()

    if api_data['status'] == 'ok':
        air_quality = api_data['data']
    else:
        air_quality = None

    # Fetch emission counts from MongoDB
    counts = emission_counts_collection.find_one({}, {"_id": 0})

    return jsonify({"air_quality": air_quality, "emission_counts": counts})

@app.route('/log_emission', methods=['POST'])
def log_emission():
    data = request.get_json()
    emission_type = data.get('type')
    timestamp = data.get('timestamp')

    if emission_type not in ['LOW', 'MODERATE', 'HEAVY']:
        return jsonify({'error': 'Invalid emission type'}), 400

    logs_collection.insert_one({'type': emission_type, 'timestamp': timestamp})
    emission_counts_collection.update_one({}, {"$inc": {emission_type: 1}})

    return jsonify({'message': 'Emission logged successfully'}), 201

@app.route('/get_logs', methods=['GET'])
def get_logs():
    logs = list(logs_collection.find({}, {"_id": 0}).sort([("_id", -1)]))  # Sort by `_id`
    return jsonify(logs)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port= 5000 ,debug=True)
