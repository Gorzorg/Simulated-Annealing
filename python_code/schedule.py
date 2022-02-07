#program that creates a list of a temperature schedule for the simulated annealing program
import numpy as np

#request the user the scalar multiple for the rate of cooling in the temperature schedule
beta = float(input("Select the rate of cooling: "))

#request the user the size of the schedule
m = int(input("Select the size of the schedule: "))

#create the temperature schedule
epsilon= beta / np.log(np.arange(2, m+2))

#create new directory to save the temperature schedule in case it doesn't exit.
import os
if not os.path.exists('schedule'):
 os.mkdir('schedule')

#save the tempurature schedule with the current day-time.
from datetime import datetime
now = datetime.now()
df_string = "schedule_" + now.strftime("%Y_%m_%d_%H:%M:%S")

np.savetxt(f"schedule/{df_string}.csv", epsilon, delimiter=";")
