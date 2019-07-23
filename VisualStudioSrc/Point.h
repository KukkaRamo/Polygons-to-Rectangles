#pragma once
class Point
{
public:
	double x;
	double y;

	Point();
	Point(const double pX, const double pY);
	~Point();
	double distanceTo (const Point target) const;
	void moveTowards(const Point pTowards, const double howMuch);
};

