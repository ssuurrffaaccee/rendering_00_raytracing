import sys
import pathlib

CURRENT_DIR = pathlib.Path(__file__).parent
sys.path.append(str(CURRENT_DIR))

from recipes import raytracing_recipes
def do_one(name):
    print("cd %s\n"%(name))
    print("echo ")
    print("echo enter %s\n"%(name))
    print("source ./build_and_run.sh\n") 
    print("cd ..\n")
    print("\n")
for recipe in raytracing_recipes:
    do_one(recipe)
print("python3 -B ./script/gen_readme.py > README.md")
