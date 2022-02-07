import pandas as pd
import random
import numpy as np
#read the matrix of distances
M = pd.read_csv( 'plane_graphs/graph_2022_02_06_17:37:59.csv' ,
                 sep = ';',
                 decimal=',')

#read the temperature schedule
v=pd.read_csv( 'schedule/schedule_2022_02_06_17:29:51.csv' ,
                 decimal='.',header = None)


#cost function (i.e sum of the distances of a path)
def cost_function(path):
    total=0
    for i in range(1,len(path)):
        total +=M[str(path[i-1])][path[i]]
    return total +M[str(path[0])][path[len(path)-1]]



#function that given a path and its cost value it returns a new path, with new cost value and the difference of the cost values
#the new path is created by switching two random nodes
def new_random_path(path,cost):
    new_path=path.copy()
    
    #pick two random node locations
    x =random.sample(range(len(path)), 2)
    #switch the two random nodes
    new_path[x] = new_path[[x[1],x[0]]]
    
    #pick the nodes that were switched
    
    x1,x0=path[x]
    
    #select the neighboring nodes of the previously selected two nodes
    if x[0] ==0:
        x0_m,x0_p=path[[len(path)-1,1]]
    
    elif x[0] == len(path)-1:
        x0_m,x0_p=path[[x[0]-1,0]]

    else:    
        x0_m,x0_p=path[[x[0]-1,x[0]+1]]
    
    if x[1] == len(path)-1:
        x1_m,x1_p=path[[x[1]-1,0]]

    elif x[1] ==0:
        x1_m,x1_p=path[[len(path)-1,1]]

    else:    
        x1_m,x1_p=path[[x[1]-1,x[1]+1]]

    #compute the distances of the selected nodes with their neighbouring nodes
    dist_original=M[str(x0)][x0_m]+M[str(x0)][x0_p]+M[str(x1)][x1_m]+M[str(x1)][x1_p]
    new_dist=M[str(x1)][x0_m]+M[str(x1)][x0_p]+M[str(x0)][x1_m]+M[str(x0)][x1_p]
    
    diff_cost=new_dist-dist_original
    new_cost=cost +diff_cost
    
    return new_path, cost_function(new_path), cost_function(new_path)-cost



#function that applies the annealing process 
from scipy.stats import bernoulli

def anneling(temp,path, cost,best_path,best_cost,n=1000):
    
    #repeat the annealing process n times
    for i in range(n):
        print(i)
        print(cost)    
        new_path,new_cost,diff_cost=new_random_path(path,cost)
        
        #if the new path is better than the best path, replace the current path and the best path
        if new_cost <best_cost:
            best_cost=new_cost
            best_path=new_path  
            cost=new_cost
            path=new_path 
    
        #replace the path, if the new path as a lower cost function
        elif diff_cost <0:
            cost=new_cost
            path=new_path
    
        #replace the path, with probability of exp(-diff_cost/temp), by the simulated annealing processure
        else:
            if bernoulli.rvs(size=1,p=np.exp(-diff_cost/temp))==1:
                cost=new_cost
                path=new_path
                print('worse choosen')
    
    return  path, cost, best_path,best_cost

#generate a random path
size=len(M.index)
current_path=np.arange(0, size)
np.random.shuffle(current_path)

#compute the initial value of the cost function
current_cost=cost_function(current_path)

#set up the inital and the best paths with their respective cost functions
intial_path=current_path.copy()
initial_cost=current_cost
best_path=current_path.copy()
best_cost=current_cost

#go through the tempurature schdedule and apply the simulated annealing method
for temp in v[0]:
    current_path, current_cost, best_path, best_cost = anneling(temp,current_path,current_cost,best_path,best_cost)

print("Initial Path:",intial_path, initial_cost)
print("Current Path:",current_path,current_cost)
print("Best Path:",best_path,best_cost)
print(cost_function(best_path))