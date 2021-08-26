# Create the tutorial.html files in each example folder

# for every folder below examples
for i in ../../../examples/[0123]*; do
#for i in ../../../examples/01.*; do	# if you want to run/update a single example use this loop
  echo NOW doing $i
  
  if ! [ -f "$i/main.cpp" ]; then 
    continue
  fi
  
  # uses the main.cpp files to create some file to use as doxygen input
  sed -f tutorials.sed $i/main.cpp >single_tut.txt
#  echo tutorials.sed has run
  
  # create the html file 
  doxygen doxygen_tutorial.cfg
#  echo doxygen has run
  
  # Fix the image links
  sed -f maketut.sed html/example???.html >tutorial.html
#  echo maketut.sed has run  
  
  #move to example folder 
  mv tutorial.html $i/tutorial.html
#  echo copied
  
  #cleanup
  rm -r html
#  echo cleaned
done

#cleanup
rm single_tut.txt
