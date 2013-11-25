
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File:			Vector2.h
// Author:			Vinessa Mayer
// Date Created:	November 2013
// Brief:			2D Vector Class Header, part of 3DMathsLibrary
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _VECTOR2_H_
#define _VECTOR2_H_
#include <string> 
#include <iostream>

class Vector2
{
public:
/////////////////////////////////////////////////////////////////////////////////////////////////////
///// CONSTRUCTORS AND DESTRUCTOR //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	Vector2(void); //Default
	Vector2(float a_X, float a_Y); //  Takes an X and a Y value (floats)
	~Vector2(void); // Destructor

/////////////////////////////////////////////////////////////////////////////////////////////////////
///// MEMBER FUNCTIONS /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	Vector2 operator + (float a_S); // Adds using a scaler
	Vector2 operator + (Vector2& a_OtherVector); // Adds one Vector to another Vector

	Vector2 operator - (float a_S); //Subtracts using a scaler
	Vector2 operator - (Vector2& a_OtherVector); // Subtracts one Vector from another Vector

	Vector2 operator * (float a_S); //Multiplies using a scaler

	void operator += (Vector2& a_OtherVector); //allows the use of  +=
	void operator -= (Vector2& a_OtherVector); // Allows the use of -=

	bool Equals(Vector2& a_OtherVector); // Tests to see if Vectors are equal
	float GetMagnitude2D();
	void Normalize();
	float EulerAngle(Vector2 &a_Vector2);
	Vector2 CrossProduct();


	float GetX(); // Returns X
	void SetX(float a_X); // Sets X

	float GetY(); // Returns Y
	void SetY(float a_Y); // Sets Y

	Vector2 GetNormal();
	float GetDotProduct2D(Vector2 &a_Vector2);
	
	

/////////////////////////////////////////////////////////////////////////////////////////////////////
///// MEMBER VARIABLES (Private) ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
private:
	float m_X;
	float m_Y;
};

#endif /*_VECTOR2_H_*/