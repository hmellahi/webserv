struct test
{
    explicit test(int i = 0) { }
    // test(test const&) implicitly declared here
};

struct test2 { };      // implicitly declared: test2(), test2(test2 const&)

void a(test d) { }
int main()
{
    test t;

    test copy(42);      // causes *definition* of the implicitly
                       // declared copy constructor

    a(test(42));
}