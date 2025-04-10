# 4Connect Solver

### Build the Solver

Compile the C++ solver with:

```bash
make
```

### Run the API Server

Start the server locally:

```bash
./run.sh
```

The server will be available at http://localhost:8080.

### Get Next Move

**Example Request:**

```json
{
    "board": [
        [0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 2, 0, 0],
        [0, 0, 0, 1, 1, 2, 0]
    ],
    "current_player": 1,
    "valid_moves": [0, 1, 2, 3, 4, 5, 6]
}
```

### Expose API Publicly

```bash
ngrok http 8080
```
