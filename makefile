objects = keygen.o akelarre.o main.o
latex_garbage = report.aux report.out report.log report.xml \
	report.toc report.blg report.bcf report.bbl

all : $(objects)
	gcc $(objects) -o akelarre

keygen.o : keygen.c keygen.h
	gcc -c keygen.c

akelarre.o : akelarre.c akelarre.h
	gcc -c akelarre.c

main.o : main.c
	gcc -c main.c

clean :
	rm $(objects) $(latex_garbage)

doc : report.tex biblio.bib
	pdflatex report
	biber report
	pdflatex report
	pdflatex report
