

all: compile install
	@echo "compile,clean,install,uninstall"
	

compile:
	@echo "Attempting to compile the documentation"
	mkdir -p ./docs/
	doxygen doxy.libkernel
	doxygen doxy.libunix
	doxygen doxy.spoon
	cat ./misc/alpha.html > ./docs/index.html
	cat ./misc/index.html >> ./docs/index.html
	cat ./misc/omega.html >> ./docs/index.html
	cp ./misc/spoon.png ./docs/spoon.png
	cp ./misc/main.css ./docs/main.css



clean:
	rm -rf ./docs/


install:
	mkdir -p /spoon/documentation/
	cp -rf ./docs/* /spoon/documentation/
	

uninstall:
	rm -rf /spoon/documentation/*
	#rm -rf /var/www/localhost/htdocs/spoon/documentation/*



