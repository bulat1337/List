PREF_OBJ = ./obj/
CC = g++
TARGET_1 = lst.out
FLAGS = -D _DEBUG -ggdb3 \
    -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat        \
    -Wmissing-declarations -Wcast-qual -Wchar-subscripts  \
    -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
    -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual \
    -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
    -Wstrict-overflow=2 \
    -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef \
    -Wunreachable-code -Wunused -Wvariadic-macros \
    -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
    -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
    -fno-omit-frame-pointer -Wlarger-than=8192 \
    -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LINK_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

$(TARGET_1): $(PREF_OBJ)main.o $(PREF_OBJ)list.o
	@ $(CC) $(LINK_FLAGS) $(PREF_OBJ)main.o $(PREF_OBJ)list.o -o $(TARGET_1)
	@ $(CC) $(LINK_FLAGS) $(TARGET_1) -L./lib
	@ $(CC) $(LINK_FLAGS) $(TARGET_1) -lstack.a

$(PREF_OBJ)main.o: main.cpp
	@ $(CC) -c main.cpp -o $(PREF_OBJ)main.o $(FLAGS)

$(PREF_OBJ)list.o: list.cpp
	@ $(CC) -c list.cpp -o $(PREF_OBJ)list.o $(FLAGS)

clean:
	rm $(TARGET_1) $(PREF_OBJ)*.o
