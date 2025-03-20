import requests
import random
import time
import datetime


# Server endpoint
SERVER_URL = "http://127.0.0.1:5000/log_emission"

# Function to send a request to the server
def send_request(emission_type):
    timestamp = datetime.datetime.now().isoformat()
    response = requests.post(SERVER_URL, json={"type": emission_type , "timestamp" : timestamp})
    if response.status_code == 201:
        print(f"✅ Logged {emission_type} emission successfully!")
    else:
        print(f"❌ Error logging {emission_type}: {response.json()}")

# Function to log specific emissions
def log_specific_emissions(low, moderate, heavy):
    print(f"\nLogging {low} LOW, {moderate} MODERATE, and {heavy} HEAVY emissions...\n")
    for _ in range(low):
        send_request("LOW")
        time.sleep(0.2)  # Short delay to simulate real requests
    for _ in range(moderate):
        send_request("MODERATE")
        time.sleep(0.2)
    for _ in range(heavy):
        send_request("HEAVY")
        time.sleep(0.2)

# Function to log random emissions
def log_random_emissions(n):
    print(f"\nLogging {n} random emissions...\n")
    emission_types = ["LOW", "MODERATE", "HEAVY"]
    for _ in range(n):
        emission = random.choice(emission_types)
        send_request(emission)
        time.sleep(0.2)

# Main function with continuous testing
def main():
    while True:
        print("\nCarbon Emission Logger Test Script")
        print("1. Log specific number of emissions")
        print("2. Log random emissions")
        print("3. Exit")
        choice = input("Enter choice (1, 2, or 3): ")

        if choice == "1":
            low = int(input("Enter number of LOW emissions: "))
            moderate = int(input("Enter number of MODERATE emissions: "))
            heavy = int(input("Enter number of HEAVY emissions: "))
            log_specific_emissions(low, moderate, heavy)

        elif choice == "2":
            n = int(input("Enter number of random emissions to log: "))
            log_random_emissions(n)

        elif choice == "3":
            print("Exiting test script...")
            break

        else:
            print("Invalid choice. Please enter 1, 2, or 3.")

if __name__ == "__main__":
    main()
