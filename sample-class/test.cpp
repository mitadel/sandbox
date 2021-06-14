
class Simplex {
  public:
    ~Simplex() {}

    // default constructor
    inline Simplex();

    // copy constructor
    inline Simplex(const Simplex &) = default;
    // inline Simplex(const Simplex &) = delete;

    // move constructor
    inline Simplex(Simplex &&) = default;
    // inline Simplex(const Simplex &&) = delete;

    // assignment operator
    inline Simplex & operator=(const Simplex &) = default;
    // inline const Simplex & operator=(const Simplex &) = delete;

    // move operator=
    inline Simplex & operator=(Simplex &&) = default;
    // inline const Simplex & operator=(const Simplex &&) = delete;
};

int
main()
{
    // default constructor
    Simplex s;
    // copy constructors
    Simplex a(s);
    Simplex b = s;
    // copy operator=
    Simplex c;
    c = s;
    // move constructor
    Simplex d(Simplex() /* temporary (has no name) */);
    Simplex e = Simplex() /* temporary (has no name) */;


    return 0;
}