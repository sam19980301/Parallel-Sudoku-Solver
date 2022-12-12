from pathlib import Path
import csv

def get_dataset(file_path):
    dataset = str(file_path).split(sep = "_dataset_") 
    dataset = dataset[-1]
    dataset = dataset[:-4]
    return dataset

if __name__ == "__main__":
    data_dir = Path("./result")

    algo_dict = {"crooks": "Crooks", "crosshatching": "Cross Hatching"}

    dataset_list = []
    threads_list = []
    branchs_list = []
    algo_list = []
    average_elapsed_time_list = []
    average_guess_list = []
    average_max_depth_list = []

    for file_path in sorted(Path(data_dir).glob("*.txt")):
        file_path_split = str(file_path).split(sep = "_")
        dataset = get_dataset(file_path)
        threads = int(file_path_split[1])
        branchs = int(file_path_split[3])
        algo = file_path_split[5]

        with open(file_path, 'r') as file:
            info = file.read().splitlines()
            info = info[-1]
        info = info.split()
        
        average_elapsed_time = info[6].split(sep = "=")
        average_elapsed_time = float(average_elapsed_time[-1])
        
        average_guess = info[9].split(sep = "=")
        average_guess = float(average_guess[-1])
        
        average_max_depth = info[-1].split(sep = "=")
        average_max_depth = float(average_max_depth[-1])

        dataset_list.append(dataset)
        threads_list.append(threads)
        branchs_list.append(branchs)
        algo_list.append(algo_dict[algo])
        average_elapsed_time_list.append(average_elapsed_time)
        average_guess_list.append(average_guess)
        average_max_depth_list.append(average_max_depth)

        continue
        print("dataset: {}".format(dataset))
        print("threads: {}".format(threads))
        print("branchs: {}".format(branchs))
        print("algo: {}".format(algo_dict[algo]))
        print("average_elapsed_time: {}".format(average_elapsed_time))
        print("average_guess: {}".format(average_guess))
        print("average_max_depth: {}".format(average_max_depth))
        print("==========================\n")
    
    with open("data.csv", 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        writer.writerow(["Dataset", "Threads", "Branchs", "Algorithm", "Average Elapsed Time", "Average Guess", "Average Max Depth"])
        
        zip_data = zip(dataset_list, threads_list, branchs_list, algo_list, average_elapsed_time_list, average_guess_list, average_max_depth_list)
        zip_data = sorted(zip_data, key = lambda s: (s[0], s[3], s[1], s[2]))
        for item in zip_data:
            writer.writerow(item)
        