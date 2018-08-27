Linux:
	gcc ksock.c -fpic -shared -l pthread -o libksock.so

OSX:
	gcc ksock.c -fpic -shared -l pthread -o libksock.so

clean:
	rm -fr *.so