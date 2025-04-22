FROM python:3.12

WORKDIR /app

# Install basic dependencies, including build tools and C++ compiler
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Configure environment
ENV PYTHONUNBUFFERED=1

# Copy Python dependencies and install them
COPY requirements.txt .
RUN pip install --upgrade pip
RUN pip install -r requirements.txt

# Copy source code
COPY . .

# Build the Connect4 solver
RUN mkdir -p build && cd build && cmake .. && make && cd .. && cp build/connect4 .

# Expose the port (provided by Render or other platforms)
EXPOSE $PORT

# Start the Python application
CMD ["python", "app.py"]