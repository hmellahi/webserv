struct test
{
    test(int i = 0) { }
    // test(test const&) implicitly declared here
};

struct test2 { };      // implicitly declared: test2(), test2(test2 const&)

int main()
{
    
}