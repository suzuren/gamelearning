


TARGET = testSnluaBootstrap


$(TARGET) : main.c
	gcc -g -Wall -o $@ $^ -lm -ldl -lrt

clean:
	rm -rf *.o *.so *.a $(TARGET)
