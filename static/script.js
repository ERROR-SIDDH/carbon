document.addEventListener("DOMContentLoaded", function () {
    fetch("/get_data")
        .then(response => response.json())
        .then(data => {
            if (data.air_quality) {
                document.getElementById("aqi").textContent = data.air_quality.aqi;
                
                const pollutants = data.air_quality.iaqi;
                document.getElementById("pm25").textContent = pollutants.pm25 ? pollutants.pm25.v : "-";
                document.getElementById("pm10").textContent = pollutants.pm10 ? pollutants.pm10.v : "-";
                document.getElementById("o3").textContent = pollutants.o3 ? pollutants.o3.v : "-";
                document.getElementById("no2").textContent = pollutants.no2 ? pollutants.no2.v : "-";
                document.getElementById("co").textContent = pollutants.co ? pollutants.co.v : "-";
                document.getElementById("so2").textContent = pollutants.so2 ? pollutants.so2.v : "-";
            } else {
                document.getElementById("aqi").textContent = "Data not available";
            }

            document.getElementById("low-emissions").textContent = data.emission_counts.LOW;
            document.getElementById("moderate-emissions").textContent = data.emission_counts.MODERATE;
            document.getElementById("heavy-emissions").textContent = data.emission_counts.HEAVY;
        })
        .catch(error => console.error("Error fetching data:", error));
});

// Fetch logs from server
function fetchLogs() {
    fetch('/get_logs')
        .then(response => response.json())
        .then(logs => {
            const logList = document.getElementById("log-list");
            logList.innerHTML = "";  // Clear old logs
            logs.forEach(log => {
                const li = document.createElement("li");
                li.innerHTML = `<strong>${log.type}</strong> - ${new Date(log.timestamp).toLocaleString()}`;
                logList.appendChild(li);
            });

            // Toggle button visibility
            document.getElementById("get-log-btn").classList.add("hidden"); // Hide "Get Logs"
            document.getElementById("refresh-log-btn").classList.remove("hidden"); // Show "Refresh"
            document.getElementById("hide-log-btn").classList.remove("hidden"); // Show "Hide"
        })
        .catch(error => console.error("Error fetching logs:", error));
}

// Clear logs & reset buttons
function clearLogs() {
    document.getElementById("log-list").innerHTML = "";  // Clear logs
    document.getElementById("get-log-btn").classList.remove("hidden"); // Show "Get Logs"
    document.getElementById("refresh-log-btn").classList.add("hidden"); // Hide "Refresh"
    document.getElementById("hide-log-btn").classList.add("hidden"); // Hide "Hide Logs"
}
