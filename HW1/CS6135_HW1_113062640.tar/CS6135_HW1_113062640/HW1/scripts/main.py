from parser import parse_arguments, Config
from GA_helper_function import  init_population, fitness, select_parent, crossover, mutation
from functools import partial

if __name__ == "__main__":
    config = parse_arguments()
    # using '-d' to turn on debug flag
    if (config.debug):
        config.print_configuration()

    POPULATION_SIZE = config.population_size
    GENOME_LENGTH = 10
    MUTATION_RATE = config.mut_prob
    CROSSOVER_RATE = config.cross_prob
    GENERATION = config.num_generations
    NUM_TRIALS = 1 #for anytime behavior
    N = config.dimension

    fitness_pram = partial(fitness, genome_length=GENOME_LENGTH, N=N, mode=config.representation)
    
    bound = [(0.1,0.9),(100,1000)] #[core_utilization, clock_period]

    population = init_population(POPULATION_SIZE, GENOME_LENGTH, N, bound, mode=config.representation)

    for generation in range(GENERATION):
        new_population = []

        while len(new_population) < POPULATION_SIZE:
            selected = select_parent(population, config.tournament_size, fitness_pram)
            parent1, parent2 = selected[0], selected[1]
            child1, child2 = crossover(parent1, parent2, CROSSOVER_RATE, mode= config.representation, uniform = config.uniform_crossover)
            new_population.append(mutation(child1, MUTATION_RATE, mode=config.representation))
            new_population.append(mutation(child2, MUTATION_RATE, mode=config.representation))
        population = sorted(population + new_population, key = fitness_pram, reverse=True)[:POPULATION_SIZE]
            
    best_gemone = population[0] #after sorted, where the maximum at 
    best_fitness = fitness_pram(best_gemone)
    print("best setting: core_utilization/clock_period: ")
    print(best_gemone)
    print("best fitting: ")
    print(best_fitness)