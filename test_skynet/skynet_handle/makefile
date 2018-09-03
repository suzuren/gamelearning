

TARGET = testHandle

$(TARGET) : main.c skynet_handle.c
	gcc -g -Wall -o $@ $^ -lm -ldl

clean:
	rm -rf $(TARGET)
