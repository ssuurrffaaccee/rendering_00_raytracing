import numpy as np
import pyvista as pv
import random
pv.global_theme.color_cycler = 'default'
max_path_step = 3
sample_num = 100

all_paths = []
with open("log.txt","r") as f:
    for line in f:
        line = line.strip()
        path = eval(line)
        if(len(path)<max_path_step+1):
            continue
        all_paths.append(path)
paths = random.choices(all_paths,k=sample_num)

def path_to_mesh(path):
    lines = []
    indexs = range(len(path))
    for start,end in zip(indexs[:-1],indexs[1:]):
        lines.append([2,start,end])
    points = np.array(path)
    lines = np.hstack(lines)
    return pv.PolyData(points, lines=lines)
print("load points lines finish!!")
meshs=[]
for path in paths:
    meshs.append(path_to_mesh(path))
print("build mesh finish")
pl = pv.Plotter()
for mesh in meshs:
    pl.add_mesh(mesh)
pl.disable_anti_aliasing()
print("begin show!!")
pl.show()