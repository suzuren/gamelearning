


TARGET = testModule

LIB=logger
OBJ=service_logger.o
OBJS=`ls *.o`

all:$(LIB) $(TARGET)

.c.o:
	gcc -O -fPIC -rdynamic $(G) -c $<

$(LIB): $(OBJ)
	gcc -shared $(G) -o $@.so $(OBJS)
	ar rvu $@.a $(OBJS)


$(TARGET) : main.c skynet_module.c
	gcc -g -Wall -o $@ $^ -lm -ldl -lrt

clean:
	rm -rf *.o *.so *.a $(TARGET)
