import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def square_to_coords(square):
    file = ord(square[0]) - ord('a')  
    rank = int(square[1]) - 1        
    return (rank, file)

def generate_heatmap_data(df, column):
    heatmap = np.zeros((8, 8))
    counts = df[column].value_counts()
    for square, count in counts.items():
        rank, file = square_to_coords(square)
        heatmap[rank, file] = count
    return heatmap

def plot_heatmap(data, title, filename):
    plt.figure(figsize=(8, 8))
    sns.heatmap(data, annot=False, fmt=".0f", cmap="coolwarm", cbar=True,
                xticklabels=['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'],
                yticklabels=['1', '2', '3', '4', '5', '6', '7', '8'])
    plt.title(title)
    plt.xlabel("File")
    plt.ylabel("Rank")
    plt.gca().invert_yaxis()  
    plt.savefig(filename)
    plt.close()

def generate_heatmaps(log_file="chess_game_log.csv"):
    if not pd.io.common.file_exists(log_file):
        print(f"Log file {log_file} not found.")
        return

    df = pd.read_csv(log_file)

    if 'AI Move' not in df.columns:
        print("AI Move column is missing in the dataset.")
        return

    df['start_square'] = df['AI Move'].str[:2]
    df['end_square'] = df['AI Move'].str[2:4]

    heatmap_start = generate_heatmap_data(df, 'start_square')
    heatmap_end = generate_heatmap_data(df, 'end_square')

    plot_heatmap(heatmap_start, "Move Start Frequency", "start_frequency_heatmap.png")
    plot_heatmap(heatmap_end, "Move End Frequency", "end_frequency_heatmap.png")

    print("Heatmaps generated and saved: start_frequency_heatmap.png, end_frequency_heatmap.png")

