PREF_LST_OBJ = ./list_obj/
PREF_LST_SRC = ./list_src/
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

$(TARGET_1): $(PREF_LST_SRC)main.o $(PREF_LST_SRC)list.o
	@ $(CC) $(LINK_FLAGS) $(PREF_LST_SRC)main.o $(PREF_LST_SRC)list.o -o $(TARGET_1) -L./lib -lstack  -I./lib

$(PREF_LST_SRC)main.o: $(PREF_LST_SRC)main.cpp
	@ $(CC) -c $(PREF_LST_SRC)main.cpp -o $(PREF_LST_SRC)main.o $(FLAGS)

$(PREF_LST_SRC)list.o: $(PREF_LST_SRC)list.cpp
	@ $(CC) -c $(PREF_LST_SRC)list.cpp -o $(PREF_LST_SRC)list.o $(FLAGS)

clean:
	rm $(TARGET_1) $(PREF_LST_SRC)*.o
