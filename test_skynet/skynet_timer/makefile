

TARGET = testTimer

$(TARGET) : main.c skynet_timer.c
	gcc -g -Wall -o $@ $^ -lm -ldl -lrt

clean:
	rm -rf $(TARGET)
