import os
import plotter

BIG_IMAGE = 1
TALL_IMAGE = 2

LAPLACIAN3 = 1
LAPLACIAN5 = 2
LAPLACIAN9 = 3
IDENTITY = 4

SEQUENTIAL = 1
SHARDED_ROWS = 2
SHARDED_COLUMNS_COLUMN_MAJOR = 3
SHARDED_COLUMNS_ROW_MAJOR = 4
WORK_QUEUE = 5


class Experiment:
    def __init__(self):
        self.sequential_runtimes = []
        self.colors = []
        self.n_threads = []
        self.chunks = []
        self.rows_runtimes = []
        self.col_col_runtimes = []
        self.col_row_runtimes = []
        self.work_queue_runtimes_fixed_chunks = []   
        self.work_queue_runtimes_dynamic_chunks = []   

    def build_command(self, image_num: int, filter_num: int, method_num: int, 
                    thread_num: int, chunk_size: int):
        command = './main'
        command += ' -b ' + str(image_num)
        command += ' -f ' + str(filter_num)
        command += ' -m ' + str(method_num)
        if method_num != SEQUENTIAL:
            command += ' -n ' + str(thread_num)
        if method_num == WORK_QUEUE:
            command += ' -c ' + str(chunk_size)
        command += ' -t ' + str(1)
        return command

    def build_threads_chunks_colors(self, times: int) -> None:
        n_threads = 1
        for i in range(times):
            self.n_threads.append(n_threads)
            self.chunks.append(n_threads)
            self.colors.append('black')
            n_threads *= 2

    def get_sequential_runtimes(self, times: int, filter: int) -> None:
        command = self.build_command(BIG_IMAGE, filter, SEQUENTIAL, 0, 0)
        for i in range(times):
            stream = os.popen(command)
            output = stream.read()
            self.sequential_runtimes.append(float(output[5:]))

    def get_parallel_runtimes(self, times: int, filter: int) -> None:
        for i in range(times):
            n_threads = self.n_threads[i]

            command = self.build_command(BIG_IMAGE, filter, 
                SHARDED_COLUMNS_COLUMN_MAJOR, n_threads, 0)
            stream = os.popen(command)
            output = stream.read()
            self.col_col_runtimes.append(float(output[5:]))

            command = self.build_command(BIG_IMAGE, filter, 
                SHARDED_COLUMNS_ROW_MAJOR, n_threads, 0)
            stream = os.popen(command)
            output = stream.read()
            self.col_row_runtimes.append(float(output[5:]))

            command = self.build_command(BIG_IMAGE, filter, 
                                         SHARDED_ROWS, n_threads, 0)
            stream = os.popen(command)
            output = stream.read()
            self.rows_runtimes.append(float(output[5:]))

    def get_work_queue_runtimes_threads_eq_chunks(self, times: int, filter: int) -> None:
        for i in range(times):
            n_threads = self.n_threads[i]

            command = self.build_command(BIG_IMAGE, filter, 
                WORK_QUEUE, n_threads, n_threads)
            stream = os.popen(command)
            output = stream.read()
            self.word_queue_runtimes_fixed_chunk.append(float(output[5:]))

    def get_work_queue_runtimes_fixed_chunks(self, times: int, filter: int) -> None:
        for i in range(times):
            n_threads = self.n_threads[i]

            command = self.build_command(BIG_IMAGE, filter, 
                WORK_QUEUE, n_threads, n_threads)
            stream = os.popen(command)
            output = stream.read()
            self.work_queue_runtimes_fixed_chunks.append(float(output[5:]))

    def get_work_queue_runtimes_dynamic_chunks(self, times: int, filter: int) -> None:
        for i in range(times):
            n_threads = self.n_threads[i]
            curr_thread = []
            for j in range(times):
                chunks = self.chunks[j]
                command = self.build_command(BIG_IMAGE, filter, 
                    WORK_QUEUE, n_threads, chunks)
                stream = os.popen(command)
                output = stream.read()
                curr_thread.append(float(output[5:]))
            self.work_queue_runtimes_dynamic_chunks.append(curr_thread)

def run(filter: int) -> None:
    os.chdir('./build')
    stream = os.popen('make')

    e = Experiment()
    times = 12
    e.build_threads_chunks_colors(times)
    e.get_sequential_runtimes(times, filter)
    e.get_parallel_runtimes(times, filter)
    e.get_work_queue_runtimes_fixed_chunks(times, filter)
    e.get_work_queue_runtimes_dynamic_chunks(times, filter)

    os.chdir('../')
    colors = [
            'red', 'purple', 'blue', 'green', 'orange', 'yellow',
            'grey', 'black', 'pink', 'olive', 'violet', 'tan']

    plotter.graph(e.n_threads, 
    [e.sequential_runtimes, e.col_col_runtimes, e.col_row_runtimes, e.rows_runtimes, e.work_queue_runtimes_fixed_chunks], 
    ['sequential_runtimes', 'col_col_runtimes', 'col_row_runtimes', 'rows_runtimes', 'work_queue_runtimes_fixed_chunks'], 
    colors[:5], 
    "test" + str(filter*2-1) + ".png" , " Algorithms run on increasing threads size filter " + str(filter), "threads size", "time spent")

    names = []
    for i in range(times):
        names.append("chunk size = " + str(i+1))
    plotter.graph(e.n_threads, 
    e.work_queue_runtimes_dynamic_chunks, 
    names, 
    colors, 
    "test" + str(filter*2) + ".png", "Work Queue on increasing chunk size filter" + str(filter), "threads size", "time spent")


if __name__ == "__main__":
    for i in range(1, 5):
        run(i)
