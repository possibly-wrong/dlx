# Generate exact cover files for input to test_dlx.

import numpy as np
from itertools import product
import sys

x = ((1, 0, 0), (0, 0, -1), (0, 1, 0))
y = ((0, 0, 1), (0, 1, 0), (-1, 0, 0))
z = ((0, -1, 0), (1, 0, 0), (0, 0, 1))

def matrix_powers(r, n):
    """Return {r^1, r^2, ..., r^n}."""
    result = []
    a = np.array(r)
    for k in range(n):
        result.append(a)
        a = a.dot(r)
    return result

def board_cover(board, pieces, rotations, optional=False):
    """Return (pairs, optional_columns) for board exact cover problem."""

    # Enumerate all possible placements/rotations of pieces.
    rows = set()
    for name, piece in pieces.items():
        for r in rotations:
            for offset in board:
                occupied = {tuple(np.array(r).dot(x) + offset) for x in piece}
                if occupied <= board:
                    rows.add((name, tuple(sorted(occupied))))

    # Convert placements to (row,col) pairs and optional column indices.
    cols = dict(enumerate(pieces))
    cols.update(enumerate(board, len(pieces)))
    cols = {v: k for k, v in cols.items()}
    pairs = []
    for i, row in enumerate(rows):
        name, occupied = row
        pairs.append((i, cols[name]))
        for x in occupied:
            pairs.append((i, cols[x]))
    return (pairs,
            list(range(len(pieces), len(cols))) if optional else [])

def print_cover(cover, file=sys.stdout):
    """Print exact cover problem for input to test_dlx."""
    pairs, opts = cover
    print(len(pairs), file=file)
    for i, j in pairs:
        print(i, j, file=file)
    print(len(opts), file=file)
    for j in opts:
        print(j, file=file)

# Example: Kanoodle
board = {(x, y, 0) for x in range(11) for y in range(5)}
pieces = {
    'Green': ((0, 0, 0), (0, 1, 0), (1, 1, 0), (1, 2, 0), (1, 3, 0)),
    'Cyan': ((0, 0, 0), (0, 1, 0), (0, 2, 0), (1, 2, 0), (2, 2, 0)),
    'Purple': ((0, 0, 0), (0, 1, 0), (0, 2, 0), (0, 3, 0)),
    'Yellow': ((0, 0, 0), (0, 1, 0), (1, 1, 0), (2, 0, 0), (2, 1, 0)),
    'Orange': ((0, 0, 0), (1, 0, 0), (1, 1, 0), (1, 2, 0)),
    'Blue': ((0, 1, 0), (1, 1, 0), (2, 1, 0), (3, 0, 0), (3, 1, 0)),
    'Pink': ((0, 0, 0), (0, 1, 0), (0, 2, 0), (0, 3, 0), (1, 2, 0)),
    'Gray': ((0, 1, 0), (1, 0, 0), (1, 1, 0), (1, 2, 0), (2, 1, 0)),
    'Magenta': ((0, 0, 0), (0, 1, 0), (1, 1, 0), (1, 2, 0), (2, 2, 0)),
    'Red': ((0, 0, 0), (0, 1, 0), (1, 0, 0), (1, 1, 0), (1, 2, 0)),
    'White': ((0, 0, 0), (1, 0, 0), (1, 1, 0)),
    'LightGreen': ((0, 0, 0), (0, 1, 0), (1, 0, 0), (1, 1, 0))}
rotations = {tuple(tuple(row) for row in np.array(a).dot(b).dot(c).tolist())
             for a, b, c in product(*(matrix_powers(r, 4) for r in [x, y, z]))}
with open('kanoodle.txt', 'w') as f:
    print_cover(board_cover(board, pieces, rotations), f)

# Example: Battleship
board = {(x, y, 0) for x in range(10) for y in range(10)}
pieces = {'Carrier': {(x, 0, 0) for x in range(5)},
          'Battleship': {(x, 0, 0) for x in range(4)},
          'Submarine': {(x, 0, 0) for x in range(3)},
          'Cruiser': {(x, 0, 0) for x in range(3)},
          'Destroyer': {(x, 0, 0) for x in range(2)}}
rotations = matrix_powers(z, 4)
with open('battleship.txt', 'w') as f:
    print_cover(board_cover(board, pieces, rotations, True), f)

# Example: Hex Battleship
def hex_battleship(is_upper):
    if is_upper:
        islands = {(-5, 4, 1), (-1, 3, -2), (-1, 7, -6), (4, 2, -6),
                   (5, -1, -4)}
    else:
        islands = {(-5, 1, 4), (-1, -6, 7), (-1, -2, 3), (2, -1, -1),
                   (4, -6, 2)}
    board = {(x, y, -x - y)
             for x in range(-7, 8)
             for y in range(max(-7 - x, -7) + 1 - abs(np.sign(x)),
                            min(7 - x, 7) + 1)
             if (6 * y >= -3 * x + x % 4) == is_upper and
             not (x, y, -x - y) in islands}
    pieces = {'Carrier': {(x, 0, -x) for x in range(3)} |
                         {(x, -1, -x + 1) for x in range(1, 3)},
              'Battleship': {(x, 0, -x) for x in range(4)},
              'Submarine': {(x, 0, -x) for x in range(3)},
              'Destroyer': {(x, 0, -x) for x in range(2)},
              'Weapons': {(0, 0, 0), (1, 0, -1), (0, 1, -1)}}
    rotations = matrix_powers(((0, -1, 0), (0, 0, -1), (-1, 0, 0)), 6)
    return board_cover(board, pieces, rotations, True)

with open('hex_battleship_upper.txt', 'w') as f:
    print_cover(hex_battleship(True), f)
with open('hex_battleship_lower.txt', 'w') as f:
    print_cover(hex_battleship(False), f)
