import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load telemetry data from a CSV file
telemetry_data = pd.read_csv("data/rocket_telemetry.csv")

# Display the first few entries of the dataframe
print(telemetry_data.head())

# Set 'time_stamp' as the index for the data
telemetry_data.set_index('time_stamp', inplace=True)
print(telemetry_data.head())

# Focus on altitude and velocity columns
use_cols = ['altitude', 'velocity']
data_selected = telemetry_data[use_cols]
print(data_selected.head())

# Calculate acceleration by taking the derivative of velocity
data_selected['acceleration'] = np.gradient(data_selected['velocity'], edge_order=2)
print(data_selected.head())

# Plot altitude and acceleration over time
plt.figure(figsize=(14,7))

# Plot for Altitude
plt.subplot(1, 2, 1)
plt.plot(data_selected['altitude'], label='Altitude')
plt.title('Altitude over Time')
plt.xlabel('Time (seconds)')
plt.ylabel('Altitude (meters)')
plt.legend()

# Plot for Acceleration
plt.subplot(1, 2, 2)
plt.plot(data_selected['acceleration'], label='Acceleration', color='red')
plt.title('Acceleration over Time')
plt.xlabel('Time (seconds)')
plt.ylabel('Acceleration (m/s²)')
plt.legend()

plt.tight_layout()
plt.show()

# Save the analyzed data to a new CSV file
data_selected.to_csv("data/analyzed_telemetry.csv")
