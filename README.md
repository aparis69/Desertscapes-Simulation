## Desertscape Simulation

<img src="https://aparis69.github.io/public_html/imgs/deserts_representative.jpg"
     alt="Deserts - representative image"
     style="float: left; margin: 5px;" />

Source code for some results shown in the paper "Desertscapes Simulation" published in Computer Graphics Forum in 2019
and presented at Pacific Graphics 2019. This is aimed at researchers, students or profesionnals who may want to reproduce **some** of the results described in the paper.
[Click here for more information about the project](https://aparis69.github.io/public_html/projects/paris2019_Deserts.html).

### Important notes
* This code is **not** the one which produced the scenes seen in the paper. Everything has been *recoded* on my side to make sure it is free to use. The original code from the paper is dependent on internal libraries of my team.
Hence, the results as well as the timings may differ from the ones in the paper.
* This is **research** code provided without any warranty. However, if you have any problem you can still send me an email or create an issue.

### Testing
There is no dependency. Running the program will output 3 .obj files which can then be visualized in another application (Blender, MeshLab). Tests have been made on:
* Visual Studio 2017: double click on the solution in ./VS2017/ and Ctrl + F5 to run
* Visual Studio 2019: double click on the solution in ./VS2019/ and Ctrl + F5 to run
* Ubuntu 16.04: cd ./G++/ && make && make run

In you can't compile or run the code, the resulting obj files are available in the Objs/ folder in the repo.

### Citation
You can use this code in any way you want, however please credit the original article:
```
@article {Paris2019Desert,
    author = {Paris, Axel and Galin, Eric and Peytavie,
              Adrien and Guérin, Eric and Argudo, Oscar},
    title = {Desertscapes Simulation},
    journal = {Computer Graphics Forum},
    volume = {38},
    number = {7},
    year = {2019},
  }
```	

### Missing
There is still some things missing from the paper implementation. They might be added in the future if someone is interested. What is not in the code:
* Interactive tools showcased in the video
* Complex wind scenario and wind roses
