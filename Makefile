
all:
	cd src/server/; make; cd ../; cd client/; make; cd ../; cd display/; make; cd ../../;

clean:
	rm -rf bin/* src/common/*.o src/common/*~
	cd src/server/; make clean; cd ../; cd client/; make clean; cd ../; cd display/; make clean; cd ../../;