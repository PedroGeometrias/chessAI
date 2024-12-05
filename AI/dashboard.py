def create_dashboard(win_loss_html, move_frequency_html, accuracy_html):
    """Creates an HTML dashboard combining all visualizations and heatmaps."""
    with open("dashboard.html", "w") as dashboard:
        dashboard.write("<html><head><title>Chess Dashboard</title></head><body>")
        dashboard.write("<h1>Chess Game Analysis Dashboard</h1>")
        dashboard.write("<h2>Game Outcomes</h2>")
        dashboard.write(win_loss_html)
        dashboard.write("<h2>Move Frequencies</h2>")
        dashboard.write(move_frequency_html)
        dashboard.write("<h2>AI Prediction Accuracy</h2>")
        dashboard.write(accuracy_html)
        dashboard.write("<h2>Heatmaps</h2>")
        dashboard.write('<img src="start_frequency_heatmap.png" alt="Start Frequency Heatmap" style="width:45%;">')
        dashboard.write('<img src="end_frequency_heatmap.png" alt="End Frequency Heatmap" style="width:45%;">')
        dashboard.write("</body></html>")

