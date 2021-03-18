# run on single_tut.txt which is created in maketutorial.sh (or .bat)

# replace image links as we don't copy the images into example folders but keep them below media folder
# most images should be in media
s/img src="/img src="..\/..\/media\//g
# ???shots.jpg files are in media/example_screenshots another subfolder (earlier replacement already moved them to ../../media/)
s/img src="..\/..\/media\/\([0-9]\{3\}shot\)/img src="..\/..\/media\/example_screenshots\/\1/
