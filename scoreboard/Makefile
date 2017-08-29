INCS:=/home/mohitjain/indday/mylibs/include
LIBS:=libcgicc.a

all:	scoreboard.img	

scoreboard.img: scoreboard.cpp 
	g++ -O2 --std=c++14 scoreboard.cpp -o scoreboard.img -I${INCS} -lpthread ${LIBS} $(CFLAGS)

clean:
	-rm scoreboard.img 
