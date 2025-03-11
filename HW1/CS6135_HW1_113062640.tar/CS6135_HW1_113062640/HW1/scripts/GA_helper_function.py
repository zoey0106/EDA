import random
import numpy as np
import os
import re

###helper functions for GA###
def random_genome(length):
    return [random.randint( 0 , 1 )for _ in range(length)]

def init_population(population_size, genome_length, N, bounds, mode='binary'):
    '''
    Initialize the population with bounded parameters
    
    Args:
        population (int)
        param_bound (list of tuples): Each tuple represent (min, max) for a parameter only for mode 'real'               ex. [(0,1),(100,1200)]
        mode (str): 'binary' , 'real'
    returns: 
        list: Initialized population
    '''
    genome_length = genome_length * N
    if mode == 'binary':
        return [random_genome(genome_length) for _ in range(population_size)]
    else:
        return [[random.uniform(min, max) for min,max in bounds] for _ in range(population_size)]

def binary_to_signed_decimal(binary_str):
    n = len(binary_str)
    if binary_str[0] == '0':
        return int(binary_str, 2)
    else:
        return int(binary_str, 2) - 2 ** n
    
# SCH fitness function
# def fitness(genome, genome_length, N, mode):
#     if mode == 'binary':
#         x_dec = [binary_to_signed_decimal("".join(map(str,genome[i*genome_length:(i+1)*genome_length]))) for i in range(N)]
#         return 418.98291 * N - sum(xi * np.sin(np.sqrt(np.abs(xi))) for xi in x_dec)
#     elif mode == 'real':
#         return 418.98291 * N - sum(xi * np.sin(np.sqrt(np.abs(xi))) for xi in genome)

def extract_value_from_report(pattern, filename):
    """
    Extract numerical value from Innovus reports.
    """
    if not os.path.exists(filename):
        print(f'file not found!filename: {filename}')
        return -1
    
    with open(filename, 'r') as file:
        for line in file:
            match = re.search(pattern, line, re.IGNORECASE)
            if match:
                return float(match.group(1))
        return float("inf") 

# Innovus fitness function 
def fitness(genome, N, mode):
    if mode != 'real':
        print('Mode setting incorrect! Should be mode real! ')
    elif mode == 'real':
        core_utilization, clock_period = genome

        file_name = f"_{core_utilization}_{clock_period}"
        sdc_file = "../sdc/sha256.sdc"
        os.system(f"sed -i '8s/.*/create_clock -name \"clk\" -period {clock_period} [get_ports clk]/' {sdc_file}")
        apr_tcl_file = "apr.tcl"
        os.system(f"sed -i '80s/.*/floorPlan -coreMarginsBy die -site asap7sc7p5t -r 1.0 {core_utilization} 6.22 6.22 6.22 6.22/' {apr_tcl_file}")
        os.system(f"sed -i '143s,.*,report_timing > ../timing{file_name}.rpt,' {apr_tcl_file}")
        os.system(f"sed -i '142s,.*,summaryReport -noHtml -outfile ../summary{file_name}.rpt,' {apr_tcl_file}")
        os.system(f"sed -i '144s,.*,verify_drc > ../drc{file_name}.rpt,' {apr_tcl_file}")

        os.system("innovus -no_gui -init apr.tcl")
        wire_length = extract_value_from_report(r"Total wire length:\s*([\d\.]+)", f"../summary{file_name}.rpt")
        chip_area = extract_value_from_report(r"Total area of chip:\s*([\d\.]+)", f"../summary{file_name}.rpt")
        slack_time = extract_value_from_report(r"Slack time:\s*([\d\.-]+)", f"../timing{file_name}.rpt")
        drc_errors = extract_value_from_report(r"Verification Complete", f"../drc{file_name}.rpt")
        if slack_time < 0 or drc_errors > 0:
            fitness = 0
        else:
            fitness = 25 + 25*(((1200-clock_period)/1100)+((200000-wire_length)/100000)+((150000-chip_area)/10000))
        return fitness

def select_parent(population, tournament_size, fitness_pram): 
    
    selected = []
    #for _ in range(len(population)):
    for _ in range(2):
        competitors = random.sample(population, tournament_size) #random.sample不會重複選->加強selection pressure
        selected.append(min(competitors, key = fitness_pram))
    return selected

def crossover(parent1, parent2, pc, mode, uniform, N = 10):
    child1, child2 = parent1.copy(), parent2.copy()
    if random.random() < pc:
        if mode == 'binary' and uniform == 1:
            for i in range(len(parent1)):
                    if random.random() < 0.5:
                        child1[i], child2[i] = child2[i], child1[i]
        if mode == 'real' and uniform == 1:
            for i in range(len(parent1)):
                    if random.random() < 0.5:
                        child1[i], child2[i] = child2[i], child1[i]
        if mode == 'binary' and uniform == 0: #two_point
            i, j = sorted(random.sample(range(len(parent1)), 2))
            child1[i:j], child2[i:j] = child2[i:j], child1[i:j]
        if mode == 'real' and uniform == 0: 
            alpha = 0.2
            child1 = [alpha * p1 + (1 - alpha) * p2 for p1, p2 in zip(parent1, parent2)]
            child2 = [alpha * p2 + (1 - alpha) * p1 for p1, p2 in zip(parent1, parent2)]
    return child1, child2

def mutation(child, pm, mode):
    if mode == 'binary':
        return [1 - bit if random.random() < pm else bit for bit in child]
    elif mode == 'real':
        return [random.uniform(-512, 511) if random.random() < pm else value for value in child]


