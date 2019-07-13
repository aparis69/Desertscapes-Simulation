## Desertscapes Simulation
Source code for some results shown in the paper "Desertscapes Simulation" published in Computer Graphics Forum in 2019. 
This is aimed at researchers, students or profesionnals who may want to reproduce **some** of the results described in the paper.
[Click here for more information about the project](link).

### Important notes
* This code is **not** the one which produced the scenes seen in the paper. Everything has been *recoded* on my side to make sure it is free to use. Hence, the results as well as the timings may differ from the ones in the paper.
* This is **research** code provided without any warranty. However, if you have any problem you can still send me an email or create an issue.

### Testing
There is no dependency on this code. It justs output obj files which can then be visualized in another application (Blender, MeshLab). Tests have been made on:
* Visual Studio 2017: double click on the solution in VS2017 folder + Ctrl/F5 to run.
* Ubuntu 16.0: make && make run in the G++ folder.

### Citation
You can use this code in any way you want, however please credit the original article:
```
@article {Paris2019Desert,
    author = {Paris, Axel and Galin, Eric and Peytavie,
              Adrien and Guérin, Eric and Argudo, Oscar},
    title = {Desertscapes Simulation},
    journal = {Computer Graphics Forum},
    volume = {??},
    number = {?},
    year = {2019},
  }
```	

### Missing
There is still some things missing from the paper implementation. They might be added in the future if someone is interested. What is not in the code:
* Interactive tools showcased in the video
