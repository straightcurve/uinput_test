.PHONY: all app clean

app:
	[ ! -d "build" ] && mkdir build && clang++ src/main.cpp -o build/app

clean:
	rm -f app
