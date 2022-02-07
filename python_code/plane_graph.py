import numpy as np
import pandas as pd
from scipy.spatial import distance_matrix

#program that generates a distance matrix of a fully connected plane graph, which the number of nodes is choose by the user

#request the number of nodes to the user
m = int(input("Select the number of nodes: "))

#generate the m array of random nodes onto a 2d plane 
df =pd.DataFrame(np.random.uniform(-10,10,size=(m,2)))

#compute the m x m distance matrix from the random arrays of m 2-dimensional nodes
df = pd.DataFrame(distance_matrix(df.values, df.values), index=df.index, columns=df.index)

#create new directory to save the the matrix in case it doesn't exit.
import os

if not os.path.exists('plane_graphs'):
 os.mkdir('plane_graphs')

#save the matrix of distances with the current day-time.
from datetime import datetime
now = datetime.now()
df_string = "graph_" + now.strftime("%Y_%m_%d_%H:%M:%S")
df.to_csv(f"plane_graphs/{df_string}.csv", sep = ';', decimal=',',index=False)