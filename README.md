![Infinite Cupcake Generator preview (The keygen looks Pinkie Pie from MLP:FiM dancing on top of some serial numbers)](https://raw.githubusercontent.com/liath/infinitecupcakegenerator/master/preview.gif)

#### Introduction
Need a swanky keygen template? I didn't! But I made it anyways so feel free to use it.

Ignore the current crypto codes, they are crippled versions from some research elsewhere that are here as examples.

#### Compiling
Docker is easiest but this requires Windows mode:
```powershell
docker build -t build_icg:latest -m 2GB .
docker run --rm -v ${PWD}:C:\build build_icg
```
You could also pollute your environment and run the steps in the Dockerfile and build.ps1 yourself but I'm not a fan of that personally. :)

#### References
* [CodeProject: How to Use a Font Without Installing it by Shao Voon Wong](http://www.codeproject.com/Articles/42041/How-to-Use-a-Font-Without-Installing-it)
* [StackOverflow: Transparency in GDI DCs answer by Remy Lebeau](http://stackoverflow.com/questions/28846219/transparency-in-gdi-dcs)
* [DaFont: Dimitri Swank by Fontalicious](http://www.dafont.com/dimitri.font)
* [Deathpwny for the GIF](https://www.deviantart.com/deathpwny/art/Pinkie-Pie-dancing-258855912)
