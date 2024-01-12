# formate of log.txt
# [[0,0,0],[-0.694592,-0.0860794,-1],]
# [[0,0,0],[-0.64287,-0.0760223,-1],]
# [[0,0,0],[-0.640942,-0.0951104,-1],]
# [[0,0,0],[-0.588041,-0.0746137,-1],]
# [[0,0,0],[-0.39067,-0.0642415,-0.694631],[-0.248768,-0.0409073,-1.43179],[-0.732908,-0.120519,-2.30315],]
# [[0,0,0],[-0.31189,-0.0603342,-0.613885],[-0.28949,-0.0560011,-1.40381],[-0.671636,-0.129926,-2.32495],]

import numpy as np
import pyvista as pv
pv.global_theme.color_cycler = 'default'
meshs=[]
points=[]
lines=[]
max_path_step = 2
for i in range(max_path_step):
    points.append([])
    lines.append([])
def path_to_mesh(path):
    for step in range(max_path_step):
        offset = len(points[step])
        points[step].extend([path[step],path[step+1]])
        lines[step].append([2,offset+step,offset+step+1])
with open("log.txt","r") as f:
    for line in f:
        line = line.strip()
        path = eval(line)
        if(len(path)<max_path_step+1):
            continue
        path_to_mesh(path)
print("load points lines finish!!")
for step in range(max_path_step):
    step_points = np.array(points[step])
    step_lines = np.hstack(lines[step])
    meshs.append(pv.PolyData(step_points, lines=step_lines))
print("build mesh finish")
pl = pv.Plotter()
for mesh in meshs:
    pl.add_mesh(mesh)
pl.disable_anti_aliasing()
print("begin show!!")
pl.show()
        

        
