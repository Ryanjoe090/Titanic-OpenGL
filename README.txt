It was pretty horrendous to use this with visual studio. For my main code. Look at object_loader.ccp which should really be renamed because it doesnt do the object loading, it handles the entry point of the program and initialisation/display of the program. It is where I define my transforamations. I dont use a matrix stack like I usually do in my other stuff but in this case since I only had one object it didn't really matter. 

Where I do my ACTUAL object loading is in object_ldr.cpp. There is a tutorial on https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ which only shows how to load an obj file that only has position vertices and the indices to disply the faces; which is not what you get like from an obj file if you where to export it from blender for example. you also get normals and texture mapping. So I extended it and it works huehuehue. If you decide for some reason to use my code for obj loading, just remember to triangulate faces so that the indicies will be correct. If your texture is messed up then you have done your UV mapping wrong, not me. 

I switch shaders for the smoke particles. If you run the executable in the debug file then if it works what you should see is a titanic in a little perlin noise generated environment. You are able to control the titanic and sink it :). You can also switch to a World War 1 dazzle camoflauge (obviously the actual Titanic never survived that long but its sister ship the Olympic had the fortune of sporting this type of camo). 

This code is based off tutorials off my wonderful lecturer Iain Martin to which I heavily modified the code, a lot of credit goes to him helping me understand and debug stuff. I have a video of the early stage of this at https://youtu.be/ZL2E4MvuYu0 .

I also tried implementing normal mapping (you can see i calculate tangents and bitangents) but i dropped it out due to time constraints with the assignment.

