import os
import plotter

class Experiment:
    def __init__(self):
        self.dataset_path = "/virtual/csc367/datasets-a3/dataset"
        self.mpi4_dataset_path = "/virtual/csc367/datasets-a3/mpi4/dataset"
        self.mpi8_dataset_path = "/virtual/csc367/datasets-a3/mpi8/dataset"

        self.mpi4 = "mpirun -np 4 --hostfile hostfile "
        self.mpi8 = "mpirun -np 8 --hostfile hostfile "

        self.parallel_methods = {1: 'seq', 2: 'omp', 3: 'mpi'}
        self.join_methods = {1: 'n', 2: 'm', 3: 'h'}
        self.partition_methods = {1: 'r', 2: 's'}

        self.time_seq_n = []
        self.time_seq_m = []
        self.time_seq_h = []
        self.time_omp_m_r = []
        self.time_omp_m_s = []
        self.time_omp_h_r = []
        self.time_omp_h_s = []
        self.time_mpi4_m_r = []
        self.time_mpi4_m_s = []
        self.time_mpi4_h_r = []
        self.time_mpi4_h_s = []
        self.time_mpi8_m_r = []
        self.time_mpi8_m_s = []
        self.time_mpi8_h_r = []
        self.time_mpi8_h_s = []

    def build_command(self, parallel_method: int, join_method: int, 
                        partition_method: int, threads: int, dataset: int):
        command = './join-'
        command += self.parallel_methods[parallel_method]
        command += ' -' + self.join_methods[join_method]
        command += ' -' + self.partition_methods[partition_method]
        command += ' -t ' + str(threads)
        command += ' ' + self.dataset_path + str(dataset)
        return command

    def build_mpi4_command(self, join_method: int, 
                        partition_method: int, dataset: int):
        command = self.mpi4
        command += ' ./join-mpi'
        command += ' -' + self.join_methods[join_method]
        command += ' -' + self.partition_methods[partition_method]
        command += ' -t 4'
        command += ' ' + self.mpi4_dataset_path + str(dataset)
        return command

    def build_mpi8_command(self, join_method: int, 
                        partition_method: int, dataset: int):
        command = self.mpi8
        command += ' ./join-mpi'
        command += ' -' + self.join_methods[join_method]
        command += ' -' + self.partition_methods[partition_method]
        command += ' -t 8'
        command += ' ' + self.mpi8_dataset_path + str(dataset)
        return command

    def get_seq_runtimes(self) -> None:
        for dataset in range(1, 6):

            command = self.build_command(1, 2, 1, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_seq_m.append(float(time))
            print(command, time)

            command = self.build_command(1, 3, 1, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_seq_h.append(float(time))
            print(command, time)

    def get_seq_runtimes(self) -> None:
        for dataset in range(1, 6):

            command = self.build_command(1, 2, 1, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_seq_m.append(float(time))
            print(command, time)

            command = self.build_command(1, 3, 1, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_seq_h.append(float(time))
            print(command, time)

    def get_omp_runtimes(self) -> None:
        for dataset in range(1, 6):

            command = self.build_command(2, 2, 1, 8, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_m_r.append(float(time))
            print(command, time)

            command = self.build_command(2, 2, 2, 8, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_m_s.append(float(time))
            print(command, time)

            command = self.build_command(2, 3, 1, 8, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_h_r.append(float(time))
            print(command, time)

            command = self.build_command(2, 3, 2, 8, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_h_s.append(float(time))
            print(command, time)

    def get_omp_threads_runtimes(self) -> None:
        for threads in range(1, 9):

            command = self.build_command(2, 2, 1, threads, 5)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_m_r.append(float(time))
            print(command, time)

            command = self.build_command(2, 2, 2, threads, 5)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_m_s.append(float(time))
            print(command, time)

            command = self.build_command(2, 3, 1, threads, 5)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_h_r.append(float(time))
            print(command, time)

            command = self.build_command(2, 3, 2, threads, 5)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_omp_h_s.append(float(time))
            print(command, time)

    def get_mpi4_runtimes(self) -> None:
        for dataset in range(1, 6):
            command = self.build_mpi4_command(2, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi4_m_r.append(float(time))
            print(command, time)

            command = self.build_mpi4_command(2, 2, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi4_m_s.append(float(time))
            print(command, time)

            command = self.build_mpi4_command(2, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi4_h_r.append(float(time))
            print(command, time)

            command = self.build_mpi4_command(2, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi4_h_s.append(float(time))
            print(command, time)

    def get_mpi8_runtimes(self) -> None:
        for dataset in range(1, 6):
            command = self.build_mpi8_command(2, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi8_m_r.append(float(time))
            print(command, time)

            command = self.build_mpi8_command(2, 2, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi8_m_s.append(float(time))
            print(command, time)

            command = self.build_mpi8_command(2, 1, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi8_h_r.append(float(time))
            print(command, time)

            command = self.build_mpi8_command(2, 2, dataset)
            stream = os.popen(command)
            time = stream.read().split()[1]
            self.time_mpi8_h_s.append(float(time))
            print(command, time)

    def clean(self):
        self.time_seq_n = []
        self.time_seq_m = []
        self.time_seq_h = []
        self.time_omp_m_r = []
        self.time_omp_m_s = []
        self.time_omp_h_r = []
        self.time_omp_h_s = []
        self.time_mpi4_m_r = []
        self.time_mpi4_m_s = []
        self.time_mpi4_h_r = []
        self.time_mpi4_h_s = []
        self.time_mpi8_m_r = []
        self.time_mpi8_m_s = []
        self.time_mpi8_h_r = []
        self.time_mpi8_h_s = []
def run() -> None:
    stream = os.popen('make')

    e = Experiment()
    colors = ['red', 'purple', 'blue', 'green', 
              'orange', 'yellow', 'grey', 'black']
    
    e.get_seq_runtimes()
    plotter.graph([1, 2, 3, 4, 5],
    [e.time_seq_m, e.time_seq_h], 
    ['time_seq_m', 'time_seq_h'], 
    colors[:2], 
    "time_seq" + ".png" , 
    "time_seq", 
    "dataset", 
    "time spent")
    e.clean()

    e.get_omp_runtimes()
    plotter.graph([1, 2, 3, 4, 5],
    [e.time_omp_m_r, e.time_omp_m_s, e.time_omp_h_r, e.time_omp_h_s], 
    ['time_omp_m_r', 'time_omp_m_s', 'time_omp_h_r', 'time_omp_h_s'], 
    colors[:4], 
    "time_omp" + ".png" , 
    "time_omp", 
    "dataset", 
    "time spent")
    e.clean()

    e.get_omp_threads_runtimes()
    plotter.graph([1, 2, 3, 4, 5, 6, 7, 8],
    [e.time_omp_m_r, e.time_omp_m_s, e.time_omp_h_r, e.time_omp_h_s], 
    ['time_omp_m_r', 'time_omp_m_s', 'time_omp_h_r', 'time_omp_h_s'], 
    colors[:4], 
    "time_omp_threads" + ".png" , 
    "time_omp_threads", 
    "threads", 
    "time spent")
    e.clean()

    e.get_mpi4_runtimes()
    plotter.graph([1, 2, 3, 4, 5],
    [e.time_mpi4_m_r, e.time_mpi4_m_s, e.time_mpi4_h_r, e.time_mpi4_h_s], 
    ['time_mpi4_m_r', 'time_mpi4_m_s', 'time_mpi4_h_r', 'time_mpi4_h_s'], 
    colors[:4], 
    "time_mpi4" + ".png" , 
    "time_mpi4", 
    "dataset", 
    "time spent")
    e.clean()

    e.get_mpi8_runtimes()
    plotter.graph([1, 2, 3, 4, 5],
    [e.time_mpi8_m_r, e.time_mpi8_m_s, e.time_mpi8_h_r, e.time_mpi8_h_s], 
    ['time_mpi8_m_r', 'time_mpi8_m_s', 'time_mpi8_h_r', 'time_mpi8_h_s'], 
    colors[:4], 
    "time_mpi8" + ".png" , 
    "time_mpi8", 
    "dataset", 
    "time spent")
    e.clean()

if __name__ == "__main__":
    run()
