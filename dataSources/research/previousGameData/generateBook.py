import pandas as pd
from collections import defaultdict

# --- Configuration ---
CSV_PATH = '/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/boardstate_games.csv'
OUTPUT_HEADER = '/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/opening_book.h'
MAX_DEPTH = 6       # Number of plies (half-moves) deep to generate the book
MIN_COUNT = 2       # Minimum times a sequence must be played to be included

def reconstruct_moves(attacker, defender):
    # Interleave attacker and defender moves to form a chronological sequence
    att_moves = str(attacker).split()
    def_moves = str(defender).split()
    moves = []
    for i in range(max(len(att_moves), len(def_moves))):
        if i < len(att_moves): moves.append(att_moves[i])
        if i < len(def_moves): moves.append(def_moves[i])
    return moves

def main():
    print(f"Loading {CSV_PATH}...")
    df = pd.read_csv(CSV_PATH)
    # Handle games where defender never got to make a move
    df['defender_moves'] = df['defender_moves'].fillna('')

    # book[prefix][next_move] = count
    book = defaultdict(lambda: defaultdict(int))

    # Build frequencies
    for _, row in df.iterrows():
        moves = reconstruct_moves(row['attacker_moves'], row['defender_moves'])
        
        for i in range(min(len(moves), MAX_DEPTH)):
            prefix = tuple(moves[:i])  # Moves played so far
            next_move = moves[i]       # The response
            book[prefix][next_move] += 1

    # Extract the most popular (best) next move for each sequence
    best_moves = {}
    for prefix, next_moves in book.items():
        # Filter out rare variations (noise/blunders)
        valid_moves = {m: count for m, count in next_moves.items() if count >= MIN_COUNT}
        if not valid_moves:
            continue
            
        # Select the move with the highest frequency
        best_move = max(valid_moves.items(), key=lambda x: x[1])[0]
        best_moves[prefix] = best_move

    print(f"Generated {len(best_moves)} opening book entries.")

    # Generate the C Code Header
    print(f"Writing to {OUTPUT_HEADER}...")
    with open(OUTPUT_HEADER, 'w') as f:
        f.write("#ifndef OPENING_BOOK_H\n")
        f.write("#define OPENING_BOOK_H\n\n")
        f.write("#include <stddef.h>\n\n")
        f.write("#include \"ai.h\"\n\n")
        
        f.write(f"#define MAX_BOOK_DEPTH {MAX_DEPTH}\n\n")
        
        f.write("typedef struct {\n")
        f.write("    UWORD num_moves;\n")
        f.write("    AIMove sequence[MAX_BOOK_DEPTH];\n")
        f.write("    AIMove best_next_move;\n")
        f.write("} OpeningBookEntry;\n\n")
        
        f.write(f"const OpeningBookEntry opening_book[{len(best_moves)}] = {{\n")
        
        # Sort by the length of the prefix (depth) first, then by the moves themselves
        sorted_entries = sorted(best_moves.items(), key=lambda x: (len(x[0]), x[0]))
        
        for prefix, b_move in sorted_entries:
            b_from, b_to = b_move.split(':')
            
            # Format the sequence of moves played so far
            seq_str = "{"
            for m in prefix:
                mf, mt = m.split(':')
                seq_str += f"{{{mf}, {mt}}}, "
            
            # Pad the rest of the fixed array with {-1, -1}
            for _ in range(MAX_DEPTH - len(prefix)):
                seq_str += "{-1, -1}, "
            seq_str = seq_str.rstrip(", ") + "}"
            
            f.write(f"    {{{len(prefix)}, {seq_str}, {{{b_from}, {b_to}, 30000}}}},\n")
            
        f.write("};\n\n")
        f.write(f"const size_t OPENING_BOOK_SIZE = {len(best_moves)};\n\n")
        f.write("#endif // OPENING_BOOK_H\n")

    print("Done!")

if __name__ == '__main__':
    main()