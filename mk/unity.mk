unity_src = $(wildcard $(UNITY_SRC)/*.c)
unity_obj = $(unity_src:.c=.o)

.PHONY: unity
$(unity_obj) : CFLAGS += -w
unity : CFLAGS += -w
unity : $(unity_obj)
