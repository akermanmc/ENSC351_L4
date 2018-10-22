word_countMR_OMP: word_countMR_OMP.cpp
	g++ -o word_countMR_OMP word_countMR_OMP.cpp -fopenmp

clean: 
	rm -f word_countMR_OMP word_countMR_OMP.o