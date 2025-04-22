from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from typing import List
import subprocess
from fastapi.middleware.cors import CORSMiddleware
import board_to_moves

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class GameState(BaseModel):
    board: List[List[int]]
    current_player: int
    valid_moves: List[int]

class AIResponse(BaseModel):
    move: int

SOLVER_PATH = "./connect4"

def convert_board_to_sequence(board, current_player):
    return board_to_moves.board_to_moves(board)

@app.get("/api/test")
async def test_endpoint():
    return {"message": "Test endpoint is working"}

@app.post("/api/connect4-move")
async def make_move(game_state: GameState) -> AIResponse:
    try:
        if not game_state.valid_moves:
            raise ValueError("No valid moves available")
        
        print("Board state:")
        for row in game_state.board:
            print(row)
        print(f"Current player: {game_state.current_player}")
        
        is_empty_board = all(all(cell == 0 for cell in row) for row in game_state.board)
        if is_empty_board:
            print("Empty board detected - using center column (3) as best move")
            return AIResponse(move=3)  # Center column is usually best first move
        
        try:
            sequence = convert_board_to_sequence(game_state.board, game_state.current_player)
            print(f"Converted sequence: {sequence}")

            result_seq = subprocess.run(
                [SOLVER_PATH], 
                input=sequence,
                text=True,
                capture_output=True
            )
            
            print(f"Solver output: {result_seq.stdout}")
            
            best_move = -1
            
            if result_seq and result_seq.stdout:
                output_lines = result_seq.stdout.strip().split('\n')
                for line in output_lines:
                    if "Best move" in line:
                        try:
                            best_move = int(line.split(':')[1].strip())
                            print(f"Found best move in output: {best_move}")
                            break
                        except (IndexError, ValueError) as e:
                            print(f"Error parsing best move: {e}")
                
                if best_move == -1:
                    parts = output_lines[-1].strip().split()
                    for part in reversed(parts):
                        if part.isdigit() and int(part) < len(game_state.board[0]):
                            best_move = int(part)
                            print(f"Found move from last line: {best_move}")
                            break
            
            print(f"Selected move from solver: {best_move}")
            
            if best_move != -1 and best_move in game_state.valid_moves:
                return AIResponse(move=best_move)
            
            if game_state.valid_moves:
                center_moves = [m for m in game_state.valid_moves if m == 3]
                if center_moves:
                    best_move = center_moves[0]
                else:
                    best_move = game_state.valid_moves[0]
            
            print(f"Fallback move: {best_move}")
            return AIResponse(move=best_move)
            
        except Exception as e:
            print(f"Error running solver: {e}")
            if game_state.valid_moves:
                return AIResponse(move=game_state.valid_moves[0])
            raise HTTPException(status_code=500, detail=str(e))
                
    except Exception as e:
        if game_state.valid_moves:
            return AIResponse(move=game_state.valid_moves[0])
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/")
async def root():
    return {"message": "Connect4 AI API is running"}

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8080)
