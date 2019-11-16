/*
 * Input:
 *  coords of two points on line
 *  (x0, y0), (x1, y1)
 * Output:
 *  coefficients A, B, C of general line equation
 *
 * return: none
 */
general_line_equation(double x0, y0, x1, y1, double *a, *b, *c)
{
	*a = y1 - y0;
	*b = x0 - x1;
	*c = x0 * (y0 - y1) + y0 * (x1 - x0);
}

/*
 * Input:
 *  rectangle corners coords: Ax, Ay, Bx, By
 *  point coords: Px, Py
 *
 *     +------------------------+ Ax,Ay
 *     |                        |
 *     |               *        |
 *     |               Px,Py    |
 *     +------------------------+
 *     Bx,By
 *
 * Sort x-coords of A and B, sort y-xoords of A and B
 * Then P is in rectangle if Ax <= Px <= Bx and Ay <= Py <= By 
 *
 * return 1 if point is in rectangle, 0 - otherwise
 */
int is_point_in_rec()
{
	/* sort Ax, Bx */

	/* sort Ay, By */

	if (Ax <= Px && Px <= Bx && Ay <= Py && Py <= By)
		return 1;
	return 0;
}

/*
 * Input:
 *  two lines
 * Output:
 *  coords of interseption point
 *
 *  \ 
 *   \ Px,Py
 * ---*---------
 *     \
 *      \
 *
 * general equation: aX + bY + c = 0
 *
 * equation by 2 points: (X - x0) / (x1 - x0) = (Y - y0) / (y1 - y0)

 * return 1 if lines intersect, 0 - otherwise
 */
intersect_point()
{

}

/*
 * Input:
 *  coords of three points on one line: Ax, Ay, Bx, By, Px, Py
 *
 * -----------*------*--------*-----
 *            A      P        B 
 *
 * return 1 if point P is between A and B, 0 - otherwise
 */
is_point_between_points()
{
}

/*
 * This handles 4 cases:
 *
 * 1. A and B are inside of the rectangle
 * +---------------------------------+
 * |                                 |
 * |    *-------------*              |
 * |    A             B              |
 * |                                 |
 * +---------------------------------+
 *
 * 2. The line fragment A,B does not intersect the rectange
 *                                   * A
 *                                    \
 * +---------------------------------+ \
 * |                                 |  \
 * |                                 |   \
 * |                                 |    \
 * |                                 |     * B
 * +---------------------------------+
 *
 * 3. The line fragment A,B
 *      *
 *       \
 * +------\--------------------------+
 * |       \                         |
 * |        \                        |
 * |         *                       |
 * |                                 |
 * +---------------------------------+
 *
 *                          *
 * 4.                      /
 * +----------------------/----------+
 * |                     /           |
 * |                    /            |
 * |                   /             |
 * |                  /              |
 * +-----------------/---------------+
 *                  /
 */
clipLine(double r1x, r1y, r2x, r2y,
	 double *ax, *ay, *bx, *by)
{
	
}
