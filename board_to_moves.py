def getstacks(board):
    counts = [0, 0, 0]
    # Convert data structure to stacks -- one stack per column
    stacks = [[] for _ in board[0]]
    for row, values in enumerate(reversed(board)):
        for col, (value, stack) in enumerate(zip(values, stacks)):
            if value:
                # Verify there are no holes
                if len(stack) != row:
                    raise ValueError(f"The disc at {row+1},{col+1} should not be floating above an empty cell")
                stack.append(value)
                counts[value] += 1
    if not (0 <= counts[1] - counts[2] <= 1):
        raise ValueError("Number of discs per player is inconsistent")
    return stacks, 1 + counts[1] - counts[2] 


def searchmoves(stacks, player):
    # Perform a depth first search with backtracking
    for col, stack in enumerate(stacks):
        if stack and stack[-1] == player:
            stack.pop()
            moves = searchmoves(stacks, 3 - player)
            stack.append(player)  # Restore
            if moves is not None:  # Success
                moves.append(col + 1)
                return moves
    if any(stacks):
        return None  # Stuck: backtrack.
    return []  # Success: all discs were removed


def board_to_moves(board):
    stacks, nextplayer = getstacks(board)
    moves = searchmoves(stacks, 3 - nextplayer)
    if moves is None:
        return None
    return ''.join(str(move) for move in moves)