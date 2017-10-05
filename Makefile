all:
	@g++ *.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system -lm
	@./game
