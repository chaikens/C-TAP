Python/Wand drawing text on image began to fail.
I was able to fix by (in the conda environment for this project)

conda install wand
(which changed repository from pipy conda-forge)
conda install fontconfig
conda install --force-reinstall imagemagick wand
