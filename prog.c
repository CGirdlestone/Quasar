int main(int argc, const char* argv[]){
    union {
        unsigned int x;
        const char* name;
    } as;

    typedef struct {
        int a;
        int b;
    } point;

    return 0;
}