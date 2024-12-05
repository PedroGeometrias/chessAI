import csv
import os
from datetime import datetime

LOG_FILE = "chess_game_log.csv"

def log_game_data(game_id, move_number, fen, ai_move, player_move, outcome="ongoing", is_valid=None):
    file_exists = os.path.isfile(LOG_FILE)
    with open(LOG_FILE, mode="a", newline="") as file:
        writer = csv.writer(file)
        if not file_exists:
            writer.writerow(["Game ID", "Move Number", "FEN", "AI Move", "Player Move", "Outcome", "Move Validity", "Timestamp"])
        writer.writerow([game_id, move_number, fen, ai_move, player_move, outcome, is_valid, datetime.now()])

