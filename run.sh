#!/bin/bash

echo "Setting up Python virtual environment..."
python3 -m venv venv
source venv/bin/activate

echo "Installing Python dependencies..."
pip install -r requirements.txt

echo "Building the Connect4 solver..."
mkdir -p build
cd build
cmake ..
make
cd ..

cp build/connect4 .

echo "Starting the Connect4 API server..."
python app.py

deactivate
