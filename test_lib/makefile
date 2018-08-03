#-------------------------------------------------------------------------
.c.o:
	gcc -I$(INCL) -O -fPIC -rdynamic $(G) -c $<
# gcc -I$(INCL) -O -fPIC -c $<             # LINUX
# gcc -I$(INCL) -O -fPIC -bdynamic -c $<   # AIX
#-------------------------------------------------------------------------

APP_HOME=.
PTS_HOME=.
INCL=$(PTS_HOME)/incl
OBJ=testadd.o
OBJS=`ls $(PTS_HOME)/*.o `
LIB=testadd

all:$(LIB)
	@echo ""
	@echo "   Output Process Services Module for PTS.OPS Library Compilied£¡   "
	@echo "                          Version no.4.0                            "
	@echo ""

$(LIB): $(OBJ)
	#@mv *.o $(PTS_HOME)/obj/ops
	gcc -shared $(G) -o lib$@.so $(OBJS)
	ar rvu lib$@.a $(OBJS)
	#@mv lib$@.a  $(PTS_HOME)/lib
	#@mv lib$@.so $(PTS_HOME)/lib


clean:
	rm -f *.o *.so *.a


	