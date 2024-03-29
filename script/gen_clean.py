import sys
import pathlib

CURRENT_DIR = pathlib.Path(__file__).parent
sys.path.append(str(CURRENT_DIR))

from recipes import raytracing_recipes
def do_one(name):
    print("cd %s"%name)
    print("rm -r ./out")
    print("cd ..")
for recipe in raytracing_recipes:
    do_one(recipe)