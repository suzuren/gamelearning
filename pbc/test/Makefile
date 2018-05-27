.PHONY: all help clean

PC = protoc
GAME_PROTO = $(wildcard ./*.proto)

text.pb : $(GAME_PROTO)
	$(PC) -o $@ -I. --include_imports $^

clean:
	rm -f *.pb