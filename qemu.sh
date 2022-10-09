qemu-system-arm -cpu cortex-m3 -M lm3s811evb -kernel build/Kernel -serial stdio -serial tcp::10000,server,wait 
