all : report.tex biblio.bib
	pdflatex report
	biber report
	pdflatex report
	pdflatex report