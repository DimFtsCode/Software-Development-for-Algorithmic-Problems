import matplotlib.pyplot as plt
import numpy as np


def plot_silhouettes_together(clusters, methodname, original_silhouettes, reduced_silhouettes, average_original, average_reduced):
    fig, ax = plt.subplots(figsize=(12, 6))
    bar_width = 0.35
    # Position of bars on x-axis
    r1 = np.arange(len(original_silhouettes))
    r2 = [x + bar_width for x in r1]
    
    # Create the bars 
    ax.bar(r1, original_silhouettes, color='b', width=bar_width, edgecolor='grey', label='Original Dataset')
    ax.bar(r2, reduced_silhouettes, color='r', width=bar_width, edgecolor='grey', label='Reduced Dataset')
    
    # add the average lines 
    ax.axhline(y=average_original, color='b', linestyle='--', label=f'Average (original): {average_original:.4f}')
    ax.axhline(y=average_reduced, color='r', linestyle='--', label=f'Average (reduced): {average_reduced:.4f}')
    
    # general layout
    plt.xlabel('Cluster Number', fontweight='bold', fontsize=12)
    plt.ylabel('Silhouette Score', fontweight='bold', fontsize=12)
    plt.xticks([r + bar_width / 2 for r in range(len(original_silhouettes))], clusters)
    plt.title(f'Comparison of Silhouette Values for {methodname} Method\n (Original vs Reduced Dataset)', fontweight='bold', fontsize=14)
    plt.legend()
    
    # show the plot
    plt.show()
    
    
def plot_silhouettes(clusters, methodname, datasetname, silhouettes, average, color='b'):
    plt.figure(figsize=(10, 6))
    bars =  plt.bar(clusters, silhouettes, color=color)
    # add the average line
    plt.axhline(y=average, color=color, linestyle='--', label=f'Average: {average:.5f}')
    
    plt.xlabel('Cluster', fontweight='bold', fontsize=12)
    plt.xticks(clusters)
    
    plt.ylabel('Silhouette Score', fontweight='bold', fontsize=12)
    plt.title(f'Silhouette Values for {methodname} Method\n ({datasetname} Dataset)', fontweight='bold', fontsize=14)
    
    # show the scores on the plot
    for bar in bars:
        plt.text(bar.get_x() + bar.get_width() / 2 - 0.1, bar.get_height(), f'{bar.get_height():.5f}', 
                va='bottom', rotation=70)

    plt.legend()
    plt.tight_layout()
    plt.show()
    

def plot_timings(methods, times_original, times_reduced):
    bar_width = 0.35
    index = np.arange(len(methods))
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    original_bars = ax.bar(index, times_original, bar_width, label='Original Dataset')
    reduced_bars = ax.bar(index + bar_width, times_reduced, bar_width, label='Reduced Dataset')
    
    ax.set_xlabel('Method', fontweight='bold', fontsize=12)
    ax.set_ylabel('Time (seconds)', fontweight='bold', fontsize=12)
    ax.set_title('Comparison of Time Taken by Each Clustering Method', fontweight='bold', fontsize=14)
    ax.set_xticks(index + bar_width / 2)
    ax.set_xticklabels(methods)
    ax.legend()
    
    # put the values on the bars
    for bar in original_bars + reduced_bars:
        height = bar.get_height()
        ax.annotate(f'{height:.2f}',
                xy=(bar.get_x() + bar.get_width() / 2, height),
                xytext=(0, 3),  # 3 points vertical offset
                textcoords="offset points",
                ha='center', va='bottom')
    
    plt.tight_layout()
    plt.show()
    
def plot_AFs(queries, AFs, averages):
    fig, ax = plt.subplots(figsize=(12, 6))
    lsh, cube, gnns, mrgn = AFs
    lsh_average, cube_average, gnns_average, mrgn_average = averages
    
    bar_width = 0.2
    # Position of bars on x-axis
    br1 = np.arange(queries)
    br2 = [x + bar_width for x in br1]
    br3 = [x + bar_width for x in br2]
    br4 = [x + bar_width for x in br3]
    
    # Create the bars
    ax.bar(br1, lsh, color ='r', width = bar_width, label ='LSH')
    ax.bar(br2, cube, color ='g', width = bar_width, label ='Cube')
    ax.bar(br3, gnns, color ='b', width = bar_width, label ='GNNS')
    ax.bar(br4, mrgn, color ='y', width = bar_width, label ='MRGN')
    
    # add the average lines 
    ax.axhline(y=lsh_average, color='r', linestyle='--', label=f'LSH average: {lsh_average:.4f}')
    ax.axhline(y=cube_average, color='g', linestyle='--', label=f'Cube average: {cube_average:.4f}')
    ax.axhline(y=gnns_average, color='b', linestyle='--', label=f'GNNS average: {gnns_average:.4f}')
    ax.axhline(y=mrgn_average, color='y', linestyle='--', label=f'MRGN average: {mrgn_average:.4f}')
    
    # general layout
    plt.xlabel('Query', fontweight ='bold', fontsize = 12)
    plt.ylabel('Approximation Factor', fontweight ='bold', fontsize = 12)
    plt.xticks([r + bar_width for r in range(queries)], [f'Q{i+1}' for i in range(queries)])
    plt.title('Approximation Factor for each query', fontweight ='bold', fontsize = 16)
    plt.legend()
    
    # show the plot
    plt.show()
