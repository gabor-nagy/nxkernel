%.o: %.c %.h depend.d $(EXTRA_DEP)
	$(CC) $(CFLAGS) -c $<

%.o: %.s depend.d $(EXTRA_DEP)
	$(AS) $(ASFLAGS) -f coff $<

$(TARGET): $(OBJS) depend.d $(EXTRA_DEP)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY: dep
dep:
	$(CC) -MM *.c >depend.d

.PHONY: clean
clean:
	$(RM) $(RMFLAGS) *.o
	$(RM) $(RMFLAGS) depend.d

-include depend.d