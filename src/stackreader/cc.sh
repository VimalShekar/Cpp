gcc -fno-stack-protector -m32 -z execstack -fno-pie -masm=intel -no-pie -o stackreader.out stackreader.c
