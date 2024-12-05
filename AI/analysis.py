from visualization import generate_visualizations, generate_prediction_accuracy_graph
from heatMap import generate_heatmaps
from dashboard import create_dashboard
import pandas as pd
import os

LOG_FILE = "chess_game_log.csv"

def analyze_and_visualize():
    """Analyze game logs and generate visualizations and dashboards."""
    if not os.path.isfile(LOG_FILE):
        print("No log file found for analysis.")
        return

    try:
        generate_heatmaps(LOG_FILE)
    except Exception as e:
        print(f"Error generating heatmaps: {e}")
        return

    try:
        df = pd.read_csv(LOG_FILE)
    except pd.errors.EmptyDataError:
        print("Log file is empty. No data to analyze.")
        return
    except pd.errors.ParserError as e:
        print(f"Error parsing log file: {e}")
        return

    required_columns = {'Move Validity', 'Outcome', 'AI Move'}
    if not required_columns.issubset(df.columns):
        print(f"Missing required columns in log file: {required_columns - set(df.columns)}")
        return

    try:
        win_loss_html, move_frequency_html = generate_visualizations(df)
        accuracy_html = generate_prediction_accuracy_graph(df)
    except Exception as e:
        print(f"Error generating visualizations: {e}")
        return

    try:
        create_dashboard(win_loss_html, move_frequency_html, accuracy_html)
        print("Dashboard created: dashboard.html")
    except Exception as e:
        print(f"Error creating dashboard: {e}")

