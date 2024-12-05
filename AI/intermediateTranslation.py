import socket
from datetime import datetime
import chess
import numpy as np
import tflite_runtime.interpreter as tflite
from logger import log_game_data
from analysis import analyze_and_visualize
from dashboard import create_dashboard

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8080

piece_map = {
    'P': 0, 'N': 1, 'B': 2, 'R': 3, 'Q': 4, 'K': 5,
    'p': 6, 'n': 7, 'b': 8, 'r': 9, 'q': 10, 'k': 11
}

def apply_temperature_scaling(logits, temperature=1.0):
    """Adjust logits for temperature scaling to diversify predictions."""
    logits = logits.flatten()
    exp_logits = np.exp(logits / temperature)
    probabilities = exp_logits / np.sum(exp_logits)
    return probabilities

def load_model():
    """Load the TFLite model and prepare the interpreter."""
    model_path = "chess_model.tflite"
    interpreter = tflite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    return interpreter, input_details, output_details

def process_fen(fen):
    """Standardize the FEN string for processing."""
    fen_parts = fen.split(' ')
    rows = fen_parts[0].split('/')
    processed_rows = []

    for row in rows:
        processed_row = ""
        empty_count = 0

        for char in row:
            if char == '.':
                empty_count += 1
            elif char.isdigit():
                empty_count += int(char)
            else:
                if empty_count > 0:
                    processed_row += str(empty_count)
                    empty_count = 0
                processed_row += char

        if empty_count > 0:
            processed_row += str(empty_count)

        processed_rows.append(processed_row)

    fen_parts[0] = "/".join(processed_rows)
    return " ".join(fen_parts)

def fen_to_tensor(fen):
    """Convert FEN to tensor for AI model input."""
    tensor = np.zeros((12, 8, 8), dtype=np.float32)
    rows = fen.split()[0].split('/')
    for row_idx, row in enumerate(rows):
        col_idx = 0
        for char in row:
            if char.isdigit():
                col_idx += int(char)
            else:
                tensor[piece_map[char], row_idx, col_idx] = 1.0
                col_idx += 1
    return np.expand_dims(tensor, axis=0)

def index_to_uci(index):
    """Convert AI move index to UCI string."""
    promotion_offset = index // 4096
    index %= 4096
    start_square = index // 64
    end_square = index % 64
    start_square_name = chess.SQUARE_NAMES[start_square]
    end_square_name = chess.SQUARE_NAMES[end_square]
    promotion_piece = {1: 'q', 2: 'r', 3: 'b', 4: 'n'}.get(promotion_offset, '')
    return start_square_name + end_square_name + promotion_piece

def main():
    interpreter, input_details, output_details = load_model()
    game_id = f"game_{datetime.now().strftime('%Y%m%d%H%M%S')}"
    move_number = 0

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        try:
            client_socket.connect((SERVER_HOST, SERVER_PORT))
            print("Connected to the server")

            while True:
                fen = client_socket.recv(1024).decode()
                if not fen:
                    print("Disconnected from server")
                    break

                print(f"Received FEN: {fen}")
                valid_fen = process_fen(fen)
                print(f"Processed FEN: {valid_fen}")

                try:
                    board = chess.Board(valid_fen)
                except ValueError as e:
                    print(f"Error initializing board with FEN: {e}")
                    client_socket.sendall("INVALID_FEN".encode())
                    continue

                if board.is_game_over():
                    result = board.result()
                    outcome = "draw" if result == "1/2-1/2" else "win" if result == "1-0" else "loss"
                    is_valid = uci_move in [move.uci() for move in board.legal_moves]
                    log_game_data(game_id, move_number, valid_fen, uci_move, "N/A", is_valid=is_valid)
                    print("Game over! Result:", outcome)
                    client_socket.sendall("GAME_OVER".encode())
                    break

                invalid_moves = set()
                retry_count = 0
                max_retries = 4000

                while retry_count < max_retries:
                    tensor = fen_to_tensor(valid_fen)
                    interpreter.set_tensor(input_details[0]['index'], tensor)
                    interpreter.invoke()

                    prediction = interpreter.get_tensor(output_details[0]['index'])
                    probabilities = apply_temperature_scaling(prediction, temperature=1.5)
                    move_index = np.random.choice(len(probabilities), p=probabilities)
                    uci_move = index_to_uci(move_index)

                    print(f"AI predicted move: {uci_move}")

                    try:
                        move = chess.Move.from_uci(uci_move)
                        if move in board.legal_moves:
                            board.push(move)
                            updated_fen = board.fen()
                            move_number += 1
                            is_valid = uci_move in [move.uci() for move in board.legal_moves]
                            log_game_data(game_id, move_number, valid_fen, uci_move, "N/A", is_valid=is_valid)
                            client_socket.sendall(updated_fen.encode())
                            print(f"Sent updated FEN: {updated_fen}")
                            break
                        else:
                            print(f"Illegal move: {uci_move}. Adding to invalid moves.")
                            invalid_moves.add(uci_move)
                            retry_count += 1
                    except chess.InvalidMoveError:
                        print(f"Illegal move format: {uci_move}. Skipping.")
                        invalid_moves.add(uci_move)
                        retry_count += 1

                if retry_count == max_retries:
                    print("AI failed to produce a valid move. Choosing randomly.")
                    legal_moves = list(board.legal_moves)
                    if legal_moves:
                        fallback_move = np.random.choice(legal_moves)
                        board.push(fallback_move)
                        updated_fen = board.fen()
                        move_number += 1
                        is_valid = uci_move in [move.uci() for move in board.legal_moves]
                        log_game_data(game_id, move_number, valid_fen, uci_move, "N/A", is_valid=is_valid)
                        client_socket.sendall(updated_fen.encode())
                        print(f"Sent fallback FEN: {updated_fen}")
                    else:
                        print("No legal moves available. Sending GAME_OVER.")
                        client_socket.sendall("GAME_OVER".encode())
                        break

        except (socket.error, ConnectionResetError) as e:
            print(f"Connection error: {e}")


if __name__ == "__main__":
    try:
        main()  
    except Exception as e:
        print(f"Unexpected error: {e}")
    finally:
        analyze_and_visualize()  
        print("Analysis and dashboard generation completed.")
