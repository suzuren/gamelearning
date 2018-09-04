

TARGET = testMq

$(TARGET) : main.c skynet_mq.c
	gcc -g -Wall -o $@ $^ -lm -ldl

clean:
	rm -rf $(TARGET)
