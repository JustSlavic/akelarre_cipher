objects = keygen.o akelarre.o main.o
latex_garbage = doc/report.aux doc/report.out \
	doc/report.log doc/report.run.xml doc/report.toc \
	doc/report.blg doc/report.bcf doc/report.bbl

all : $(objects)
	gcc $(objects) -o akelarre

keygen.o : keygen.c keygen.h
	gcc -c keygen.c

akelarre.o : akelarre.c akelarre.h
	gcc -c akelarre.c

main.o : main.c
	gcc -c main.c

clean :
	rm -v akelarre $(objects) $(latex_garbage)

doc : doc/report.tex doc/biblio.bib
	cd doc && pdflatex report
	cd doc && biber report
	cd doc && pdflatex report
	cd doc && pdflatex report
