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
	rm -v akelarre $(objects)

doc/report.pdf : doc/report.tex doc/biblio.bib
	cd doc/ && pdflatex report
	cd doc/ && biber report
	cd doc/ && pdflatex report
	# latexmk -pdf -pdflatex="pdflatex -interactive=nonstopmode" -use-make -jobname=doc/report -cd doc/report

doc : doc/report.pdf

cleanup :
	cd doc && latexmk -CA
