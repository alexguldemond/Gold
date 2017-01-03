/*********************************************//**
 * A class representing a rectangle in R2
 ************************************************/

namespace Gold {
  class rectangle {
  private:
    int x0, y0, x1, y1;
  public:

    /**********************************************//**
     *Contructor that takes 4 ints for the 4 corners of a rectangle
     *
     *Parameters: int x0, int y0, int x1, int y1
     *
     *x0, y0, x1, y1 => The four corners of the rectangle
     **************************************************/
    rectangle(int x0=0, int y0=0, int x1=0, int y1=0);

    ///Destructor
    ~rectangle();

    /**********************************************//**
     *getArea
     *
     *Description: Return the area of the rectangle
     *
     *Parameters: none
     *
     *Return value: int representing the area of the rectangle
     **************************************************/
    int getArea();

    /**********************************************//**
     *getSize
     *
     *Description: Get the width and the height of the rectangle
     *
     *Parameters: int& width, int& height
     *
     *width, height => Pointers to ints to hold the width and height
     *
     *Return value: void
     *************************************************/
    void getSize(int& width, int& height);

    /**********************************************//**
     *move
     *
     *Description: Move the rectangle
     *
     *Parameters: int dx, int dy
     *
     *dx, dy => The amount to move the rectangle
     *
     *Return value: void
     *************************************************/
    void move(int dx, int dy=0);
  };
}
