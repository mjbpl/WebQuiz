INCS:=/home/mohitjain/indday/mylibs/include
LIBS:=libcgicc.a

all:	quizsubmit.img	

print-%  : ; @echo $* = $($*)

quizsubmit.img: quizsubmit.cpp 
	g++ -O2 --std=c++14 quizsubmit.cpp -o quizsubmit.img -I${INCS} -lpthread ${LIBS} $(CFLAGS)

clean:
	-rm quizsubmit.img 
