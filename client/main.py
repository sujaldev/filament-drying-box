from time import time
from collections import deque

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from websockets.sync.client import connect

MAX_BUFFER = 10

humidity, temperature = 0.0, 0.0
time_buffer = deque([time()] * MAX_BUFFER, maxlen=MAX_BUFFER)

humidity_buffer = deque([humidity] * MAX_BUFFER, maxlen=MAX_BUFFER)
temperature_buffer = deque([temperature] * MAX_BUFFER, maxlen=MAX_BUFFER)

# Setup plot
fig, (ax1, ax2) = plt.subplots(2)
HUMIDITY_LABEL = fig.text(.15, .85, f"Humidity: {humidity}", fontsize=15)
TEMPERATURE_LABEL = fig.text(.15, .4, f"Temperature: {temperature}", fontsize=15)


def animate(_interval):
    read()
    update_plot()


def read():
    global ws, humidity, temperature
    humidity, temperature = ws.recv().split()
    humidity, temperature = float(humidity), float(temperature)
    humidity_buffer.append(humidity)
    temperature_buffer.append(temperature)
    time_buffer.append(time())


def update_plot():
    ax1.clear()
    ax2.clear()
    ax1.plot(time_buffer, humidity_buffer, label="raw", linewidth=1)
    ax2.plot(time_buffer, temperature_buffer, label="raw")
    HUMIDITY_LABEL.set_text(f"Humidity: {humidity}")
    TEMPERATURE_LABEL.set_text(f"Temperature: {temperature}")


if __name__ == "__main__":
    ws = connect("ws://10.2.0.137/ws")
    # noinspection PyTypeChecker
    ani = animation.FuncAnimation(fig, animate, interval=10)
    plt.get_current_fig_manager().resize(1920, 1080)
    plt.show()
    ws.close()
