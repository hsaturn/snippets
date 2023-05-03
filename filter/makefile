all: filter sample

filter: filter.cpp
	g++ -std=c++14 filter.cpp -o filter

sample: sample.cpp
	g++ -std=c++14 sample.cpp -o sample

run: filter
	./filter data.in


view: all
	./filter data.in -s | feedgnuplot --monotonic --points --domain --lines --exit


view_sample: mksample
	./filter data.gen -s | feedgnuplot --monotonic --domain --lines --exit


mksample: all
	./sample -pr 400 600 -p 500 -j 600 100 -s 1 > data.gen

mksample2: all
	./sample -pr 400 600 -p 500 -t 50 -2 -t 100 1 -j 600 100 -s 1 > data.gen

