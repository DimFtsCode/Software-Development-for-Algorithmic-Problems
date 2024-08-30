# Μεταβλητές
CC = g++
CFLAGS = -Wall -std=c++11
SRC_DIR = src
OBJ_DIR = obj

# Στόχοι
all: lsh cube cluster

lsh: $(OBJ_DIR)/lsh.o $(OBJ_DIR)/lsh_main.o $(OBJ_DIR)/functions.o
	$(CC) $^ -o $@

cube: $(OBJ_DIR)/cube.o $(OBJ_DIR)/cube_main.o $(OBJ_DIR)/functions.o
	$(CC) $^ -o $@

cluster: $(OBJ_DIR)/clustering.o $(OBJ_DIR)/clustering_main.o $(OBJ_DIR)/functions.o $(OBJ_DIR)/lsh.o $(OBJ_DIR)/cube.o
	$(CC) $^ -o $@


# Κανόνες για την μεταγλώττιση των .cpp σε .o
$(OBJ_DIR)/lsh.o: $(SRC_DIR)/lsh.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lsh_main.o: $(SRC_DIR)/lsh_main.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/cube.o: $(SRC_DIR)/cube.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/cube_main.o: $(SRC_DIR)/cube_main.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/functions.o: $(SRC_DIR)/functions.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/clustering.o: $(SRC_DIR)/clustering.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/clustering_main.o: $(SRC_DIR)/clustering_main.cpp $(SRC_DIR)/my_header.h
	$(CC) $(CFLAGS) -c $< -o $@

# Καθαρισμός των αντικειμενικών αρχείων και των εκτελέσιμων
clean:
	rm -f $(OBJ_DIR)/*.o lsh cube cluster

# Ελέγξτε εάν υπάρχει ο φάκελος obj και δημιουργήστε τον εάν δεν υπάρχει
$(shell mkdir -p $(OBJ_DIR))
