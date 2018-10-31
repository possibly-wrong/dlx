# Generate crossword exact cover files for input to test_dlx.

from itertools import product
import sys

def print_crossword_cover(words, double=1, file=sys.stdout):
    """Print crossword exact cover problem for input to test_dlx.

    This reduction to exact cover assumes len(letters) > 2.
    If double=1 ("double word square"), then each word may be used at
    most once; solutions will be generated as A, Tr(A) pairs.
    If double=0, then words may be used more than once; solutions
    may or may not be symmetric.
    """
    letters = 'abcdefghijklmnopqrstuvwxyz'
    m = len(words)
    n = len(words[0])
    print(m * n * 2 * (n * 26 + double), file=file)
    b = [True, False]
    for row, ((w, word), k, across) in enumerate(
                product(enumerate(words), range(n), b)):
        for col, (i, j, letter, horiz) in enumerate(
                product(range(n), range(n), letters, b)):
            if ((i if across else j) == k and
                (word[j if across else i] == letter) == (across == horiz)):
                print(row, col, file=file)
        if double == 1:
            print(row, -(w + 1), file=file)
    if double == 0:
        m = 0
    print(m, file=file)
    for w in range(m):
        print(-(w + 1), file=file)

# Words in the dictionary words.txt must all be of the same length.
with open('words.txt') as f:
    words = [word.strip().lower() for word in f]
with open('crossword.txt', 'w') as f:
    print_crossword_cover(words, file=f)
