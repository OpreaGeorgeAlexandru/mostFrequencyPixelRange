all:
	g++ `pkg-config --cflags opencv` trainer.cpp `pkg-config --libs opencv` -o trainer
