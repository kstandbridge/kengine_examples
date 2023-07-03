extern long write(int, const char *, unsigned long);

int main(int argc, char *argv[])
{
    char str[] = "Hello.\n";

    write(1, str, 7);

    return 0;
}