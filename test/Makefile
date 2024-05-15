CC = cc
CFLAGS = -Wall
OutputDir = build



test: $(OutputDir) $(OutputDir)/tric_test
	@ echo 'running tric self tests:';
	@ ./$(OutputDir)/tric_test && echo 'all tests ok';



$(OutputDir):
	@ mkdir $(OutputDir);



$(OutputDir)/tric_test: tric_test.c ../tric.h
	@ echo 'building tric self tests';
	@ $(CC) $(CFLAGS) -o $@ $<;



clean:
	@ if [ -d $(OutputDir) ]; then rm -r $(OutputDir); fi;



