TARGET = kernel.elf
OBJS = cursored_textbox.o main.o textwindow.o graphics.o task.o fat.o syscall.o \
       newlib_support.o logger.o osbanner.o console.o asmfunc.o mouse.o timer.o \
	   libcxx_support.o layer.o interrupt.o segment.o paging.o memory_manager.o \
       window.o file.o frame_buffer.o keyboard.o pci.o terminal.o acpi.o \
	   \
       lang/font.o lang/zenkaku.o lang/textwindowbhat.o lang/textwindowpekzep.o \
	   lang/hankaku.o lang/praige_r_dict.o lang/fontpktk.o \
	   \
       usb/memory.o usb/device.o usb/xhci/ring.o usb/xhci/trb.o usb/xhci/xhci.o \
       usb/xhci/port.o usb/xhci/device.o usb/xhci/devmgr.o usb/xhci/registers.o \
       usb/classdriver/mouse.o usb/classdriver/hid.o usb/classdriver/keyboard.o \
       usb/classdriver/base.o
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

CPPFLAGS += -I.
CFLAGS   += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fshort-wchar
CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fshort-wchar \
            -fno-exceptions -fno-rtti -std=c++17
LDFLAGS  += --entry KernelMain -z norelro --image-base 0x100000 --static


.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

kernel.elf: $(OBJS) Makefile
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS) -lc -lc++ -lc++abi -lm \
	-lfreetype

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.%.d: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.c Makefile
	clang $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.%.d: %.c
	clang $(CPPFLAGS) $(CFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.c=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.asm Makefile
	nasm -f elf64 -o $@ $<

lang/hankaku.bin: lang/hankaku.txt
	../tools/makefont.py --width 8 --height 16 -o $@ $<

lang/hankaku.o: lang/hankaku.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@

lang/zenkaku.bin: lang/zenkaku.txt
	../tools/makefont.py --width 16 --height 16 -o $@ $<

lang/zenkaku.o: lang/zenkaku.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@

lang/praige_r_dict.cpp:
	../tools/download_and_convert_praige_dict.sh

.%.d: %.bin
	touch $@

.PHONY: depends
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)
