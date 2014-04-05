horosphere provides glue for using versor within allosystem projects

for now this assumes you have allosystem and versor installed along side this repo,
for instance in $(HOME)/code/, you have done the following:

  cd $HOME/code/
  git clone git://github.com/AlloSphereResearchGroup/AlloSystem.git
  git clone git://github.com/wolftype/versor.git
  git clong git://github.com/wolftype/horosphere.git

make and test allosystem and versor independently,
then test this current repo:

  ./compile.sh demos/graphics/xSimpleApp.cpp
  ./build/bin/xSimpleApp

to build in the allosphere:
  ./allocompile.sh demos/graphics/xSimpleApp.cpp
  ./deployAndRun.sh xSimpleApp




