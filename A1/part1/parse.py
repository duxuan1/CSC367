import csv

# read from txt file
cache = []
f = open("cache_hierarchy.txt", "r") 
line = f.readline()
while line: 
    data = line.split()
    if data:
        cache.append(data)
    line = f.readline().strip()
f.close()

# write to csv file
f = open('cache_hierarchy.csv', 'w', encoding='utf-8', newline='')
csv_writer = csv.writer(f)
for d in cache:
    csv_writer.writerow(d)
f.close()

# read from txt file
cache_line = []
f = open("cache_line.txt", "r") 
line = f.readline()
while line: 
    data = line.split()
    if data:
        cache_line.append(data)
    line = f.readline().strip()
f.close()

# write to csv file
f = open('cache_line.csv', 'w', encoding='utf-8', newline='')
csv_writer = csv.writer(f)
for d in cache_line:
    csv_writer.writerow(d)
f.close()