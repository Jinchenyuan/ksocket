#export LD_LIBRARY_PATH=../

linux:
	gcc test_client.c -o test_client.out -I../ -L../ -l ksock 
	gcc test_server.c -o test_server.out -I../ -L../ -l ksock 

OSX:
	gcc test_client.c -o test_client.out -I../ -L../ -l ksock
	gcc test_server.c -o test_server.out -I../ -L../ -l ksock

clean:
	rm -fr *.out