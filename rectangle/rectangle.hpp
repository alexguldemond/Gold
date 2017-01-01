namespace Gold {
  class rectangle {
  public:
    int x0, y0, x1, y1;

    rectangle();

    rectangle(int x0, int y0, int x1, int y1);

    ~rectangle();

    int getArea();

    void getSize(int* width, int* height);

    void move(int dx, int dy);
  };
}
