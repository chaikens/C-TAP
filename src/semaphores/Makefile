all : install

semaphores : semopen semclose semup semdown

install:  semopen semclose semup semdown
	cp $^ ../..


%: %.c
	$(CC) -lpthread $< -o $@
