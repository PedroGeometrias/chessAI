import plotly.express as px

def generate_visualizations(df):
    outcomes = df['Outcome'].value_counts()
    fig1 = px.pie(outcomes, values=outcomes.values, names=outcomes.index, title="Win/Loss/Draw Rates")
    win_loss_html = fig1.to_html(full_html=False)

    move_counts = df['AI Move'].value_counts()
    fig2 = px.bar(move_counts, x=move_counts.index, y=move_counts.values, title="Move Frequency")
    move_frequency_html = fig2.to_html(full_html=False)

    return win_loss_html, move_frequency_html

def generate_prediction_accuracy_graph(df):
    valid_counts = df['Move Validity'].value_counts()
    labels = ['Valid', 'Invalid']
    values = [valid_counts.get(True, 0), valid_counts.get(False, 0)]
    
    fig = px.bar(x=labels, y=values, labels={'x': 'Prediction Validity', 'y': 'Count'}, 
                 title="AI Move Prediction Accuracy")
    accuracy_html = fig.to_html(full_html=False)
    return accuracy_html

