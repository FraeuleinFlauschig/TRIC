CC = cc
CFLAGS = -Wall
OutputDir = build



test: $(OutputDir) $(OutputDir)/tric_test $(OutputDir)/tric_assert_test
	@ echo 'running tric self tests:';
	@ ./$(OutputDir)/tric_test && echo 'all tests ok';
	@ echo 'running tric assertion tests:';
	@ ./$(OutputDir)/tric_assert_test && echo 'all tests ok';



$(OutputDir):
	@ mkdir $(OutputDir);



$(OutputDir)/tric_test: tric_test.c ../tric.h
	@ echo 'building tric self tests';
	@ $(CC) $(CFLAGS) -o $@ $<;



$(OutputDir)/tric_assert_test: tric_assert_test.c ../tric.h ../tric_assert.h
	@ echo 'building tric assertion tests';
	@ $(CC) $(CFLAGS) -o $@ $<;



clean:
	@ if [ -d $(OutputDir) ]; then rm -r $(OutputDir); fi;



